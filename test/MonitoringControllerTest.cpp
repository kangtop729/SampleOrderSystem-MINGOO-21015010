#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include "../src/Controller/MonitoringController.h"
#include "../src/Model/Order.h"
#include "../src/Model/OrderStatus.h"
#include "../src/Model/Sample.h"
#include "../src/Service/MonitoringService.h"
#include "../src/View/MonitoringView.h"
#include "mocks/MockOrderRepository.h"
#include "mocks/MockSampleRepository.h"

using Model::Order;
using Model::OrderStatus;
using Model::Sample;
using Service::MonitoringService;
using Test::MockOrderRepository;
using Test::MockSampleRepository;
using ::testing::Return;

class MonitoringControllerTest : public ::testing::Test {
protected:
    MockOrderRepository orderRepository_;
    MockSampleRepository sampleRepository_;
};

TEST_F(MonitoringControllerTest, Run_주문량확인선택시_CountOrdersByStatus결과가출력된다) {
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::RESERVED))
        .WillOnce(Return(std::vector<Order>{Order("ORD-001", "SMP-001", "A", 5)}));
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::CONFIRMED)).WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::PRODUCING)).WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::RELEASE)).WillOnce(Return(std::vector<Order>{}));

    std::istringstream in("1\n0\n");
    std::ostringstream out;
    View::MonitoringView view(out);
    MonitoringService service(orderRepository_, sampleRepository_);
    Controller::MonitoringController controller(in, out, view, service);

    controller.Run();

    EXPECT_FALSE(out.str().empty());
}

TEST_F(MonitoringControllerTest, Run_재고량확인선택시_GetSampleStockStatuses결과가출력된다) {
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 10);
    EXPECT_CALL(sampleRepository_, FindAll()).WillOnce(Return(std::vector<Sample>{sample}));
    EXPECT_CALL(orderRepository_, FindBySampleId(std::string("SMP-001"))).WillOnce(Return(std::vector<Order>{}));

    std::istringstream in("2\n0\n");
    std::ostringstream out;
    View::MonitoringView view(out);
    MonitoringService service(orderRepository_, sampleRepository_);
    Controller::MonitoringController controller(in, out, view, service);

    controller.Run();

    EXPECT_NE(out.str().find("SMP-001"), std::string::npos);
}

TEST_F(MonitoringControllerTest, Run_0을입력하면_즉시반환한다) {
    EXPECT_CALL(orderRepository_, FindByStatus(::testing::_)).Times(0);
    EXPECT_CALL(sampleRepository_, FindAll()).Times(0);

    std::istringstream in("0\n");
    std::ostringstream out;
    View::MonitoringView view(out);
    MonitoringService service(orderRepository_, sampleRepository_);
    Controller::MonitoringController controller(in, out, view, service);

    controller.Run();
}
