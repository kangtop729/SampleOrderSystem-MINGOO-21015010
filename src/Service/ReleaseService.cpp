#include "ReleaseService.h"

namespace Service {

ReleaseService::ReleaseService(Repository::IOrderRepository& orderRepository) : orderRepository_(orderRepository) {}

Model::Order ReleaseService::FindOrderOrThrow(const std::string& orderNo) const {
    auto order = orderRepository_.FindById(orderNo);
    if (!order.has_value()) {
        throw OrderNotFoundException("존재하지 않는 orderNo입니다: " + orderNo);
    }
    return order.value();
}

std::vector<Model::Order> ReleaseService::GetConfirmedOrders() const {
    return orderRepository_.FindByStatus(Model::OrderStatus::CONFIRMED);
}

void ReleaseService::ReleaseOrder(const std::string& orderNo) {
    Model::Order order = FindOrderOrThrow(orderNo);
    order.Release();
    orderRepository_.Update(order);
}

}  // namespace Service
