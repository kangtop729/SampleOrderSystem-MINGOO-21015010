#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <sstream>
#include <vector>

#include "../src/Controller/ApprovalMenuController.h"
#include "../src/Controller/MainController.h"
#include "../src/Controller/MonitoringController.h"
#include "../src/Controller/OrderMenuController.h"
#include "../src/Controller/ProductionLineController.h"
#include "../src/Controller/ReleaseMenuController.h"
#include "../src/Controller/SampleMenuController.h"
#include "../src/Model/Order.h"
#include "../src/Model/OrderStatus.h"
#include "../src/Model/Sample.h"
#include "../src/Service/MonitoringService.h"
#include "../src/Service/OrderService.h"
#include "../src/Service/ProductionLineService.h"
#include "../src/Service/ReleaseService.h"
#include "../src/Service/SampleService.h"
#include "../src/View/ApprovalMenuView.h"
#include "../src/View/MainView.h"
#include "../src/View/MonitoringView.h"
#include "../src/View/OrderMenuView.h"
#include "../src/View/ProductionLineView.h"
#include "../src/View/ReleaseMenuView.h"
#include "../src/View/SampleMenuView.h"
#include "mocks/MockOrderRepository.h"
#include "mocks/MockSampleRepository.h"

using Model::Order;
using Model::OrderStatus;
using Model::Sample;
using Service::MonitoringService;
using Service::OrderService;
using Service::ProductionLineService;
using Service::ReleaseService;
using Service::SampleService;
using Test::MockOrderRepository;
using Test::MockSampleRepository;
using ::testing::Return;

namespace {

// 다양한 Service들이 참조하는 mock repository들을 "비어있음" 상태로 일관되게 세팅한다.
// MainController의 요약 정보 계산과 서브 메뉴 진입(호출) 양쪽에서 반복적으로 호출되므로
// WillRepeatedly로 스텁을 구성한다.
void StubEmptyRepositories(MockOrderRepository& orderRepository, MockSampleRepository& sampleRepository) {
    EXPECT_CALL(orderRepository, FindByStatus(::testing::_)).WillRepeatedly(Return(std::vector<Order>{}));
    EXPECT_CALL(sampleRepository, FindAll()).WillRepeatedly(Return(std::vector<Sample>{}));
    EXPECT_CALL(sampleRepository, FindByName(::testing::_)).WillRepeatedly(Return(std::vector<Sample>{}));
    EXPECT_CALL(orderRepository, FindBySampleId(::testing::_)).WillRepeatedly(Return(std::vector<Order>{}));
}

struct MainControllerFixtureContext {
    MockOrderRepository orderRepository;
    MockSampleRepository sampleRepository;

    View::MainView mainView;
    View::SampleMenuView sampleMenuView;
    View::OrderMenuView orderMenuView;
    View::ApprovalMenuView approvalMenuView;
    View::ProductionLineView productionLineView;
    View::MonitoringView monitoringView;
    View::ReleaseMenuView releaseMenuView;

    SampleService sampleService;
    OrderService orderService;
    ProductionLineService productionLineService;
    MonitoringService monitoringService;
    ReleaseService releaseService;

    Controller::SampleMenuController sampleMenuController;
    Controller::OrderMenuController orderMenuController;
    Controller::ApprovalMenuController approvalMenuController;
    Controller::ProductionLineController productionLineController;
    Controller::MonitoringController monitoringController;
    Controller::ReleaseMenuController releaseMenuController;

    explicit MainControllerFixtureContext(std::istream& in, std::ostream& out)
        : mainView(out),
          sampleMenuView(out),
          orderMenuView(out),
          approvalMenuView(out),
          productionLineView(out),
          monitoringView(out),
          releaseMenuView(out),
          sampleService(sampleRepository),
          orderService(orderRepository, sampleRepository),
          productionLineService(orderRepository, sampleRepository),
          monitoringService(orderRepository, sampleRepository),
          releaseService(orderRepository),
          sampleMenuController(in, out, sampleMenuView, sampleService),
          orderMenuController(in, out, orderMenuView, orderService),
          approvalMenuController(in, out, approvalMenuView, orderService),
          productionLineController(in, out, productionLineView, productionLineService),
          monitoringController(in, out, monitoringView, monitoringService),
          releaseMenuController(in, out, releaseMenuView, releaseService) {}
};

}  // namespace

TEST(MainControllerTest, Run_요약정보가_각Service의반환값을반영하여출력된다) {
    std::istringstream in("0\n");
    std::ostringstream out;
    MainControllerFixtureContext ctx(in, out);
    StubEmptyRepositories(ctx.orderRepository, ctx.sampleRepository);

    Controller::MainController controller(in, out, ctx.mainView, ctx.sampleService, ctx.orderService,
                                            ctx.productionLineService, ctx.monitoringService, ctx.releaseService,
                                            ctx.sampleMenuController, ctx.orderMenuController,
                                            ctx.approvalMenuController, ctx.productionLineController,
                                            ctx.monitoringController, ctx.releaseMenuController);

    controller.Run();

    // 모든 repository가 비어있으므로 요약 수치는 전부 0이어야 하고, 메인 메뉴가 최소 한 번 출력되어야 한다.
    EXPECT_NE(out.str().find("[0] 종료"), std::string::npos);
}

TEST(MainControllerTest, Run_선택1을입력하면_SampleMenuController로라우팅된다) {
    std::istringstream in("1\n0\n0\n");
    std::ostringstream out;
    MainControllerFixtureContext ctx(in, out);
    StubEmptyRepositories(ctx.orderRepository, ctx.sampleRepository);

    Controller::MainController controller(in, out, ctx.mainView, ctx.sampleService, ctx.orderService,
                                            ctx.productionLineService, ctx.monitoringService, ctx.releaseService,
                                            ctx.sampleMenuController, ctx.orderMenuController,
                                            ctx.approvalMenuController, ctx.productionLineController,
                                            ctx.monitoringController, ctx.releaseMenuController);

    controller.Run();

    EXPECT_NE(out.str().find("[1] 시료 등록"), std::string::npos);
}

TEST(MainControllerTest, Run_0을입력하면_종료메시지출력후_반환한다) {
    std::istringstream in("0\n");
    std::ostringstream out;
    MainControllerFixtureContext ctx(in, out);
    StubEmptyRepositories(ctx.orderRepository, ctx.sampleRepository);

    Controller::MainController controller(in, out, ctx.mainView, ctx.sampleService, ctx.orderService,
                                            ctx.productionLineService, ctx.monitoringService, ctx.releaseService,
                                            ctx.sampleMenuController, ctx.orderMenuController,
                                            ctx.approvalMenuController, ctx.productionLineController,
                                            ctx.monitoringController, ctx.releaseMenuController);

    controller.Run();

    EXPECT_FALSE(out.str().empty());
}

TEST(MainControllerTest, Run_잘못된선택시_안내메시지출력후_루프가계속된다) {
    std::istringstream in("9\n0\n");
    std::ostringstream out;
    MainControllerFixtureContext ctx(in, out);
    StubEmptyRepositories(ctx.orderRepository, ctx.sampleRepository);

    Controller::MainController controller(in, out, ctx.mainView, ctx.sampleService, ctx.orderService,
                                            ctx.productionLineService, ctx.monitoringService, ctx.releaseService,
                                            ctx.sampleMenuController, ctx.orderMenuController,
                                            ctx.approvalMenuController, ctx.productionLineController,
                                            ctx.monitoringController, ctx.releaseMenuController);

    controller.Run();

    EXPECT_NE(out.str().find("[0] 종료"), std::string::npos);
}
