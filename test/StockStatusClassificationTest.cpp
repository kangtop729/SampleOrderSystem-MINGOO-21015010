#include <gtest/gtest.h>

#include "../src/Service/MonitoringService.h"

using Service::MonitoringService;
using Service::StockStatus;

// ------------------------- ClassifyStockStatus 경계값 -------------------------

TEST(StockStatusClassificationTest, 재고가_0이고_미해결수요도_0이면_DEPLETED이다) {
    EXPECT_EQ(MonitoringService::ClassifyStockStatus(0, 0), StockStatus::DEPLETED);
}

TEST(StockStatusClassificationTest, 재고가_0이고_미해결수요가_있으면_DEPLETED이다) {
    EXPECT_EQ(MonitoringService::ClassifyStockStatus(0, 10), StockStatus::DEPLETED);
}

TEST(StockStatusClassificationTest, 재고와_미해결수요가_정확히_같으면_SUFFICIENT이다) {
    EXPECT_EQ(MonitoringService::ClassifyStockStatus(5, 5), StockStatus::SUFFICIENT);
}

TEST(StockStatusClassificationTest, 재고가_미해결수요보다_1개_부족하면_SHORTAGE이다) {
    EXPECT_EQ(MonitoringService::ClassifyStockStatus(5, 6), StockStatus::SHORTAGE);
}

TEST(StockStatusClassificationTest, 재고가_미해결수요보다_크게_부족하면_SHORTAGE이다) {
    EXPECT_EQ(MonitoringService::ClassifyStockStatus(1, 100), StockStatus::SHORTAGE);
}

TEST(StockStatusClassificationTest, 미해결수요가_없으면_재고가_있는한_SUFFICIENT이다) {
    EXPECT_EQ(MonitoringService::ClassifyStockStatus(10, 0), StockStatus::SUFFICIENT);
}
