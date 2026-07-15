#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>
#include <vector>

#include "../src/Model/Sample.h"
#include "../src/Service/SampleService.h"
#include "mocks/MockSampleRepository.h"

using Model::Sample;
using Service::DuplicateSampleIdException;
using Service::InvalidSampleException;
using Service::SampleService;
using Test::MockSampleRepository;
using ::testing::_;
using ::testing::AllOf;
using ::testing::Eq;
using ::testing::Property;
using ::testing::Return;

namespace {

// Save로 전달되는 Sample의 각 필드가 기대값과 일치하는지 검증하는 매처.
auto SampleFieldsAre(const std::string& sampleId, const std::string& name, double averageProductionTimeMinutes,
                      double yield, int stock) {
    return AllOf(Property(&Sample::GetSampleId, Eq(sampleId)), Property(&Sample::GetName, Eq(name)),
                 Property(&Sample::GetAverageProductionTimeMinutes, Eq(averageProductionTimeMinutes)),
                 Property(&Sample::GetYield, Eq(yield)), Property(&Sample::GetStock, Eq(stock)));
}

}  // namespace

class SampleServiceTest : public ::testing::Test {
protected:
    // 필드 검증 실패로 등록이 거부될 때 repository가 전혀 호출되지 않아야 함을 기대한다.
    void ExpectRepositoryNotCalled() {
        EXPECT_CALL(repository_, FindById(_)).Times(0);
        EXPECT_CALL(repository_, Save(_)).Times(0);
    }

    MockSampleRepository repository_;
};

TEST_F(SampleServiceTest, RegisterSample_유효한값이면_Save가_정확한_Sample로_한번_호출된다) {
    EXPECT_CALL(repository_, FindById(std::string("SMP-001"))).WillOnce(Return(std::nullopt));
    EXPECT_CALL(repository_, Save(SampleFieldsAre("SMP-001", "테스트시료", 30.0, 0.8, 10))).Times(1);

    SampleService service(repository_);

    service.RegisterSample("SMP-001", "테스트시료", 30.0, 0.8, 10);
}

TEST_F(SampleServiceTest, RegisterSample_중복된_sampleId면_DuplicateSampleIdException을_던지고_Save는_호출되지_않는다) {
    Sample existing("SMP-001", "기존시료", 30.0, 0.8, 5);
    EXPECT_CALL(repository_, FindById(std::string("SMP-001"))).WillOnce(Return(existing));
    EXPECT_CALL(repository_, Save(_)).Times(0);

    SampleService service(repository_);

    EXPECT_THROW(service.RegisterSample("SMP-001", "테스트시료", 30.0, 0.8, 10), DuplicateSampleIdException);
}

TEST_F(SampleServiceTest, RegisterSample_sampleId가_빈문자열이면_InvalidSampleException을_던지고_repository는_호출되지_않는다) {
    ExpectRepositoryNotCalled();

    SampleService service(repository_);

    EXPECT_THROW(service.RegisterSample("", "테스트시료", 30.0, 0.8, 10), InvalidSampleException);
}

TEST_F(SampleServiceTest, RegisterSample_name이_빈문자열이면_InvalidSampleException을_던지고_repository는_호출되지_않는다) {
    ExpectRepositoryNotCalled();

    SampleService service(repository_);

    EXPECT_THROW(service.RegisterSample("SMP-001", "", 30.0, 0.8, 10), InvalidSampleException);
}

TEST_F(SampleServiceTest, RegisterSample_평균생산시간이_0이면_InvalidSampleException을_던지고_repository는_호출되지_않는다) {
    ExpectRepositoryNotCalled();

    SampleService service(repository_);

    EXPECT_THROW(service.RegisterSample("SMP-001", "테스트시료", 0.0, 0.8, 10), InvalidSampleException);
}

TEST_F(SampleServiceTest, RegisterSample_평균생산시간이_음수면_InvalidSampleException을_던지고_repository는_호출되지_않는다) {
    ExpectRepositoryNotCalled();

    SampleService service(repository_);

    EXPECT_THROW(service.RegisterSample("SMP-001", "테스트시료", -1.0, 0.8, 10), InvalidSampleException);
}

TEST_F(SampleServiceTest, RegisterSample_수율이_0이면_InvalidSampleException을_던지고_repository는_호출되지_않는다) {
    ExpectRepositoryNotCalled();

    SampleService service(repository_);

    EXPECT_THROW(service.RegisterSample("SMP-001", "테스트시료", 30.0, 0.0, 10), InvalidSampleException);
}

