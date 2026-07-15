#include "gtest/gtest.h"

#include <sstream>
#include <string>
#include <vector>

#include "../src/Service/MonitoringService.h"
#include "../src/View/MonitoringView.h"

namespace {

TEST(MonitoringViewTest, ShowOrderCounts_상태별숫자가_모두포함된다) {
    std::ostringstream oss;
    View::MonitoringView view(oss);
    Service::OrderStatusCounts counts;
    counts.reserved = 3;
    counts.confirmed = 4;
    counts.producing = 5;
    counts.released = 6;

    view.ShowOrderCounts(counts);

    const std::string output = oss.str();
    EXPECT_NE(output.find("3"), std::string::npos);
    EXPECT_NE(output.find("4"), std::string::npos);
    EXPECT_NE(output.find("5"), std::string::npos);
    EXPECT_NE(output.find("6"), std::string::npos);
}

TEST(MonitoringViewTest, ShowStockStatuses_시료명재고상태라벨이_포함된다) {
    std::ostringstream oss;
    View::MonitoringView view(oss);
    const std::vector<Service::SampleStockStatus> statuses = {
        Service::SampleStockStatus{"SMP-001", "Alpha", 0, 5, Service::StockStatus::DEPLETED},
        Service::SampleStockStatus{"SMP-002", "Beta", 100, 5, Service::StockStatus::SUFFICIENT},
    };

    view.ShowStockStatuses(statuses);

    const std::string output = oss.str();
    EXPECT_NE(output.find("Alpha"), std::string::npos);
    EXPECT_NE(output.find("Beta"), std::string::npos);
    EXPECT_NE(output.find("고갈"), std::string::npos);
    EXPECT_NE(output.find("여유"), std::string::npos);
}

}  // namespace
