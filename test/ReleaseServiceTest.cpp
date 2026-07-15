#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>
#include <vector>

#include "../src/Model/Order.h"
#include "../src/Model/OrderStatus.h"
#include "../src/Service/OrderService.h"
#include "../src/Service/ReleaseService.h"
#include "mocks/MockOrderRepository.h"

using Model::InvalidOrderTransitionException;
using Model::Order;
using Model::OrderStatus;
using Service::OrderNotFoundException;
using Service::ReleaseService;
using Test::MockOrderRepository;
using ::testing::_;
using ::testing::AllOf;
using ::testing::Eq;
using ::testing::Property;
using ::testing::Return;

namespace {

constexpr const char* kOrderNo = "ORD-001";

}  // namespace

class ReleaseServiceTest : public ::testing::Test {
protected:
    MockOrderRepository orderRepository_;
};

// ------------------------- GetConfirmedOrders -------------------------

TEST_F(ReleaseServiceTest, GetConfirmedOrders_repository의_FindByStatus_CONFIRMED_결과를_그대로_반환한다) {
    std::vector<Order> expected{Order("ORD-001", "SMP-001", "A", 5), Order("ORD-002", "SMP-002", "B", 3)};
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::CONFIRMED)).WillOnce(Return(expected));

    ReleaseService service(orderRepository_);

    std::vector<Order> actual = service.GetConfirmedOrders();

    ASSERT_EQ(actual.size(), 2u);
    EXPECT_EQ(actual[0].GetOrderNo(), "ORD-001");
    EXPECT_EQ(actual[1].GetOrderNo(), "ORD-002");
}

// ------------------------- ReleaseOrder: 정상 케이스 -------------------------

TEST_F(ReleaseServiceTest, ReleaseOrder_CONFIRMED상태_주문을_출고하면_RELEASE상태로_전환되고_Update가_한번_호출된다) {
    Order order(kOrderNo, "SMP-001", "홍길동", 10);
    order.Confirm();
    EXPECT_CALL(orderRepository_, FindById(std::string(kOrderNo))).WillOnce(Return(order));
    EXPECT_CALL(orderRepository_,
                Update(AllOf(Property(&Order::GetOrderNo, Eq(std::string(kOrderNo))),
                              Property(&Order::GetStatus, Eq(OrderStatus::RELEASE)))))
        .WillOnce(Return(true));

    ReleaseService service(orderRepository_);

    service.ReleaseOrder(kOrderNo);
}

// ------------------------- ReleaseOrder: orderNo 미존재 -------------------------

TEST_F(ReleaseServiceTest, ReleaseOrder_존재하지않는_orderNo면_OrderNotFoundException을_던지고_Update는_호출되지_않는다) {
    EXPECT_CALL(orderRepository_, FindById(std::string("ORD-999"))).WillOnce(Return(std::nullopt));
    EXPECT_CALL(orderRepository_, Update(_)).Times(0);

    ReleaseService service(orderRepository_);

    EXPECT_THROW(service.ReleaseOrder("ORD-999"), OrderNotFoundException);
}

// ------------------------- ReleaseOrder: CONFIRMED가 아닌 상태 -------------------------

TEST_F(ReleaseServiceTest, ReleaseOrder_RESERVED상태_주문에_출고시도하면_InvalidOrderTransitionException이_전파되고_Update는_호출되지_않는다) {
    Order order(kOrderNo, "SMP-001", "홍길동", 10);
    EXPECT_CALL(orderRepository_, FindById(std::string(kOrderNo))).WillOnce(Return(order));
    EXPECT_CALL(orderRepository_, Update(_)).Times(0);

    ReleaseService service(orderRepository_);

    EXPECT_THROW(service.ReleaseOrder(kOrderNo), InvalidOrderTransitionException);
}

TEST_F(ReleaseServiceTest, ReleaseOrder_PRODUCING상태_주문에_출고시도하면_InvalidOrderTransitionException이_전파되고_Update는_호출되지_않는다) {
    Order order(kOrderNo, "SMP-001", "홍길동", 10);
    order.StartProducing();
    EXPECT_CALL(orderRepository_, FindById(std::string(kOrderNo))).WillOnce(Return(order));
    EXPECT_CALL(orderRepository_, Update(_)).Times(0);

    ReleaseService service(orderRepository_);

    EXPECT_THROW(service.ReleaseOrder(kOrderNo), InvalidOrderTransitionException);
}

TEST_F(ReleaseServiceTest, ReleaseOrder_이미_RELEASE상태인_주문에_재출고시도하면_InvalidOrderTransitionException이_전파되고_Update는_호출되지_않는다) {
    Order order(kOrderNo, "SMP-001", "홍길동", 10);
    order.Confirm();
    order.Release();
    EXPECT_CALL(orderRepository_, FindById(std::string(kOrderNo))).WillOnce(Return(order));
    EXPECT_CALL(orderRepository_, Update(_)).Times(0);

    ReleaseService service(orderRepository_);

    EXPECT_THROW(service.ReleaseOrder(kOrderNo), InvalidOrderTransitionException);
}
