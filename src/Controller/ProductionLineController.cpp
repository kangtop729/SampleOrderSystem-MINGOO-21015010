#include "ProductionLineController.h"

#include "ConsoleInput.h"

namespace Controller {

namespace {
constexpr int kMenuExit = 0;
}  // namespace

ProductionLineController::ProductionLineController(std::istream& in, std::ostream& out,
                                                     View::ProductionLineView& view,
                                                     Service::ProductionLineService& productionLineService)
    : in_(in), out_(out), view_(view), productionLineService_(productionLineService) {}

void ProductionLineController::ShowSubMenu() const {
    out_ << "[0] 뒤로\n";
    out_ << "선택 > ";
}

void ProductionLineController::Run() {
    productionLineService_.AutoCompleteFinishedJobs();

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
        }
    }
}

}  // namespace Controller