TEST_F(SampleServiceTest, RegisterSample_수율이_1이면_경계값으로_허용되어_정상등록된다) {
    EXPECT_CALL(repository_, FindById(std::string("SMP-001"))).WillOnce(Return(std::nullopt));
    EXPECT_CALL(repository_, Save(SampleFieldsAre("SMP-001", "테스트시료", 30.0, 1.0, 10))).Times(1);

    SampleService service(repository_);

    EXPECT_NO_THROW(service.RegisterSample("SMP-001", "테스트시료", 30.0, 1.0, 10));
}

TEST_F(SampleServiceTest, RegisterSample_수율이_1보다_크면_InvalidSampleException을_던지고_repository는_호출되지_않는다) {
    ExpectRepositoryNotCalled();

    SampleService service(repository_);

    EXPECT_THROW(service.RegisterSample("SMP-001", "테스트시료", 30.0, 1.1, 10), InvalidSampleException);
}

TEST_F(SampleServiceTest, RegisterSample_초기재고가_음수면_InvalidSampleException을_던지고_repository는_호출되지_않는다) {
    ExpectRepositoryNotCalled();

    SampleService service(repository_);

    EXPECT_THROW(service.RegisterSample("SMP-001", "테스트시료", 30.0, 0.8, -1), InvalidSampleException);
}

TEST_F(SampleServiceTest, RegisterSample_초기재고가_0이면_경계값으로_허용되어_정상등록된다) {
    EXPECT_CALL(repository_, FindById(std::string("SMP-001"))).WillOnce(Return(std::nullopt));
    EXPECT_CALL(repository_, Save(SampleFieldsAre("SMP-001", "테스트시료", 30.0, 0.8, 0))).Times(1);

    SampleService service(repository_);

    EXPECT_NO_THROW(service.RegisterSample("SMP-001", "테스트시료", 30.0, 0.8, 0));
}

TEST_F(SampleServiceTest, GetAllSamples_repository의_FindAll_결과를_그대로_반환한다) {
    std::vector<Sample> expected{Sample("SMP-001", "A", 30.0, 0.8, 10), Sample("SMP-002", "B", 40.0, 0.9, 20)};
    EXPECT_CALL(repository_, FindAll()).WillOnce(Return(expected));

    SampleService service(repository_);

    std::vector<Sample> actual = service.GetAllSamples();

    ASSERT_EQ(actual.size(), 2u);
    EXPECT_EQ(actual[0].GetSampleId(), "SMP-001");
    EXPECT_EQ(actual[1].GetSampleId(), "SMP-002");
}

TEST_F(SampleServiceTest, SearchByName_repository의_FindByName에_keyword를_정확히_위임한다) {
    std::vector<Sample> expected{Sample("SMP-001", "웨이퍼A", 30.0, 0.8, 10)};
    EXPECT_CALL(repository_, FindByName(std::string("웨이퍼"))).WillOnce(Return(expected));

    SampleService service(repository_);

    std::vector<Sample> actual = service.SearchByName("웨이퍼");

    ASSERT_EQ(actual.size(), 1u);
    EXPECT_EQ(actual[0].GetName(), "웨이퍼A");
}

TEST_F(SampleServiceTest, FindOne_repository의_FindById_결과를_그대로_반환한다) {
    Sample expected("SMP-001", "테스트시료", 30.0, 0.8, 10);
    EXPECT_CALL(repository_, FindById(std::string("SMP-001"))).WillOnce(Return(expected));

    SampleService service(repository_);

    std::optional<Sample> actual = service.FindOne("SMP-001");

    ASSERT_TRUE(actual.has_value());
    EXPECT_EQ(actual->GetSampleId(), "SMP-001");
    EXPECT_EQ(actual->GetName(), "테스트시료");
    EXPECT_EQ(actual->GetAverageProductionTimeMinutes(), 30.0);
    EXPECT_EQ(actual->GetYield(), 0.8);
    EXPECT_EQ(actual->GetStock(), 10);
}

TEST_F(SampleServiceTest, FindOne_존재하지않는_sampleId면_nullopt를_반환한다) {
    EXPECT_CALL(repository_, FindById(std::string("SMP-999"))).WillOnce(Return(std::nullopt));

    SampleService service(repository_);

    std::optional<Sample> actual = service.FindOne("SMP-999");

    EXPECT_FALSE(actual.has_value());
}
