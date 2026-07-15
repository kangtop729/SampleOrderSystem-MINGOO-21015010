#include "SampleSerialization.h"

using Model::Sample;

nlohmann::json SampleSerialization::ToJson(const Sample& sample) {
    return nlohmann::json{{"sampleId", sample.GetSampleId()},
                           {"name", sample.GetName()},
                           {"averageProductionTimeMinutes", sample.GetAverageProductionTimeMinutes()},
                           {"yield", sample.GetYield()},
                           {"stock", sample.GetStock()}};
}

Sample SampleSerialization::FromJson(const nlohmann::json& json) {
    return Sample(json.at("sampleId").get<std::string>(), json.at("name").get<std::string>(),
                  json.at("averageProductionTimeMinutes").get<double>(), json.at("yield").get<double>(),
                  json.at("stock").get<int>());
}
