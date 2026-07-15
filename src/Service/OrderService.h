#pragma once

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include "../Model/Order.h"
#include "../Model/OrderStatus.h"
#include "../Repository/IOrderRepository.h"
#include "../Repository/ISampleRepository.h"

namespace Service {

// 주문 입력값(수량, 고객명 등)이 유효하지 않을 때 발생하는 예외.
class InvalidOrderException : public std::invalid_argument {
public:
    explicit InvalidOrderException(const std::string& message);
};

// 존재하지 않는 sampleId로 주문을 접수하려 할 때 발생하는 예외.
class SampleNotFoundException : public std::logic_error {
public:
    explicit SampleNotFoundException(const std::string& message);
};

// 존재하지 않는 orderNo로 주문을 조회/조작하려 할 때 발생하는 예외.
class OrderNotFoundException : public std::logic_error {
public:
    explicit OrderNotFoundException(const std::string& message);
};

// 시료 주문(Order) 접수/승인/거절/조회에 관한 도메인 로직을 담당한다.
class OrderService {
public:
    static constexpr int kMinQuantityExclusive = 0;

    // 새 주문의 orderNo를 생성하는 함수 타입.
    using OrderNumberGenerator = std::function<std::string()>;

    explicit OrderService(Repository::IOrderRepository& orderRepository, Repository::ISampleRepository& sampleRepository,
                           OrderNumberGenerator orderNumberGenerator = DefaultOrderNumberGenerator());

    // sampleId, customerName, quantity를 검증한 뒤 RESERVED 상태의 Order를 생성하여 저장한다.
    // sampleId가 존재하지 않으면 SampleNotFoundException, quantity<=0 또는 customerName이 빈 문자열이면
    // InvalidOrderException을 던진다.
    Model::Order PlaceOrder(const std::string& sampleId, const std::string& customerName, int quantity);

    // orderNo의 주문을 승인한다. 재고가 주문 수량 이상이면 CONFIRMED로, 부족하면 PRODUCING으로 전환한다.
    // orderNo가 존재하지 않으면 OrderNotFoundException을 던진다. 전이 후의 OrderStatus를 반환한다.
    Model::OrderStatus ApproveOrder(const std::string& orderNo);

    // orderNo의 주문을 거절(REJECTED)한다. orderNo가 존재하지 않으면 OrderNotFoundException을 던진다.
    void RejectOrder(const std::string& orderNo);

    std::vector<Model::Order> GetReservedOrders() const;

private:
    static OrderNumberGenerator DefaultOrderNumberGenerator();

    void ValidatePlaceOrderFields(const std::string& customerName, int quantity) const;
    Model::Order FindOrderOrThrow(const std::string& orderNo) const;

    Repository::IOrderRepository& orderRepository_;
    Repository::ISampleRepository& sampleRepository_;
    OrderNumberGenerator orderNumberGenerator_;
};

}  // namespace Service
