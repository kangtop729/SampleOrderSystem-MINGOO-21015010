#include <gtest/gtest.h>

#include "../src/Model/Sample.h"

using Model::Sample;

namespace {

Sample MakeSample(int initialStock = 10) {
    return Sample("SMP-001", "테스트시료", 30.0, 0.8, initialStock);
}

}  // namespace

TEST(SampleTest, 생성자로_설정한_값들이_getter로_정확히_조회된다) {
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 10);

    EXPECT_EQ(sample.GetSampleId(), "SMP-001");
    EXPECT_EQ(sample.GetName(), "테스트시료");
    EXPECT_DOUBLE_EQ(sample.GetAverageProductionTimeMinutes(), 30.0);
    EXPECT_DOUBLE_EQ(sample.GetYield(), 0.8);
    EXPECT_EQ(sample.GetStock(), 10);
}

TEST(SampleTest, IncreaseStock_호출시_재고가_정확히_증가한다) {
    Sample sample = MakeSample(10);

    sample.IncreaseStock(5);

    EXPECT_EQ(sample.GetStock(), 15);
}

TEST(SampleTest, IncreaseStock_0을_전달하면_재고가_그대로_유지된다) {
    Sample sample = MakeSample(10);

    sample.IncreaseStock(0);

    EXPECT_EQ(sample.GetStock(), 10);
}

TEST(SampleTest, IncreaseStock_음수를_전달하면_예외를_던진다) {
    Sample sample = MakeSample(10);

    EXPECT_THROW(sample.IncreaseStock(-1), std::exception);
}
