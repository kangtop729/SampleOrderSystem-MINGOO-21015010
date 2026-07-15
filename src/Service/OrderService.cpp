#include "OrderService.h"

#include <chrono>
#include <sstream>

namespace Service {

InvalidOrderException::InvalidOrderException(const std::string& message) : std::invalid_argument(message) {}

SampleNotFoundException::SampleNotFoundException(const std::string& message) : std::logic_error(message) {}

OrderNotFoundException::OrderNotFoundException(const std::string& message) : std::logic_error(message) {}

OrderService::OrderNumberGenerator OrderService::DefaultOrderNumberGenerator() {
    return [] {
        auto now = std::chrono::system_clock::now().time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
        std::ostringstream oss;
        oss << "ORD-" << millis;
        return oss.str();
    };
}

OrderService::OrderService(Repository::IOrderRepository& orderRepository, Repository::ISampleRepository& sampleRepository,
                            OrderNumberGenerator orderNumberGenerator)
    : orderRepository_(orderRepository),
      sampleRepository_(sampleRepository),
      orderNumberGenerator_(std::move(orderNumberGenerator)) {}

void OrderService::ValidatePlaceOrderFields(const std::string& customerName, int quantity) const {
    if (quantity <= kMinQuantityExclusive) {
        throw InvalidOrderException("quantity는 0보다 커야 합니다.");
    }
    if (customerName.empty()) {
        throw InvalidOrderException("customerName은 빈 문자열일 수 없습니다.");
    }
}

Model::Order OrderService::PlaceOrder(const std::string& sampleId, const std::string& customerName, int quantity) {
    if (!sampleRepository_.FindById(sampleId).has_value()) {
        throw SampleNotFoundException("존재하지 않는 sampleId입니다: " + sampleId);
    }

    ValidatePlaceOrderFields(customerName, quantity);

    Model::Order order(orderNumberGenerator_(), sampleId, customerName, quantity);
    orderRepository_.Save(order);
    return order;
}

Model::Order OrderService::FindOrderOrThrow(const std::string& orderNo) const {
    auto order = orderRepository_.FindById(orderNo);
    if (!order.has_value()) {
        throw OrderNotFoundException("존재하지 않는 orderNo입니다: " + orderNo);
    }
    return order.value();
}

void OrderService::ApproveOrder(const std::string& orderNo) {
    Model::Order order = FindOrderOrThrow(orderNo);

    auto sample = sampleRepository_.FindById(order.GetSampleId());
    if (!sample.has_value()) {
        throw SampleNotFoundException("존재하지 않는 sampleId입니다: " + order.GetSampleId());
    }

    if (sample->GetStock() >= order.GetQuantity()) {
        order.Confirm();
    } else {
        order.StartProducing();
    }

    orderRepository_.Update(order);
}

void OrderService::RejectOrder(const std::string& orderNo) {
    Model::Order order = FindOrderOrThrow(orderNo);
    order.Reject();
    orderRepository_.Update(order);
}

std::vector<Model::Order> OrderService::GetReservedOrders() const {
    return orderRepository_.FindByStatus(Model::OrderStatus::RESERVED);
}

}  // namespace Service
