#pragma once

#include <vector>

#include "../Model/Order.h"
#include "../Model/OrderStatus.h"
#include "IRepository.h"

namespace Repository {

// Order 엔티티에 대한 Repository 인터페이스.
class IOrderRepository : public IRepository<Model::Order> {
public:
    ~IOrderRepository() override = default;

    // 특정 시료(sampleId)에 대한 주문을 모두 반환한다.
    virtual std::vector<Model::Order> FindBySampleId(const std::string& sampleId) const = 0;

    // 특정 상태의 주문을 모두 반환한다.
    virtual std::vector<Model::Order> FindByStatus(Model::OrderStatus status) const = 0;
};

}  // namespace Repository
