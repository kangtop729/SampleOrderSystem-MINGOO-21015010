#pragma once

#include <vector>

#include "../Model/Sample.h"
#include "IRepository.h"

namespace Repository {

// Sample 엔티티에 대한 Repository 인터페이스.
class ISampleRepository : public IRepository<Model::Sample> {
public:
    ~ISampleRepository() override = default;

    // 이름에 keyword가 포함된(부분 일치) 시료를 모두 반환한다.
    virtual std::vector<Model::Sample> FindByName(const std::string& keyword) const = 0;
};

}  // namespace Repository
