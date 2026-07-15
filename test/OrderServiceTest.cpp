#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>
#include <vector>

#include "../src/Model/Order.h"
#include "../src/Model/OrderStatus.h"
#include "../src/Model/Sample.h"
#include "../src/Service/OrderService.h"
#include "mocks/MockOrderRepository.h"
#include "mocks/MockSampleRepository.h"

using Model::InvalidOrderTransitionException;
using Model::Order;
using Model::OrderStatus;
using Model::Sample;
using Service::InvalidOrderException;
using Service::OrderNotFoundException;
using Service::OrderService;
using Service::SampleNotFoundException;
using Test::MockOrderRepository;
using Test::MockSampleRepository;
using ::testing::_;
using ::testing::AllOf;
using ::testing::Eq;
using ::testing::Property;
using ::testing::Return;

namespace {

constexpr const char* kOrderNo = "ORD-001";

auto OrderFieldsAre(const std::string& sampleId, const std::string& customerName, int quantity,
                     OrderStatus status) {
    return AllOf(Property(&Order::GetSampleId, Eq(sampleId)), Property(&Order::GetCustomerName, Eq(customerName)),
                 Property(&Order::GetQuantity, Eq(quantity)), Property(&Order::GetStatus, Eq(status)));
}

}  // namespace

class OrderServiceTest : public ::testing::Test {
protected:
    void ExpectOrderRepositoryNotCalled() {
        EXPECT_CALL(orderRepository_, Save(_)).Times(0);
        EXPECT_CALL(orderRepository_, Update(_)).Times(0);
    }

    MockOrderRepository orderRepository_;
    MockSampleRepository sampleRepository_;
};

// ------------------------- PlaceOrder -------------------------

TEST_F(OrderServiceTest, PlaceOrder_유효한값이면_RESERVED상태의_Order를_생성하고_Save가_한번_호출된다) {
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 10);
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillOnce(Return(sample));
    EXPECT_CALL(orderRepository_, Save(OrderFieldsAre("SMP-001", "홍길동", 5, OrderStatus::RESERVED))).Times(1);

    OrderService service(orderRepository_, sampleRepository_, [] { return std::string(kOrderNo); });

    Order order = service.PlaceOrder("SMP-001", "홍길동", 5);

    EXPECT_EQ(order.GetOrderNo(), kOrderNo);
    EXPECT_EQ(order.GetSampleId(), "SMP-001");
    EXPECT_EQ(order.GetCustomerName(), "홍길동");
    EXPECT_EQ(order.GetQuantity(), 5);
    EXPECT_EQ(order.GetStatus(), OrderStatus::RESERVED);
}

TEST_F(OrderServiceTest, PlaceOrder_존재하지않는_sampleId면_SampleNotFoundException을_던지고_Save는_호출되지_않는다) {
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-999"))).WillOnce(Return(std::nullopt));
    ExpectOrderRepositoryNotCalled();

    OrderService service(orderRepository_, sampleRepository_, [] { return std::string(kOrderNo); });

    EXPECT_THROW(service.PlaceOrder("SMP-999", "홍길동", 5), SampleNotFoundException);
}

TEST_F(OrderServiceTest, PlaceOrder_quantity가_0이면_InvalidOrderException을_던지고_Save는_호출되지_않는다) {
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 10);
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillOnce(Return(sample));
    ExpectOrderRepositoryNotCalled();

    OrderService service(orderRepository_, sampleRepository_, [] { return std::string(kOrderNo); });

    EXPECT_THROW(service.PlaceOrder("SMP-001", "홍길동", 0), InvalidOrderException);
}

TEST_F(OrderServiceTest, PlaceOrder_quantity가_음수면_InvalidOrderException을_던지고_Save는_호출되지_않는다) {
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 10);
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillOnce(Return(sample));
    ExpectOrderRepositoryNotCalled();

    OrderService service(orderRepository_, sampleRepository_, [] { return std::string(kOrderNo); });

    EXPECT_THROW(service.PlaceOrder("SMP-001", "홍길동", -1), InvalidOrderException);
}

TEST_F(OrderServiceTest, PlaceOrder_customerName이_빈문자열이면_InvalidOrderException을_던지고_Save는_호출되지_않는다) {
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 10);
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillOnce(Return(sample));
    ExpectOrderRepositoryNotCalled();

    OrderService service(orderRepository_, sampleRepository_, [] { return std::string(kOrderNo); });

    EXPECT_THROW(service.PlaceOrder("SMP-001", "", 5), InvalidOrderException);
}

// ------------------------- ApproveOrder -------------------------

TEST_F(OrderServiceTest, ApproveOrder_재고가_주문수량과_정확히_같으면_CONFIRMED로_전환된다) {
    Order order(kOrderNo, "SMP-001", "홍길동", 10);
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 10);
    EXPECT_CALL(orderRepository_, FindById(std::string(kOrderNo))).WillOnce(Return(order));
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillOnce(Return(sample));
    EXPECT_CALL(orderRepository_, Update(Property(&Order::GetStatus, Eq(OrderStatus::CONFIRMED)))).WillOnce(Return(true));

    OrderService service(orderRepository_, sampleRepository_);

    Model::OrderStatus result = service.ApproveOrder(kOrderNo);

    EXPECT_EQ(result, Model::OrderStatus::CONFIRMED);
}

