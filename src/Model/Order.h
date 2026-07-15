#pragma once

#include <chrono>
#include <stdexcept>
#include <string>

#include "OrderStatus.h"

namespace Model {

// 정의되지 않은 주문 상태 전이를 시도할 때 던지는 예외.
class InvalidOrderTransitionException : public std::logic_error {
public:
    explicit InvalidOrderTransitionException(const std::string& message);
};

// 시료 주문(Order) 엔티티.
// 상태 전이는 허용 전이표(TransitionTo)에 의해서만 이루어진다.
//
// 허용 전이표:
//   RESERVED  -> REJECTED, CONFIRMED, PRODUCING
//   PRODUCING -> CONFIRMED
//   CONFIRMED -> RELEASE
// REJECTED, RELEASE는 종단 상태로 그 이후 전이는 모두 불가능하다.
class Order {
public:
    Order(std::string orderNo, std::string sampleId, std::string customerName, int quantity);

    const std::string& GetOrderNo() const;
    const std::string& GetSampleId() const;
    const std::string& GetCustomerName() const;
    int GetQuantity() const;
    OrderStatus GetStatus() const;
    std::chrono::system_clock::time_point GetCreatedAt() const;

    void Reject();
    void Confirm();
    void StartProducing();
    void CompleteProduction();
    void Release();

private:
    // 현재 상태가 requiredCurrentStatus와 일치할 때만 nextStatus로 전이한다.
    // 그렇지 않으면 InvalidOrderTransitionException을 던진다.
    void TransitionTo(OrderStatus requiredCurrentStatus, OrderStatus nextStatus);

    std::string orderNo_;
    std::string sampleId_;
    std::string customerName_;
    int quantity_;
    OrderStatus status_;
    std::chrono::system_clock::time_point createdAt_;
};

}  // namespace Model
