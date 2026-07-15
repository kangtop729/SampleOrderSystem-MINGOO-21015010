#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "../src/Model/Order.h"
#include "../src/Model/OrderStatus.h"
#include "../src/Model/Sample.h"
#include "../src/Service/MonitoringService.h"
#include "mocks/MockOrderRepository.h"
#include "mocks/MockSampleRepository.h"

using Model::Order;
using Model::OrderStatus;
using Model::Sample;
using Service::MonitoringService;
using Service::OrderStatusCounts;
using Service::SampleStockStatus;
using Service::StockStatus;
using Test::MockOrderRepository;
using Test::MockSampleRepository;
using ::testing::Return;

class MonitoringServiceTest : public ::testing::Test {
protected:
    MockOrderRepository orderRepository_;
    MockSampleRepository sampleRepository_;
};

// ------------------------- CountOrdersByStatus -------------------------

TEST_F(MonitoringServiceTest, CountOrdersByStatus_상태별_개수를_정확히_집계하고_REJECTED는_집계하지_않는다) {
    std::vector<Order> reserved{Order("ORD-001", "SMP-001", "A", 5)};
    std::vector<Order> confirmed{Order("ORD-002", "SMP-001", "B", 3), Order("ORD-003", "SMP-001", "C", 2)};
    std::vector<Order> producing{};
    std::vector<Order> released{Order("ORD-004", "SMP-001", "D", 1), Order("ORD-005", "SMP-001", "E", 1),
                                 Order("ORD-006", "SMP-001", "F", 1)};

    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::RESERVED)).WillOnce(Return(reserved));
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::CONFIRMED)).WillOnce(Return(confirmed));
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::PRODUCING)).WillOnce(Return(producing));
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::RELEASE)).WillOnce(Return(released));
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::REJECTED)).Times(0);

    MonitoringService service(orderRepository_, sampleRepository_);

    OrderStatusCounts counts = service.CountOrdersByStatus();

    EXPECT_EQ(counts.reserved, 1);
    EXPECT_EQ(counts.confirmed, 2);
    EXPECT_EQ(counts.producing, 0);
    EXPECT_EQ(counts.released, 3);
}

// ------------------------- GetSampleStockStatuses -------------------------

TEST_F(MonitoringServiceTest, GetSampleStockStatuses_시료별로_재고상태를_SUFFICIENT_SHORTAGE_DEPLETED로_정확히_분류한다) {
    Sample sufficientSample("SMP-001", "충분시료", 30.0, 0.8, 10);
    Sample shortageSample("SMP-002", "부족시료", 30.0, 0.8, 5);
    Sample depletedSample("SMP-003", "고갈시료", 30.0, 0.8, 0);
    std::vector<Sample> samples{sufficientSample, shortageSample, depletedSample};

    EXPECT_CALL(sampleRepository_, FindAll()).WillOnce(Return(samples));

    std::vector<Order> sufficientDemand{Order("ORD-001", "SMP-001", "A", 5)};
    sufficientDemand[0].StartProducing();  // PRODUCING 상태로 만들어 pendingDemand에 포함되게 함

    std::vector<Order> shortageDemand{Order("ORD-002", "SMP-002", "B", 6)};  // RESERVED 상태 그대로

    std::vector<Order> depletedDemand{};  // 미해결 수요 없음

    EXPECT_CALL(orderRepository_, FindBySampleId(std::string("SMP-001"))).WillOnce(Return(sufficientDemand));
    EXPECT_CALL(orderRepository_, FindBySampleId(std::string("SMP-002"))).WillOnce(Return(shortageDemand));
    EXPECT_CALL(orderRepository_, FindBySampleId(std::string("SMP-003"))).WillOnce(Return(depletedDemand));

    MonitoringService service(orderRepository_, sampleRepository_);

    std::vector<SampleStockStatus> statuses = service.GetSampleStockStatuses();

    ASSERT_EQ(statuses.size(), 3u);

    EXPECT_EQ(statuses[0].sampleId, "SMP-001");
    EXPECT_EQ(statuses[0].stock, 10);
    EXPECT_EQ(statuses[0].pendingDemand, 5);
    EXPECT_EQ(statuses[0].status, StockStatus::SUFFICIENT);

    EXPECT_EQ(statuses[1].sampleId, "SMP-002");
    EXPECT_EQ(statuses[1].stock, 5);
    EXPECT_EQ(statuses[1].pendingDemand, 6);
    EXPECT_EQ(statuses[1].status, StockStatus::SHORTAGE);

    EXPECT_EQ(statuses[2].sampleId, "SMP-003");
    EXPECT_EQ(statuses[2].stock, 0);
    EXPECT_EQ(statuses[2].pendingDemand, 0);
    EXPECT_EQ(statuses[2].status, StockStatus::DEPLETED);
}

TEST_F(MonitoringServiceTest,
       GetSampleStockStatuses_pendingDemand는_RESERVED와_PRODUCING만_합산하고_나머지_상태는_제외한다) {
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 100);
    std::vector<Sample> samples{sample};
    EXPECT_CALL(sampleRepository_, FindAll()).WillOnce(Return(samples));

    Order reservedOrder("ORD-001", "SMP-001", "A", 3);  // RESERVED -> 포함

    Order producingOrder("ORD-002", "SMP-001", "B", 4);
    producingOrder.StartProducing();  // PRODUCING -> 포함

    Order confirmedOrder("ORD-003", "SMP-001", "C", 100);
    confirmedOrder.Confirm();  // CONFIRMED -> 제외

    Order releasedOrder("ORD-004", "SMP-001", "D", 100);
    releasedOrder.Confirm();
    releasedOrder.Release();  // RELEASE -> 제외

    Order rejectedOrder("ORD-005", "SMP-001", "E", 100);
    rejectedOrder.Reject();  // REJECTED -> 제외

    std::vector<Order> orders{reservedOrder, producingOrder, confirmedOrder, releasedOrder, rejectedOrder};
    EXPECT_CALL(orderRepository_, FindBySampleId(std::string("SMP-001"))).WillOnce(Return(orders));

    MonitoringService service(orderRepository_, sampleRepository_);

    std::vector<SampleStockStatus> statuses = service.GetSampleStockStatuses();

    ASSERT_EQ(statuses.size(), 1u);
    EXPECT_EQ(statuses[0].pendingDemand, 7);  // 3(RESERVED) + 4(PRODUCING)만 합산
}
