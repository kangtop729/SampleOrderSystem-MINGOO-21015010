#include "JsonSampleRepository.h"

#include <algorithm>
#include <fstream>
#include <sstream>

#include "../Serialization/SampleSerialization.h"
#include "RepositoryException.h"

using Model::Sample;

namespace Repository {

JsonSampleRepository::JsonSampleRepository(std::string filePath) : filePath_(std::move(filePath)) {}

std::vector<Sample> JsonSampleRepository::Load() const {
    std::ifstream in(filePath_);
    if (!in.is_open()) {
        return {};
    }

    nlohmann::json root;
    try {
        in >> root;
    } catch (const nlohmann::json::parse_error& e) {
        throw RepositoryException(std::string("JsonSampleRepository: JSON 파싱에 실패했습니다: ") + e.what());
    }

    std::vector<Sample> samples;
    for (const nlohmann::json& item : root) {
        samples.push_back(SampleSerialization::FromJson(item));
    }
    return samples;
}

void JsonSampleRepository::Flush(const std::vector<Sample>& samples) const {
    nlohmann::json root = nlohmann::json::array();
    for (const Sample& sample : samples) {
        root.push_back(SampleSerialization::ToJson(sample));
    }

    std::ofstream out(filePath_, std::ios::trunc);
    out << root.dump(2);
}

void JsonSampleRepository::Save(const Sample& sample) {
    std::vector<Sample> samples = Load();
    samples.push_back(sample);
    Flush(samples);
}

std::vector<Sample> JsonSampleRepository::FindAll() const {
    return Load();
}

std::optional<Sample> JsonSampleRepository::FindById(const std::string& sampleId) const {
    std::vector<Sample> samples = Load();
    auto it = std::find_if(samples.begin(), samples.end(),
                            [&](const Sample& s) { return s.GetSampleId() == sampleId; });
    if (it == samples.end()) {
        return std::nullopt;
    }
    return *it;
}

bool JsonSampleRepository::Update(const Sample& sample) {
    std::vector<Sample> samples = Load();
    auto it = std::find_if(samples.begin(), samples.end(),
                            [&](const Sample& s) { return s.GetSampleId() == sample.GetSampleId(); });
    if (it == samples.end()) {
        return false;
    }
    *it = sample;
    Flush(samples);
    return true;
}

bool JsonSampleRepository::Remove(const std::string& sampleId) {
    std::vector<Sample> samples = Load();
    auto it = std::find_if(samples.begin(), samples.end(),
                            [&](const Sample& s) { return s.GetSampleId() == sampleId; });
    if (it == samples.end()) {
        return false;
    }
    samples.erase(it);
    Flush(samples);
    return true;
}

std::vector<Sample> JsonSampleRepository::FindByName(const std::string& keyword) const {
    std::vector<Sample> samples = Load();
    std::vector<Sample> matched;
    std::copy_if(samples.begin(), samples.end(), std::back_inserter(matched),
                 [&](const Sample& s) { return s.GetName().find(keyword) != std::string::npos; });
    return matched;
}

}  // namespace Repository
