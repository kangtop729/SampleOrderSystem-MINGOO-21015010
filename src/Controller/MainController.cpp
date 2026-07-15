#include "MainController.h"

#include "ConsoleInput.h"

namespace Controller {

namespace {
constexpr int kMenuSample = 1;
constexpr int kMenuOrder = 2;
constexpr int kMenuApproval = 3;
constexpr int kMenuProduction = 4;
constexpr int kMenuMonitoring = 5;
constexpr int kMenuRelease = 6;
constexpr int kMenuExit = 0;
}  // namespace

MainController::MainController(std::istream& in, std::ostream& out, View::MainView& mainView,
                                 Service::SampleService& sampleService, Service::OrderService& orderService,
                                 Service::ProductionLineService& productionLineService,
                                 Service::MonitoringService& monitoringService, Service::ReleaseService& releaseService,
                                 SampleMenuController& sampleMenuController, OrderMenuController& orderMenuController,
                                 ApprovalMenuController& approvalMenuController,
                                 ProductionLineController& productionLineController,
                                 MonitoringController& monitoringController,
                                 ReleaseMenuController& releaseMenuController)
    : in_(in),
      out_(out),
      mainView_(mainView),
      sampleService_(sampleService),
      orderService_(orderService),
      productionLineService_(productionLineService),
      monitoringService_(monitoringService),
      releaseService_(releaseService),
      sampleMenuController_(sampleMenuController),
      orderMenuController_(orderMenuController),
      approvalMenuController_(approvalMenuController),
      productionLineController_(productionLineController),
      monitoringController_(monitoringController),
      releaseMenuController_(releaseMenuController) {}

View::SystemStatusSummary MainController::BuildSummary() const {
    productionLineService_.AutoCompleteFinishedJobs();

    const auto samples = sampleService_.GetAllSamples();
    long long totalStock = 0;
    for (const auto& sample : samples) {
        totalStock += sample.GetStock();
    }

    const auto counts = monitoringService_.CountOrdersByStatus();
    const int totalOrderCount = counts.reserved + counts.confirmed + counts.producing + counts.released;

    const int pendingProductionCount = static_cast<int>(productionLineService_.GetQueue().size());

    return View::SystemStatusSummary{static_cast<int>(samples.size()), totalStock, totalOrderCount,
                                      pendingProductionCount};
}

void MainController::Run() {
    while (true) {
        mainView_.ShowMainMenu(BuildSummary());
        const auto choice = ConsoleInput::ReadInt(in_, out_, "");
        if (!choice.has_value()) {
            mainView_.ShowInvalidSelectionMessage();
            continue;
        }

        switch (*choice) {
            case kMenuSample:
                sampleMenuController_.Run();
                break;
            case kMenuOrder:
                orderMenuController_.Run();
                break;
            case kMenuApproval:
                approvalMenuController_.Run();
                break;
            case kMenuProduction:
                productionLineController_.Run();
                break;
            case kMenuMonitoring:
                monitoringController_.Run();
                break;
            case kMenuRelease:
                releaseMenuController_.Run();
                break;
            case kMenuExit:
                mainView_.ShowExitMessage();
                return;
            default:
                mainView_.ShowInvalidSelectionMessage();
                break;
        }
    }
}

}  // namespace Controller
