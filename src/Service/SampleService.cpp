#include "SampleService.h"

namespace Service {

InvalidSampleException::InvalidSampleException(const std::string& message) : std::invalid_argument(message) {}

DuplicateSampleIdException::DuplicateSampleIdException(const std::string& message) : std::logic_error(message) {}

SampleService::SampleService(Repository::ISampleRepository& repository) : repository_(repository) {}

void SampleService::ValidateFields(const std::string& sampleId, const std::string& name,
                                    double averageProductionTimeMinutes, double yield, int initialStock) const {
    if (sampleId.empty()) {
        throw InvalidSampleException("sampleId는 빈 문자열일 수 없습니다.");
    }
    if (name.empty()) {
        throw InvalidSampleException("name은 빈 문자열일 수 없습니다.");
    }
    if (averageProductionTimeMinutes <= kMinAverageProductionTimeExclusive) {
        throw InvalidSampleException("averageProductionTimeMinutes는 0보다 커야 합니다.");
    }
    if (yield <= kMinYieldExclusive || yield > kMaxYieldInclusive) {
        throw InvalidSampleException("yield는 0보다 크고 1 이하여야 합니다.");
    }
    if (initialStock < kMinInitialStockInclusive) {
        throw InvalidSampleException("initialStock은 음수일 수 없습니다.");
    }
}

void SampleService::RegisterSample(const std::string& sampleId, const std::string& name,
                                    double averageProductionTimeMinutes, double yield, int initialStock) {
    ValidateFields(sampleId, name, averageProductionTimeMinutes, yield, initialStock);

    if (repository_.FindById(sampleId).has_value()) {
        throw DuplicateSampleIdException("이미 존재하는 sampleId입니다: " + sampleId);
    }

    Model::Sample sample(sampleId, name, averageProductionTimeMinutes, yield, initialStock);
    repository_.Save(sample);
}

std::vector<Model::Sample> SampleService::GetAllSamples() const {
    return repository_.FindAll();
}

std::vector<Model::Sample> SampleService::SearchByName(const std::string& keyword) const {
    return repository_.FindByName(keyword);
}

}  // namespace Service
