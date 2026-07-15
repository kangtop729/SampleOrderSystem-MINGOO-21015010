#pragma once

#include <istream>
#include <ostream>

#include "../Service/MonitoringService.h"
#include "../Service/OrderService.h"
#include "../Service/ProductionLineService.h"
#include "../Service/ReleaseService.h"
#include "../Service/SampleService.h"
#include "../View/MainView.h"
#include "ApprovalMenuController.h"
#include "MonitoringController.h"
#include "OrderMenuController.h"
#include "ProductionLineController.h"
#include "ReleaseMenuController.h"
#include "SampleMenuController.h"

namespace Controller {

// 메인 메뉴(시스템 현황 요약 + 서브 메뉴 라우팅)의 입출력 흐름을 담당하는 Controller.
class MainController {
public:
    MainController(std::istream& in, std::ostream& out, View::MainView& mainView, Service::SampleService& sampleService,
                   Service::OrderService& orderService, Service::ProductionLineService& productionLineService,
                   Service::MonitoringService& monitoringService, Service::ReleaseService& releaseService,
                   SampleMenuController& sampleMenuController, OrderMenuController& orderMenuController,
                   ApprovalMenuController& approvalMenuController, ProductionLineController& productionLineController,
                   MonitoringController& monitoringController, ReleaseMenuController& releaseMenuController);

    void Run();

private:
    View::SystemStatusSummary BuildSummary() const;

    std::istream& in_;
    std::ostream& out_;
    View::MainView& mainView_;
    Service::SampleService& sampleService_;
    Service::OrderService& orderService_;
    Service::ProductionLineService& productionLineService_;
    Service::MonitoringService& monitoringService_;
    Service::ReleaseService& releaseService_;

    SampleMenuController& sampleMenuController_;
    OrderMenuController& orderMenuController_;
    ApprovalMenuController& approvalMenuController_;
    ProductionLineController& productionLineController_;
    MonitoringController& monitoringController_;
    ReleaseMenuController& releaseMenuController_;
};

}  // namespace Controller
