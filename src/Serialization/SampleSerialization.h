#pragma once

#include "../Model/Sample.h"
#include "../ThirdParty/nlohmann/json.hpp"

// Sample 엔티티 <-> JSON 변환을 담당한다.
// JSON 스키마: {"sampleId":"...","name":"...","averageProductionTimeMinutes":0.0,"yield":0.0,"stock":0}
class SampleSerialization {
public:
    static nlohmann::json ToJson(const Model::Sample& sample);
    static Model::Sample FromJson(const nlohmann::json& json);
};
