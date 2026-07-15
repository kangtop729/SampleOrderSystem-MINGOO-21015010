#pragma once

#include <iostream>
#include <ostream>
#include <vector>

#include "../Service/MonitoringService.h"

namespace View {

// 모니터링 화면(주문 현황/재고 현황)을 출력하는 View.
class MonitoringView {
public:
    explicit MonitoringView(std::ostream& out = std::cout);

    void ShowOrderCounts(const Service::OrderStatusCounts& counts) const;
    void ShowStockStatuses(const std::vector<Service::SampleStockStatus>& statuses) const;

private:
    std::ostream& out_;
};

}  // namespace View
