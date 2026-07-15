#include <gtest/gtest.h>

#include "../src/Model/Order.h"
#include "../src/Model/OrderStatus.h"

using Model::InvalidOrderTransitionException;
using Model::Order;
using Model::OrderStatus;

namespace {

Order MakeOrder() {
    return Order("ORD-001", "SMP-001", "홍길동", 100);
}

}  // namespace

// ---- 생성 직후 상태 ----

TEST(OrderTest, 생성_직후_상태는_RESERVED이다) {
    Order order = MakeOrder();

    EXPECT_EQ(order.GetStatus(), OrderStatus::RESERVED);
}

TEST(OrderTest, 생성자로_설정한_값들이_getter로_정확히_조회된다) {
    Order order("ORD-001", "SMP-001", "홍길동", 100);

    EXPECT_EQ(order.GetOrderNo(), "ORD-001");
    EXPECT_EQ(order.GetSampleId(), "SMP-001");
    EXPECT_EQ(order.GetCustomerName(), "홍길동");
    EXPECT_EQ(order.GetQuantity(), 100);
}

// ---- 정상 전이 ----

TEST(OrderTest, Reject_호출시_RESERVED에서_REJECTED로_전환된다) {
    Order order = MakeOrder();

    order.Reject();

    EXPECT_EQ(order.GetStatus(), OrderStatus::REJECTED);
}

TEST(OrderTest, Confirm_호출시_RESERVED에서_CONFIRMED로_전환된다) {
    Order order = MakeOrder();

    order.Confirm();

    EXPECT_EQ(order.GetStatus(), OrderStatus::CONFIRMED);
}

TEST(OrderTest, StartProducing_호출시_RESERVED에서_PRODUCING으로_전환된다) {
    Order order = MakeOrder();

    order.StartProducing();

    EXPECT_EQ(order.GetStatus(), OrderStatus::PRODUCING);
}

TEST(OrderTest, CompleteProduction_호출시_PRODUCING에서_CONFIRMED로_전환된다) {
    Order order = MakeOrder();
    order.StartProducing();

    order.CompleteProduction();

    EXPECT_EQ(order.GetStatus(), OrderStatus::CONFIRMED);
}

TEST(OrderTest, Release_호출시_CONFIRMED에서_RELEASE로_전환된다) {
    Order order = MakeOrder();
    order.Confirm();

    order.Release();

    EXPECT_EQ(order.GetStatus(), OrderStatus::RELEASE);
}

// ---- 불법 전이 ----

TEST(OrderTest, RESERVED에서_CompleteProduction_호출시_예외를_던진다) {
    Order order = MakeOrder();

    EXPECT_THROW(order.CompleteProduction(), InvalidOrderTransitionException);
}

TEST(OrderTest, RESERVED에서_Release_호출시_예외를_던진다) {
    Order order = MakeOrder();

    EXPECT_THROW(order.Release(), InvalidOrderTransitionException);
}

TEST(OrderTest, REJECTED_상태에서_Confirm_호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.Reject();

    EXPECT_THROW(order.Confirm(), InvalidOrderTransitionException);
}

TEST(OrderTest, REJECTED_상태에서_StartProducing_호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.Reject();

    EXPECT_THROW(order.StartProducing(), InvalidOrderTransitionException);
}

TEST(OrderTest, REJECTED_상태에서_Reject_재호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.Reject();

    EXPECT_THROW(order.Reject(), InvalidOrderTransitionException);
}

TEST(OrderTest, REJECTED_상태에서_CompleteProduction_호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.Reject();

    EXPECT_THROW(order.CompleteProduction(), InvalidOrderTransitionException);
}

TEST(OrderTest, REJECTED_상태에서_Release_호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.Reject();

    EXPECT_THROW(order.Release(), InvalidOrderTransitionException);
}

TEST(OrderTest, PRODUCING에서_Reject_호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.StartProducing();

    EXPECT_THROW(order.Reject(), InvalidOrderTransitionException);
}

TEST(OrderTest, PRODUCING에서_StartProducing_재호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.StartProducing();

    EXPECT_THROW(order.StartProducing(), InvalidOrderTransitionException);
}

TEST(OrderTest, PRODUCING에서_Release_호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.StartProducing();

    EXPECT_THROW(order.Release(), InvalidOrderTransitionException);
}

TEST(OrderTest, CONFIRMED에서_Reject_호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.Confirm();

    EXPECT_THROW(order.Reject(), InvalidOrderTransitionException);
}

TEST(OrderTest, CONFIRMED에서_StartProducing_호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.Confirm();

    EXPECT_THROW(order.StartProducing(), InvalidOrderTransitionException);
}

TEST(OrderTest, CONFIRMED에서_CompleteProduction_호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.Confirm();

    EXPECT_THROW(order.CompleteProduction(), InvalidOrderTransitionException);
}

TEST(OrderTest, RELEASE_상태에서_Reject_호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.Confirm();
    order.Release();

    EXPECT_THROW(order.Reject(), InvalidOrderTransitionException);
}

TEST(OrderTest, RELEASE_상태에서_Confirm_호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.Confirm();
    order.Release();

    EXPECT_THROW(order.Confirm(), InvalidOrderTransitionException);
}

TEST(OrderTest, RELEASE_상태에서_StartProducing_호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.Confirm();
    order.Release();

    EXPECT_THROW(order.StartProducing(), InvalidOrderTransitionException);
}

TEST(OrderTest, RELEASE_상태에서_CompleteProduction_호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.Confirm();
    order.Release();

    EXPECT_THROW(order.CompleteProduction(), InvalidOrderTransitionException);
}

TEST(OrderTest, RELEASE_상태에서_Release_재호출시_예외를_던진다) {
    Order order = MakeOrder();
    order.Confirm();
    order.Release();

    EXPECT_THROW(order.Release(), InvalidOrderTransitionException);
}