TEST_F(OrderServiceTest, ApproveOrder_재고가_주문수량보다_1개_부족하면_PRODUCING으로_전환된다) {
    Order order(kOrderNo, "SMP-001", "홍길동", 10);
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 9);
    EXPECT_CALL(orderRepository_, FindById(std::string(kOrderNo))).WillOnce(Return(order));
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillOnce(Return(sample));
    EXPECT_CALL(orderRepository_, Update(Property(&Order::GetStatus, Eq(OrderStatus::PRODUCING)))).WillOnce(Return(true));

    OrderService service(orderRepository_, sampleRepository_);

    Model::OrderStatus result = service.ApproveOrder(kOrderNo);

    EXPECT_EQ(result, Model::OrderStatus::PRODUCING);
}

TEST_F(OrderServiceTest, ApproveOrder_재고가_0이고_주문수량이_있으면_전량부족으로_PRODUCING으로_전환된다) {
    Order order(kOrderNo, "SMP-001", "홍길동", 10);
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 0);
    EXPECT_CALL(orderRepository_, FindById(std::string(kOrderNo))).WillOnce(Return(order));
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillOnce(Return(sample));
    EXPECT_CALL(orderRepository_, Update(Property(&Order::GetStatus, Eq(OrderStatus::PRODUCING)))).WillOnce(Return(true));

    OrderService service(orderRepository_, sampleRepository_);

    Model::OrderStatus result = service.ApproveOrder(kOrderNo);

    EXPECT_EQ(result, Model::OrderStatus::PRODUCING);
}

TEST_F(OrderServiceTest, ApproveOrder_재고가_주문수량보다_여유있으면_CONFIRMED로_전환된다) {
    Order order(kOrderNo, "SMP-001", "홍길동", 10);
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 100);
    EXPECT_CALL(orderRepository_, FindById(std::string(kOrderNo))).WillOnce(Return(order));
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillOnce(Return(sample));
    EXPECT_CALL(orderRepository_, Update(Property(&Order::GetStatus, Eq(OrderStatus::CONFIRMED)))).WillOnce(Return(true));

    OrderService service(orderRepository_, sampleRepository_);

    Model::OrderStatus result = service.ApproveOrder(kOrderNo);

    EXPECT_EQ(result, Model::OrderStatus::CONFIRMED);
}

TEST_F(OrderServiceTest, ApproveOrder_존재하지않는_orderNo면_OrderNotFoundException을_던진다) {
    EXPECT_CALL(orderRepository_, FindById(std::string("ORD-999"))).WillOnce(Return(std::nullopt));

    OrderService service(orderRepository_, sampleRepository_);

    EXPECT_THROW(service.ApproveOrder("ORD-999"), OrderNotFoundException);
}

TEST_F(OrderServiceTest, ApproveOrder_이미_CONFIRMED인_주문에_재승인시도하면_InvalidOrderTransitionException이_전파된다) {
    Order order(kOrderNo, "SMP-001", "홍길동", 10);
    order.Confirm();
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 10);
    EXPECT_CALL(orderRepository_, FindById(std::string(kOrderNo))).WillOnce(Return(order));
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillOnce(Return(sample));
    EXPECT_CALL(orderRepository_, Update(_)).Times(0);

    OrderService service(orderRepository_, sampleRepository_);

    EXPECT_THROW(service.ApproveOrder(kOrderNo), InvalidOrderTransitionException);
}

// ------------------------- RejectOrder -------------------------

TEST_F(OrderServiceTest, RejectOrder_정상거절하면_REJECTED상태로_전환되고_Update가_호출된다) {
    Order order(kOrderNo, "SMP-001", "홍길동", 10);
    EXPECT_CALL(orderRepository_, FindById(std::string(kOrderNo))).WillOnce(Return(order));
    EXPECT_CALL(orderRepository_, Update(Property(&Order::GetStatus, Eq(OrderStatus::REJECTED)))).WillOnce(Return(true));

    OrderService service(orderRepository_, sampleRepository_);

    service.RejectOrder(kOrderNo);
}

TEST_F(OrderServiceTest, RejectOrder_존재하지않는_orderNo면_OrderNotFoundException을_던진다) {
    EXPECT_CALL(orderRepository_, FindById(std::string("ORD-999"))).WillOnce(Return(std::nullopt));

    OrderService service(orderRepository_, sampleRepository_);

    EXPECT_THROW(service.RejectOrder("ORD-999"), OrderNotFoundException);
}

// ------------------------- GetReservedOrders -------------------------

TEST_F(OrderServiceTest, GetReservedOrders_repository의_FindByStatus_RESERVED_결과를_그대로_반환한다) {
    std::vector<Order> expected{Order("ORD-001", "SMP-001", "A", 5), Order("ORD-002", "SMP-002", "B", 3)};
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::RESERVED)).WillOnce(Return(expected));

    OrderService service(orderRepository_, sampleRepository_);

    std::vector<Order> actual = service.GetReservedOrders();

    ASSERT_EQ(actual.size(), 2u);
    EXPECT_EQ(actual[0].GetOrderNo(), "ORD-001");
    EXPECT_EQ(actual[1].GetOrderNo(), "ORD-002");
}
