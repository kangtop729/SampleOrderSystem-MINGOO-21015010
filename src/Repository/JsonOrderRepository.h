#pragma once

#include <string>
#include <vector>

#include "../Model/Order.h"
#include "../Model/OrderStatus.h"
#include "IOrderRepository.h"

namespace Repository {

// orders.json 파일을 저장소로 사용하는 IOrderRepository 구현체.
// 파일이 없으면 빈 컬렉션으로 취급하고, CRUD 연산 후 즉시 파일에 반영(flush)한다.
class JsonOrderRepository : public IOrderRepository {
public:
    explicit JsonOrderRepository(std::string filePath);

    void Save(const Model::Order& order) override;
    std::vector<Model::Order> FindAll() const override;
    std::optional<Model::Order> FindById(const std::string& orderNo) const override;
    bool Update(const Model::Order& order) override;
    bool Remove(const std::string& orderNo) override;
    std::vector<Model::Order> FindBySampleId(const std::string& sampleId) const override;
    std::vector<Model::Order> FindByStatus(Model::OrderStatus status) const override;

private:
    std::vector<Model::Order> Load() const;
    void Flush(const std::vector<Model::Order>& orders) const;

    std::string filePath_;
};

}  // namespace Repository
