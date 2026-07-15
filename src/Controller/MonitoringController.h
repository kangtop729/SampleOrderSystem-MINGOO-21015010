#pragma once

#include <istream>
#include <ostream>

#include "../Service/MonitoringService.h"
#include "../View/MonitoringView.h"

namespace Controller {

// 모니터링 메뉴(주문 현황/재고 현황)의 입출력 흐름을 담당하는 Controller.
class MonitoringController {
public:
    MonitoringController(std::istream& in, std::ostream& out, View::MonitoringView& view,
                          Service::MonitoringService& monitoringService);

    void Run();

private:
    void ShowSubMenu() const;

    std::istream& in_;
    std::ostream& out_;
    View::MonitoringView& view_;
    Service::MonitoringService& monitoringService_;
};

}  // namespace Controller
