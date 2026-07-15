#pragma once

#include <string>
#include <vector>

#include "../Model/Order.h"
#include "../Repository/IOrderRepository.h"
#include "OrderService.h"

namespace Service {

// 시료 주문(Order)의 출고(CONFIRMED -> RELEASE)에 관한 도메인 로직을 담당한다.
class ReleaseService {
public:
    explicit ReleaseService(Repository::IOrderRepository& orderRepository);

    // CONFIRMED 상태의 주문 목록을 조회한다.
    std::vector<Model::Order> GetConfirmedOrders() const;

    // orderNo의 주문을 출고(RELEASE)한다. orderNo가 존재하지 않으면 OrderNotFoundException을,
    // 주문이 CONFIRMED 상태가 아니면 Model::InvalidOrderTransitionException을 던진다.
    void ReleaseOrder(const std::string& orderNo);

private:
    Model::Order FindOrderOrThrow(const std::string& orderNo) const;

    Repository::IOrderRepository& orderRepository_;
};

}  // namespace Service
