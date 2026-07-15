#include "Order.h"

#include <utility>

namespace Model {

InvalidOrderTransitionException::InvalidOrderTransitionException(const std::string& message)
    : std::logic_error(message) {}

Order::Order(std::string orderNo, std::string sampleId, std::string customerName, int quantity)
    : orderNo_(std::move(orderNo)),
      sampleId_(std::move(sampleId)),
      customerName_(std::move(customerName)),
      quantity_(quantity),
      status_(OrderStatus::RESERVED),
      createdAt_(std::chrono::system_clock::now()) {}

Order::Order(std::string orderNo, std::string sampleId, std::string customerName, int quantity, OrderStatus status,
             std::chrono::system_clock::time_point createdAt)
    : orderNo_(std::move(orderNo)),
      sampleId_(std::move(sampleId)),
      customerName_(std::move(customerName)),
      quantity_(quantity),
      status_(status),
      createdAt_(createdAt) {}

const std::string& Order::GetOrderNo() const {
    return orderNo_;
}

const std::string& Order::GetSampleId() const {
    return sampleId_;
}

const std::string& Order::GetCustomerName() const {
    return customerName_;
}

int Order::GetQuantity() const {
    return quantity_;
}

OrderStatus Order::GetStatus() const {
    return status_;
}

std::chrono::system_clock::time_point Order::GetCreatedAt() const {
    return createdAt_;
}

void Order::TransitionTo(OrderStatus requiredCurrentStatus, OrderStatus nextStatus) {
    if (status_ != requiredCurrentStatus) {
        throw InvalidOrderTransitionException("Order: 허용되지 않은 상태 전이입니다.");
    }
    status_ = nextStatus;
}

void Order::Reject() {
    TransitionTo(OrderStatus::RESERVED, OrderStatus::REJECTED);
}

void Order::Confirm() {
    TransitionTo(OrderStatus::RESERVED, OrderStatus::CONFIRMED);
}

void Order::StartProducing() {
    TransitionTo(OrderStatus::RESERVED, OrderStatus::PRODUCING);
}

void Order::CompleteProduction() {
    TransitionTo(OrderStatus::PRODUCING, OrderStatus::CONFIRMED);
}

void Order::Release() {
    TransitionTo(OrderStatus::CONFIRMED, OrderStatus::RELEASE);
}

}  // namespace Model
