#include "OrderSerialization.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using Model::Order;
using Model::OrderStatus;

namespace {

const char* const kIso8601Format = "%Y-%m-%dT%H:%M:%SZ";

}  // namespace

nlohmann::json OrderSerialization::ToJson(const Order& order) {
    return nlohmann::json{{"orderNo", order.orderNo_},
                           {"sampleId", order.sampleId_},
                           {"customerName", order.customerName_},
                           {"quantity", order.quantity_},
                           {"status", StatusToString(order.status_)},
                           {"createdAt", CreatedAtToIso8601(order.createdAt_)}};
}

Order OrderSerialization::FromJson(const nlohmann::json& json) {
    return Order(json.at("orderNo").get<std::string>(), json.at("sampleId").get<std::string>(),
                 json.at("customerName").get<std::string>(), json.at("quantity").get<int>(),
                 StatusFromString(json.at("status").get<std::string>()),
                 CreatedAtFromIso8601(json.at("createdAt").get<std::string>()));
}

std::string OrderSerialization::StatusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::RESERVED:
            return "RESERVED";
        case OrderStatus::REJECTED:
            return "REJECTED";
        case OrderStatus::PRODUCING:
            return "PRODUCING";
        case OrderStatus::CONFIRMED:
            return "CONFIRMED";
        case OrderStatus::RELEASE:
            return "RELEASE";
    }
    throw std::invalid_argument("OrderSerialization: 알 수 없는 OrderStatus입니다.");
}

OrderStatus OrderSerialization::StatusFromString(const std::string& statusName) {
    if (statusName == "RESERVED") {
        return OrderStatus::RESERVED;
    }
    if (statusName == "REJECTED") {
        return OrderStatus::REJECTED;
    }
    if (statusName == "PRODUCING") {
        return OrderStatus::PRODUCING;
    }
    if (statusName == "CONFIRMED") {
        return OrderStatus::CONFIRMED;
    }
    if (statusName == "RELEASE") {
        return OrderStatus::RELEASE;
    }
    throw std::invalid_argument("OrderSerialization: 알 수 없는 상태 문자열입니다: " + statusName);
}

std::string OrderSerialization::CreatedAtToIso8601(std::chrono::system_clock::time_point createdAt) {
    std::time_t time = std::chrono::system_clock::to_time_t(createdAt);
    std::tm tmValue{};
    gmtime_s(&tmValue, &time);
    std::ostringstream oss;
    oss << std::put_time(&tmValue, kIso8601Format);
    return oss.str();
}

std::chrono::system_clock::time_point OrderSerialization::CreatedAtFromIso8601(const std::string& iso8601) {
    std::tm tmValue{};
    std::istringstream iss(iso8601);
    iss >> std::get_time(&tmValue, kIso8601Format);
    if (iss.fail()) {
        throw std::invalid_argument("OrderSerialization: createdAt 형식이 올바르지 않습니다: " + iso8601);
    }
    std::time_t time = _mkgmtime(&tmValue);
    return std::chrono::system_clock::from_time_t(time);
}
