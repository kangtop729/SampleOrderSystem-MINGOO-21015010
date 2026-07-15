#include <gtest/gtest.h>

#include "../src/Model/Sample.h"
#include "../src/Serialization/SampleSerialization.h"
#include "../src/ThirdParty/nlohmann/json.hpp"

using Model::Sample;

namespace {

Sample MakeSample() {
    return Sample("SMP-001", "Wafer-A", 45.0, 0.92, 120);
}

}  // namespace

// ---- ToJson: 스키마 필드 확인 ----

TEST(SampleSerializationTest, ToJson_결과에_스키마에_정의된_필드가_모두_포함된다) {
    Sample sample = MakeSample();

    nlohmann::json json = SampleSerialization::ToJson(sample);

    EXPECT_EQ(json.at("sampleId").get<std::string>(), "SMP-001");
    EXPECT_EQ(json.at("name").get<std::string>(), "Wafer-A");
    EXPECT_DOUBLE_EQ(json.at("averageProductionTimeMinutes").get<double>(), 45.0);
    EXPECT_DOUBLE_EQ(json.at("yield").get<double>(), 0.92);
    EXPECT_EQ(json.at("stock").get<int>(), 120);
}

// ---- ToJson -> FromJson 라운드트립 ----

TEST(SampleSerializationTest, ToJson_FromJson_라운드트립시_모든_필드가_정확히_보존된다) {
    Sample sample = MakeSample();

    nlohmann::json json = SampleSerialization::ToJson(sample);
    Sample restored = SampleSerialization::FromJson(json);

    EXPECT_EQ(restored.GetSampleId(), sample.GetSampleId());
    EXPECT_EQ(restored.GetName(), sample.GetName());
    EXPECT_DOUBLE_EQ(restored.GetAverageProductionTimeMinutes(), sample.GetAverageProductionTimeMinutes());
    EXPECT_DOUBLE_EQ(restored.GetYield(), sample.GetYield());
    EXPECT_EQ(restored.GetStock(), sample.GetStock());
}

TEST(SampleSerializationTest, ToJson_FromJson_라운드트립시_재고가_0인_시료도_정확히_보존된다) {
    Sample sample("SMP-002", "Wafer-B", 30.0, 1.0, 0);

    nlohmann::json json = SampleSerialization::ToJson(sample);
    Sample restored = SampleSerialization::FromJson(json);

    EXPECT_EQ(restored.GetStock(), 0);
    EXPECT_DOUBLE_EQ(restored.GetYield(), 1.0);
}
