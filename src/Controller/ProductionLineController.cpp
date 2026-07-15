#include "ProductionLineController.h"

#include "ConsoleInput.h"

namespace Controller {

namespace {
constexpr int kMenuComplete = 1;
constexpr int kMenuExit = 0;
}  // namespace

ProductionLineController::ProductionLineController(std::istream& in, std::ostream& out,
                                                     View::ProductionLineView& view,
                                                     Service::ProductionLineService& productionLineService)
    : in_(in), out_(out), view_(view), productionLineService_(productionLineService) {}

void ProductionLineController::ShowSubMenu() const {
    out_ << "[1] 완료 처리\n";
    out_ << "[0] 뒤로\n";
    out_ << "선택 > ";
}

void ProductionLineController::Run() {
    try {
        view_.ShowCurrentJob(productionLineService_.GetCurrentJob());
    } catch (const Service::NoProductionJobException&) {
        view_.ShowNoCurrentJob();
    }
    view_.ShowPendingQueue(productionLineService_.GetQueue());

    while (true) {
        ShowSubMenu();
        const auto choice = ConsoleInput::ReadInt(in_, out_, "");
        if (!choice.has_value()) {
            continue;
        }

        if (*choice == kMenuExit) {
            return;
        } else if (*choice == kMenuComplete) {
            HandleComplete();
        }
    }
}

void ProductionLineController::HandleComplete() {
    try {
        const auto job = productionLineService_.GetCurrentJob();
        const std::string orderNo = job.GetOrderNo();
        productionLineService_.CompleteCurrentJob();
        view_.ShowCompletionResult(orderNo);
    } catch (const Service::NoProductionJobException&) {
        view_.ShowNoCurrentJob();
    }
}

}  // namespace Controller
