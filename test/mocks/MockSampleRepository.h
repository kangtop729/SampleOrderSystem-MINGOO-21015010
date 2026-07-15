#pragma once

#include <gmock/gmock.h>

#include <optional>
#include <string>
#include <vector>

#include "../../src/Model/Sample.h"
#include "../../src/Repository/ISampleRepository.h"

namespace Test {

// ISampleRepository에 대한 gmock 기반 테스트 더블.
// SampleService 등 Repository에 의존하는 상위 계층의 단위 테스트에서 사용한다.
class MockSampleRepository : public Repository::ISampleRepository {
public:
    MOCK_METHOD(void, Save, (const Model::Sample& entity), (override));
    MOCK_METHOD(std::vector<Model::Sample>, FindAll, (), (const, override));
    MOCK_METHOD(std::optional<Model::Sample>, FindById, (const std::string& id), (const, override));
    MOCK_METHOD(bool, Update, (const Model::Sample& entity), (override));
    MOCK_METHOD(bool, Remove, (const std::string& id), (override));
    MOCK_METHOD(std::vector<Model::Sample>, FindByName, (const std::string& keyword), (const, override));
};

}  // namespace Test
