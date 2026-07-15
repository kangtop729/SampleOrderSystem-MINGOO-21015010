#include <gtest/gtest.h>

#include <ctime>
#include <iomanip>
#include <sstream>

#include "../src/Model/Order.h"
#include "../src/Model/OrderStatus.h"
#include "../src/Serialization/OrderSerialization.h"
#include "../src/ThirdParty/nlohmann/json.hpp"

using Model::Order;
using Model::OrderStatus;

namespace {

Order MakeOrder() {
    return Order("ORD-0001", "SMP-001", "ACME Corp", 50);
}

// createdAt은 초 단위까지만 JSON에 보존되므로, 비교 시 초 단위로 절삭한 문자열로 비교한다.
std::string ToIso8601Seconds(std::chrono::system_clock::time_point tp) {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm tmValue{};
    gmtime_s(&tmValue, &time);
    std::ostringstream oss;
    oss << std::put_time(&tmValue, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

void ExpectRoundTripPreservesFields(const Order& original) {
    nlohmann::json json = OrderSerialization::ToJson(original);
    Order restored = OrderSerialization::FromJson(json);

    EXPECT_EQ(restored.GetOrderNo(), original.GetOrderNo());
    EXPECT_EQ(restored.GetSampleId(), original.GetSampleId());
    EXPECT_EQ(restored.GetCustomerName(), original.GetCustomerName());
    EXPECT_EQ(restored.GetQuantity(), original.GetQuantity());
    EXPECT_EQ(restored.GetStatus(), original.GetStatus());
    EXPECT_EQ(ToIso8601Seconds(restored.GetCreatedAt()), ToIso8601Seconds(original.GetCreatedAt()));
}

}  // namespace

// ---- ToJson: 스키마 필드 확인 ----

TEST(OrderSerializationTest, ToJson_결과에_스키마에_정의된_필드가_모두_포함된다) {
    Order order = MakeOrder();

    nlohmann::json json = OrderSerialization::ToJson(order);

    EXPECT_EQ(json.at("orderNo").get<std::string>(), "ORD-0001");
    EXPECT_EQ(json.at("sampleId").get<std::string>(), "SMP-001");
    EXPECT_EQ(json.at("customerName").get<std::string>(), "ACME Corp");
    EXPECT_EQ(json.at("quantity").get<int>(), 50);
    EXPECT_EQ(json.at("status").get<std::string>(), "RESERVED");
    EXPECT_TRUE(json.contains("createdAt"));
}

TEST(OrderSerializationTest, ToJson_결과의_status는_enum_이름_문자열이다) {
    Order order = MakeOrder();
    order.StartProducing();

    nlohmann::json json = OrderSerialization::ToJson(order);

    EXPECT_EQ(json.at("status").get<std::string>(), "PRODUCING");
}

// ---- ToJson -> FromJson 라운드트립: 상태별 ----

TEST(OrderSerializationTest, ToJson_FromJson_라운드트립시_RESERVED_주문이_정확히_보존된다) {
    Order order = MakeOrder();

    ExpectRoundTripPreservesFields(order);
}

TEST(OrderSerializationTest, ToJson_FromJson_라운드트립시_REJECTED_주문이_정확히_보존된다) {
    Order order = MakeOrder();
    order.Reject();

    ExpectRoundTripPreservesFields(order);
}

TEST(OrderSerializationTest, ToJson_FromJson_라운드트립시_PRODUCING_주문이_정확히_보존된다) {
    Order order = MakeOrder();
    order.StartProducing();

    ExpectRoundTripPreservesFields(order);
}

TEST(OrderSerializationTest, ToJson_FromJson_라운드트립시_CONFIRMED_주문이_정확히_보존된다) {
    Order order = MakeOrder();
    order.Confirm();

    ExpectRoundTripPreservesFields(order);
}

TEST(OrderSerializationTest, ToJson_FromJson_라운드트립시_생산완료를_거친_CONFIRMED_주문이_정확히_보존된다) {
    Order order = MakeOrder();
    order.StartProducing();
    order.CompleteProduction();

    ExpectRoundTripPreservesFields(order);
}

TEST(OrderSerializationTest, ToJson_FromJson_라운드트립시_RELEASE_주문이_정확히_보존된다) {
    Order order = MakeOrder();
    order.Confirm();
    order.Release();

    ExpectRoundTripPreservesFields(order);
}
