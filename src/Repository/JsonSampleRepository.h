#pragma once

#include <string>
#include <vector>

#include "../Model/Sample.h"
#include "ISampleRepository.h"

namespace Repository {

// samples.json 파일을 저장소로 사용하는 ISampleRepository 구현체.
// 파일이 없으면 빈 컬렉션으로 취급하고, CRUD 연산 후 즉시 파일에 반영(flush)한다.
class JsonSampleRepository : public ISampleRepository {
public:
    explicit JsonSampleRepository(std::string filePath);

    void Save(const Model::Sample& sample) override;
    std::vector<Model::Sample> FindAll() const override;
    std::optional<Model::Sample> FindById(const std::string& sampleId) const override;
    bool Update(const Model::Sample& sample) override;
    bool Remove(const std::string& sampleId) override;
    std::vector<Model::Sample> FindByName(const std::string& keyword) const override;

private:
    std::vector<Model::Sample> Load() const;
    void Flush(const std::vector<Model::Sample>& samples) const;

    std::string filePath_;
};

}  // namespace Repository
