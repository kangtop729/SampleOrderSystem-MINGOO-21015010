// Release 구성 진입점.
// Debug 구성은 GoogleTest 러너(test/TestMain.cpp)가 진입점을 제공하므로 이 파일은
// Release 구성 링크를 위해서만 존재한다.
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

#include "Controller/ApprovalMenuController.h"
#include "Controller/MainController.h"
#include "Controller/MonitoringController.h"
#include "Controller/OrderMenuController.h"
#include "Controller/ProductionLineController.h"
#include "Controller/ReleaseMenuController.h"
#include "Controller/SampleMenuController.h"
#include "Repository/JsonOrderRepository.h"
#include "Repository/JsonSampleRepository.h"
#include "Service/MonitoringService.h"
#include "Service/OrderService.h"
#include "Service/ProductionLineService.h"
#include "Service/ReleaseService.h"
#include "Service/SampleService.h"
#include "View/ApprovalMenuView.h"
#include "View/MainView.h"
#include "View/MonitoringView.h"
#include "View/OrderMenuView.h"
#include "View/ProductionLineView.h"
#include "View/ReleaseMenuView.h"
#include "View/SampleMenuView.h"

int main() {
#ifdef _WIN32
    // 소스 파일은 UTF-8로 저장되어 있으므로, Windows 콘솔의 입출력 코드페이지를 UTF-8(65001)로
    // 맞춰야 한글이 깨지지 않고 출력된다.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    Repository::JsonSampleRepository sampleRepository("samples.json");
    Repository::JsonOrderRepository orderRepository("orders.json");

    Service::SampleService sampleService(sampleRepository);
    Service::OrderService orderService(orderRepository, sampleRepository);
    Service::ProductionLineService productionLineService(orderRepository, sampleRepository);
    Service::MonitoringService monitoringService(orderRepository, sampleRepository);
    Service::ReleaseService releaseService(orderRepository);

    View::MainView mainView(std::cout);
    View::SampleMenuView sampleMenuView(std::cout);
    View::OrderMenuView orderMenuView(std::cout);
    View::ApprovalMenuView approvalMenuView(std::cout);
    View::MonitoringView monitoringView(std::cout);
    View::ProductionLineView productionLineView(std::cout);
    View::ReleaseMenuView releaseMenuView(std::cout);

    Controller::SampleMenuController sampleMenuController(std::cin, std::cout, sampleMenuView, sampleService);
    Controller::OrderMenuController orderMenuController(std::cin, std::cout, orderMenuView, orderService,
                                                          sampleService);
    Controller::ApprovalMenuController approvalMenuController(std::cin, std::cout, approvalMenuView, orderService,
                                                               sampleService);
    Controller::ProductionLineController productionLineController(std::cin, std::cout, productionLineView,
                                                                     productionLineService);
    Controller::MonitoringController monitoringController(std::cin, std::cout, monitoringView, monitoringService);
    Controller::ReleaseMenuController releaseMenuController(std::cin, std::cout, releaseMenuView, releaseService);

    Controller::MainController mainController(
        std::cin, std::cout, mainView, sampleService, orderService, productionLineService, monitoringService,
        releaseService, sampleMenuController, orderMenuController, approvalMenuController, productionLineController,
        monitoringController, releaseMenuController);

    mainController.Run();

    return 0;
}
