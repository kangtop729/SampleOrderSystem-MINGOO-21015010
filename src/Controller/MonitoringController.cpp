#include "MonitoringController.h"

#include "ConsoleInput.h"

namespace Controller {

namespace {
constexpr int kMenuOrderCounts = 1;
constexpr int kMenuStockStatuses = 2;
constexpr int kMenuExit = 0;
}  // namespace

MonitoringController::MonitoringController(std::istream& in, std::ostream& out, View::MonitoringView& view,
                                            Service::MonitoringService& monitoringService)
    : in_(in), out_(out), view_(view), monitoringService_(monitoringService) {}

void MonitoringController::ShowSubMenu() const {
    out_ << "[1] 주문 현황\n";
    out_ << "[2] 재고 현황\n";
    out_ << "[0] 뒤로\n";
    out_ << "선택 > ";
}

void MonitoringController::Run() {
    while (true) {
        ShowSubMenu();
        const auto choice = ConsoleInput::ReadInt(in_, out_, "");
        if (!choice.has_value()) {
            continue;
        }

        if (*choice == kMenuExit) {
            return;
        } else if (*choice == kMenuOrderCounts) {
            view_.ShowOrderCounts(monitoringService_.CountOrdersByStatus());
        } else if (*choice == kMenuStockStatuses) {
            view_.ShowStockStatuses(monitoringService_.GetSampleStockStatuses());
        }
    }
}

}  // namespace Controller
