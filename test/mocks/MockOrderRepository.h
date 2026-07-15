#pragma once

#include <gmock/gmock.h>

#include <optional>
#include <string>
#include <vector>

#include "../../src/Model/Order.h"
#include "../../src/Model/OrderStatus.h"
#include "../../src/Repository/IOrderRepository.h"

namespace Test {

// IOrderRepository에 대한 gmock 기반 테스트 더블.
// OrderService 등 Repository에 의존하는 상위 계층의 단위 테스트에서 사용한다.
class MockOrderRepository : public Repository::IOrderRepository {
public:
    MOCK_METHOD(void, Save, (const Model::Order& entity), (override));
    MOCK_METHOD(std::vector<Model::Order>, FindAll, (), (const, override));
    MOCK_METHOD(std::optional<Model::Order>, FindById, (const std::string& id), (const, override));
    MOCK_METHOD(bool, Update, (const Model::Order& entity), (override));
    MOCK_METHOD(bool, Remove, (const std::string& id), (override));
    MOCK_METHOD(std::vector<Model::Order>, FindBySampleId, (const std::string& sampleId), (const, override));
    MOCK_METHOD(std::vector<Model::Order>, FindByStatus, (Model::OrderStatus status), (const, override));
};

}  // namespace Test
