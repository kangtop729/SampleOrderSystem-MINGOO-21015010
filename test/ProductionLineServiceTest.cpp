#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "../src/Model/Order.h"
#include "../src/Model/OrderStatus.h"
#include "../src/Model/ProductionJob.h"
#include "../src/Model/Sample.h"
#include "../src/Service/ProductionLineService.h"
#include "mocks/MockOrderRepository.h"
#include "mocks/MockSampleRepository.h"

using Model::Order;
using Model::OrderStatus;
using Model::ProductionJob;
using Model::Sample;
using Service::NoProductionJobException;
using Service::ProductionLineService;
using Test::MockOrderRepository;
using Test::MockSampleRepository;
using ::testing::_;
using ::testing::AllOf;
using ::testing::Eq;
using ::testing::Property;
using ::testing::Return;

namespace {

// 두 Order가 서로 다른 createdAt을 갖도록 생성 사이에 약간의 지연을 둔다.
Order MakeProducingOrder(const std::string& orderNo, const std::string& sampleId, const std::string& customerName,
                          int quantity) {
    Order order(orderNo, sampleId, customerName, quantity);
    order.StartProducing();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    return order;
}

auto SampleFieldsAre(const std::string& sampleId, int stock) {
    return AllOf(Property(&Sample::GetSampleId, Eq(sampleId)), Property(&Sample::GetStock, Eq(stock)));
}

auto OrderFieldsAre(const std::string& orderNo, OrderStatus status) {
    return AllOf(Property(&Order::GetOrderNo, Eq(orderNo)), Property(&Order::GetStatus, Eq(status)));
}

}  // namespace

class ProductionLineServiceTest : public ::testing::Test {
protected:
    void ExpectNoRepositoryUpdates() {
        EXPECT_CALL(orderRepository_, Update(_)).Times(0);
        EXPECT_CALL(sampleRepository_, Update(_)).Times(0);
    }

    MockOrderRepository orderRepository_;
    MockSampleRepository sampleRepository_;
};

// ------------------------- GetQueue -------------------------

TEST_F(ProductionLineServiceTest, GetQueue_여러주문이_섞인순서로_반환되어도_createdAt_오름차순으로_정렬된다) {
    // sample1: SMP-001, stock=5, yield=0.5, averageProductionTimeMinutes=2.0
    Sample sample1("SMP-001", "시료A", 2.0, 0.5, 5);
    // sample2: SMP-002, stock=3, yield=1.0, averageProductionTimeMinutes=1.0
    Sample sample2("SMP-002", "시료B", 1.0, 1.0, 3);

    Order order1 = MakeProducingOrder("ORD-100", "SMP-001", "홍길동", 10);  // shortfall=5, actualQty=10, minutes=20.0
    Order order2 = MakeProducingOrder("ORD-200", "SMP-002", "김철수", 3);   // shortfall=0, actualQty=0, minutes=0.0
    Order order3 = MakeProducingOrder("ORD-300", "SMP-001", "이영희", 7);   // shortfall=2, actualQty=4, minutes=8.0

    // 뒤섞인 순서로 반환되어도 createdAt 기준 오름차순(order1, order2, order3)으로 정렬되어야 한다.
    std::vector<Order> shuffled{order3, order1, order2};
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::PRODUCING)).WillOnce(Return(shuffled));
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillRepeatedly(Return(sample1));
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-002"))).WillRepeatedly(Return(sample2));

    ProductionLineService service(orderRepository_, sampleRepository_);

    std::vector<ProductionJob> queue = service.GetQueue();

    ASSERT_EQ(queue.size(), 3u);

    EXPECT_EQ(queue[0].GetOrderNo(), "ORD-100");
    EXPECT_EQ(queue[0].GetShortfall(), 5);
    EXPECT_EQ(queue[0].GetActualProductionQty(), 10);
    EXPECT_DOUBLE_EQ(queue[0].GetTotalProductionMinutes(), 20.0);

    EXPECT_EQ(queue[1].GetOrderNo(), "ORD-200");
    EXPECT_EQ(queue[1].GetShortfall(), 0);
    EXPECT_EQ(queue[1].GetActualProductionQty(), 0);
    EXPECT_DOUBLE_EQ(queue[1].GetTotalProductionMinutes(), 0.0);

    EXPECT_EQ(queue[2].GetOrderNo(), "ORD-300");
    EXPECT_EQ(queue[2].GetShortfall(), 2);
    EXPECT_EQ(queue[2].GetActualProductionQty(), 4);
    EXPECT_DOUBLE_EQ(queue[2].GetTotalProductionMinutes(), 8.0);
}

