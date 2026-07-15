#include <gtest/gtest.h>

#include "../src/Service/ProductionLineService.h"

using Service::ProductionLineService;

// ------------------------- CalculateShortfall -------------------------

TEST(ProductionCalculationTest, CalculateShortfall_재고가_주문수량과_정확히_같으면_0을_반환한다) {
    EXPECT_EQ(ProductionLineService::CalculateShortfall(10, 10), 0);
}

TEST(ProductionCalculationTest, CalculateShortfall_재고가_주문수량보다_1개_부족하면_1을_반환한다) {
    EXPECT_EQ(ProductionLineService::CalculateShortfall(10, 9), 1);
}

TEST(ProductionCalculationTest, CalculateShortfall_재고가_0이면_주문수량_그대로_반환한다) {
    EXPECT_EQ(ProductionLineService::CalculateShortfall(10, 0), 10);
}

TEST(ProductionCalculationTest, CalculateShortfall_재고가_주문수량보다_많으면_0을_반환한다_음수방지) {
    EXPECT_EQ(ProductionLineService::CalculateShortfall(10, 100), 0);
}

// ------------------------- CalculateActualProductionQty -------------------------

TEST(ProductionCalculationTest, CalculateActualProductionQty_부족분이_0이면_0을_반환한다) {
    EXPECT_EQ(ProductionLineService::CalculateActualProductionQty(0, 0.5), 0);
}

TEST(ProductionCalculationTest, CalculateActualProductionQty_부족분이_음수이면_0을_반환한다) {
    EXPECT_EQ(ProductionLineService::CalculateActualProductionQty(-5, 0.5), 0);
}

TEST(ProductionCalculationTest, CalculateActualProductionQty_10을_수율0_5로_나누면_부동소수점오차없이_정확히_20이다) {
    // 10.0 / 0.5 = 20.0 이지만 부동소수점 오차로 20.000000000000004가 되어
    // ceil이 21을 반환하는 버그가 흔하다. epsilon 보정이 반드시 적용되어야 한다.
    EXPECT_EQ(ProductionLineService::CalculateActualProductionQty(10, 0.5), 20);
}

TEST(ProductionCalculationTest, CalculateActualProductionQty_수율이_1_0이면_부족분과_동일하다) {
    EXPECT_EQ(ProductionLineService::CalculateActualProductionQty(10, 1.0), 10);
}

TEST(ProductionCalculationTest, CalculateActualProductionQty_나누어떨어지지않으면_올림한다) {
    // 1 / 0.92 = 1.08695... -> ceil 2
    EXPECT_EQ(ProductionLineService::CalculateActualProductionQty(1, 0.92), 2);
}

TEST(ProductionCalculationTest, CalculateActualProductionQty_170을_수율0_92로_나누면_185이다) {
    // 170 / 0.92 = 184.7826... -> ceil 185
    EXPECT_EQ(ProductionLineService::CalculateActualProductionQty(170, 0.92), 185);
}

// ------------------------- CalculateTotalProductionMinutes -------------------------

TEST(ProductionCalculationTest, CalculateTotalProductionMinutes_평균생산시간과_실생산량을_곱한값을_반환한다) {
    EXPECT_DOUBLE_EQ(ProductionLineService::CalculateTotalProductionMinutes(0.8, 54), 43.2);
}

TEST(ProductionCalculationTest, CalculateTotalProductionMinutes_실생산량이_0이면_0을_반환한다) {
    EXPECT_DOUBLE_EQ(ProductionLineService::CalculateTotalProductionMinutes(0.8, 0), 0.0);
}
