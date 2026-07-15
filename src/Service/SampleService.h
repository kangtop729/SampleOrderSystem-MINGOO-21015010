#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "../Model/Sample.h"
#include "../Repository/ISampleRepository.h"

namespace Service {

// 시료 등록 입력값이 유효하지 않을 때 발생하는 예외.
class InvalidSampleException : public std::invalid_argument {
public:
    explicit InvalidSampleException(const std::string& message);
};

// 이미 존재하는 sampleId로 시료를 등록하려 할 때 발생하는 예외.
class DuplicateSampleIdException : public std::logic_error {
public:
    explicit DuplicateSampleIdException(const std::string& message);
};

// 시료(Sample) 등록/조회에 관한 도메인 로직을 담당한다.
class SampleService {
public:
    static constexpr double kMinYieldExclusive = 0.0;
    static constexpr double kMaxYieldInclusive = 1.0;
    static constexpr double kMinAverageProductionTimeExclusive = 0.0;
    static constexpr int kMinInitialStockInclusive = 0;

    explicit SampleService(Repository::ISampleRepository& repository);

    // 입력값을 검증한 뒤, 중복되지 않은 sampleId이면 시료를 등록한다.
    // 검증 실패 시 InvalidSampleException, sampleId 중복 시 DuplicateSampleIdException을 던진다.
    void RegisterSample(const std::string& sampleId, const std::string& name,
                         double averageProductionTimeMinutes, double yield, int initialStock);

    std::vector<Model::Sample> GetAllSamples() const;
    std::vector<Model::Sample> SearchByName(const std::string& keyword) const;

private:
    void ValidateFields(const std::string& sampleId, const std::string& name,
                         double averageProductionTimeMinutes, double yield, int initialStock) const;

    Repository::ISampleRepository& repository_;
};

}  // namespace Service