TEST_F(ProductionLineServiceTest, GetQueue_대기중인_주문이_없으면_빈벡터를_반환한다) {
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::PRODUCING)).WillOnce(Return(std::vector<Order>{}));

    ProductionLineService service(orderRepository_, sampleRepository_);

    std::vector<ProductionJob> queue = service.GetQueue();

    EXPECT_TRUE(queue.empty());
}

// ------------------------- GetCurrentJob -------------------------

TEST_F(ProductionLineServiceTest, GetCurrentJob_큐에_항목이_있으면_가장_오래된_주문을_반환한다) {
    Sample sample1("SMP-001", "시료A", 2.0, 0.5, 5);

    Order order1 = MakeProducingOrder("ORD-100", "SMP-001", "홍길동", 10);
    Order order2 = MakeProducingOrder("ORD-200", "SMP-001", "김철수", 8);

    std::vector<Order> shuffled{order2, order1};
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::PRODUCING)).WillOnce(Return(shuffled));
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillRepeatedly(Return(sample1));

    ProductionLineService service(orderRepository_, sampleRepository_);

    ProductionJob currentJob = service.GetCurrentJob();

    EXPECT_EQ(currentJob.GetOrderNo(), "ORD-100");
}

TEST_F(ProductionLineServiceTest, GetCurrentJob_큐가_비어있으면_NoProductionJobException을_던진다) {
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::PRODUCING)).WillOnce(Return(std::vector<Order>{}));

    ProductionLineService service(orderRepository_, sampleRepository_);

    EXPECT_THROW(service.GetCurrentJob(), NoProductionJobException);
}

// ------------------------- CompleteCurrentJob -------------------------

TEST_F(ProductionLineServiceTest, CompleteCurrentJob_정상케이스면_재고증가와_주문상태전환이_반영되어_Update된다) {
    // shortfall = 10-5 = 5, actualQty = ceil(5/0.5) = 10 -> sample 재고는 5+10=15가 되어야 한다.
    Sample sample1("SMP-001", "시료A", 2.0, 0.5, 5);

    Order order1 = MakeProducingOrder("ORD-100", "SMP-001", "홍길동", 10);
    Order order2 = MakeProducingOrder("ORD-200", "SMP-001", "김철수", 8);

    std::vector<Order> shuffled{order2, order1};
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::PRODUCING)).WillOnce(Return(shuffled));
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillRepeatedly(Return(sample1));
    EXPECT_CALL(sampleRepository_, Update(SampleFieldsAre("SMP-001", 15))).WillOnce(Return(true));
    EXPECT_CALL(orderRepository_, Update(OrderFieldsAre("ORD-100", OrderStatus::CONFIRMED))).WillOnce(Return(true));

    ProductionLineService service(orderRepository_, sampleRepository_);

    service.CompleteCurrentJob();
}

TEST_F(ProductionLineServiceTest, CompleteCurrentJob_큐가_비어있으면_NoProductionJobException을_던지고_Update는_호출되지_않는다) {
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::PRODUCING)).WillOnce(Return(std::vector<Order>{}));
    ExpectNoRepositoryUpdates();

    ProductionLineService service(orderRepository_, sampleRepository_);

    EXPECT_THROW(service.CompleteCurrentJob(), NoProductionJobException);
}
