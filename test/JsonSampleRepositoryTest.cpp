#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <functional>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

#include "../src/Model/Sample.h"
#include "../src/Repository/JsonSampleRepository.h"
#include "../src/Repository/RepositoryException.h"

using Model::Sample;
using Repository::JsonSampleRepository;
using Repository::RepositoryException;

namespace {

void WriteRawFile(const std::filesystem::path& path, const std::string& content) {
    std::ofstream out(path, std::ios::trunc);
    out << content;
}

Sample MakeSample(const std::string& sampleId = "SMP-001", const std::string& name = "Wafer-A", int stock = 120) {
    return Sample(sampleId, name, 45.0, 0.92, stock);
}

}  // namespace

class JsonSampleRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        const std::string rawTestName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
        // 테스트 이름에 한글이 포함되어 있으면 std::filesystem::path 생성 시 Windows ANSI 코드
        // 페이지 변환에서 예외가 발생할 수 있어(특히 /utf-8 컴파일 옵션 적용 후), ASCII 안전한
        // 해시값으로 대체해 파일명을 만든다.
        const std::string testName = std::to_string(std::hash<std::string>{}(rawTestName));
        filePath_ = std::filesystem::temp_directory_path() / ("sample_repo_test_" + testName + ".json");
        std::filesystem::remove(filePath_);
    }

    void TearDown() override {
        std::filesystem::remove(filePath_);
    }

    std::filesystem::path filePath_;
};

// ---- 파일 부재 ----

TEST_F(JsonSampleRepositoryTest, 파일이_없으면_FindAll은_빈_벡터를_반환한다) {
    JsonSampleRepository repository(filePath_.string());

    EXPECT_TRUE(repository.FindAll().empty());
}

// ---- 저장/조회 ----

TEST_F(JsonSampleRepositoryTest, Save_후_FindAll에서_저장한_시료를_조회할_수_있다) {
    JsonSampleRepository repository(filePath_.string());

    repository.Save(MakeSample());

    std::vector<Sample> all = repository.FindAll();
    ASSERT_EQ(all.size(), 1u);
    EXPECT_EQ(all[0].GetSampleId(), "SMP-001");
    EXPECT_EQ(all[0].GetName(), "Wafer-A");
    EXPECT_EQ(all[0].GetStock(), 120);
}

TEST_F(JsonSampleRepositoryTest, Save_후_FindById로_저장한_시료를_조회할_수_있다) {
    JsonSampleRepository repository(filePath_.string());

    repository.Save(MakeSample());

    std::optional<Sample> found = repository.FindById("SMP-001");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->GetSampleId(), "SMP-001");
}

TEST_F(JsonSampleRepositoryTest, 존재하지_않는_id로_FindById_호출시_nullopt를_반환한다) {
    JsonSampleRepository repository(filePath_.string());

    repository.Save(MakeSample());

    EXPECT_FALSE(repository.FindById("SMP-999").has_value());
}

// ---- 재로드 시 데이터 일치 ----

TEST_F(JsonSampleRepositoryTest, 저장후_새_Repository_인스턴스로_재로드해도_데이터가_일치한다) {
    {
        JsonSampleRepository repository(filePath_.string());
        repository.Save(MakeSample("SMP-001", "Wafer-A", 120));
        repository.Save(MakeSample("SMP-002", "Wafer-B", 30));
    }

    JsonSampleRepository reloaded(filePath_.string());
    std::vector<Sample> all = reloaded.FindAll();

    ASSERT_EQ(all.size(), 2u);
    std::optional<Sample> first = reloaded.FindById("SMP-001");
    std::optional<Sample> second = reloaded.FindById("SMP-002");
    ASSERT_TRUE(first.has_value());
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(first->GetStock(), 120);
    EXPECT_EQ(second->GetStock(), 30);
}

// ---- Update ----

TEST_F(JsonSampleRepositoryTest, Update_존재하는_id면_true를_반환하고_필드가_반영된다) {
    JsonSampleRepository repository(filePath_.string());
    repository.Save(MakeSample("SMP-001", "Wafer-A", 120));

    bool updated = repository.Update(Sample("SMP-001", "Wafer-A", 45.0, 0.92, 200));

    EXPECT_TRUE(updated);
    std::optional<Sample> found = repository.FindById("SMP-001");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->GetStock(), 200);
}

TEST_F(JsonSampleRepositoryTest, Update_존재하지_않는_id면_false를_반환한다) {
    JsonSampleRepository repository(filePath_.string());
    repository.Save(MakeSample("SMP-001", "Wafer-A", 120));

    bool updated = repository.Update(Sample("SMP-999", "Unknown", 10.0, 1.0, 1));

    EXPECT_FALSE(updated);
}

// ---- Remove ----

TEST_F(JsonSampleRepositoryTest, Remove_존재하는_id면_true를_반환하고_이후_조회되지_않는다) {
    JsonSampleRepository repository(filePath_.string());
    repository.Save(MakeSample("SMP-001", "Wafer-A", 120));

    bool removed = repository.Remove("SMP-001");

    EXPECT_TRUE(removed);
    EXPECT_FALSE(repository.FindById("SMP-001").has_value());
}

TEST_F(JsonSampleRepositoryTest, Remove_존재하지_않는_id면_false를_반환한다) {
    JsonSampleRepository repository(filePath_.string());
    repository.Save(MakeSample("SMP-001", "Wafer-A", 120));

    EXPECT_FALSE(repository.Remove("SMP-999"));
}

// ---- FindByName 부분 일치 검색 ----

TEST_F(JsonSampleRepositoryTest, FindByName은_이름에_키워드가_포함된_시료만_반환한다) {
    JsonSampleRepository repository(filePath_.string());
    repository.Save(MakeSample("SMP-001", "Wafer-A", 120));
    repository.Save(MakeSample("SMP-002", "Wafer-B", 30));
    repository.Save(MakeSample("SMP-003", "Chip-C", 10));

    std::vector<Sample> found = repository.FindByName("Wafer");

    ASSERT_EQ(found.size(), 2u);
    EXPECT_NE(std::find_if(found.begin(), found.end(),
                           [](const Sample& s) { return s.GetSampleId() == "SMP-001"; }),
              found.end());
    EXPECT_NE(std::find_if(found.begin(), found.end(),
                           [](const Sample& s) { return s.GetSampleId() == "SMP-002"; }),
              found.end());
}

TEST_F(JsonSampleRepositoryTest, FindByName은_일치하는_시료가_없으면_빈_벡터를_반환한다) {
    JsonSampleRepository repository(filePath_.string());
    repository.Save(MakeSample("SMP-001", "Wafer-A", 120));

    EXPECT_TRUE(repository.FindByName("NoSuchKeyword").empty());
}

// ---- 손상된 파일 ----

TEST_F(JsonSampleRepositoryTest, 손상된_JSON_파일을_읽으면_RepositoryException을_던진다) {
    WriteRawFile(filePath_, "{ this is not valid json !!!");

    EXPECT_THROW(
        {
            JsonSampleRepository repository(filePath_.string());
            repository.FindAll();
        },
        RepositoryException);
}
