#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "../src/Controller/ApprovalMenuController.h"
#include "../src/Model/Order.h"
#include "../src/Model/OrderStatus.h"
#include "../src/Model/Sample.h"
#include "../src/Service/OrderService.h"
#include "../src/Service/SampleService.h"
#include "../src/View/ApprovalMenuView.h"
#include "mocks/MockOrderRepository.h"
#include "mocks/MockSampleRepository.h"

using Model::Order;
using Model::OrderStatus;
using Model::Sample;
using Service::OrderService;
using Service::SampleService;
using Test::MockOrderRepository;
using Test::MockSampleRepository;
using ::testing::_;
using ::testing::Property;
using ::testing::Eq;
using ::testing::Return;

class ApprovalMenuControllerTest : public ::testing::Test {
protected:
    MockOrderRepository orderRepository_;
    MockSampleRepository sampleRepository_;
};

TEST_F(ApprovalMenuControllerTest, Run_번호선택후_재고충분하고Y입력시_CONFIRMED로전환되고결과가출력된다) {
    Order order("ORD-001", "SMP-001", "홍길동", 5);
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 10);
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::RESERVED)).WillOnce(Return(std::vector<Order>{order}));
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillRepeatedly(Return(sample));
    EXPECT_CALL(orderRepository_, FindById(std::string("ORD-001"))).WillOnce(Return(order));
    EXPECT_CALL(orderRepository_, Update(Property(&Order::GetStatus, Eq(OrderStatus::CONFIRMED)))).WillOnce(Return(true));

    std::istringstream in("1\nY\n0\n");
    std::ostringstream out;
    View::ApprovalMenuView view(out);
    OrderService orderService(orderRepository_, sampleRepository_);
    SampleService sampleService(sampleRepository_);
    Controller::ApprovalMenuController controller(in, out, view, orderService, sampleService);

    controller.Run();

    const std::string output = out.str();
    EXPECT_NE(output.find("ORD-001"), std::string::npos);
    EXPECT_NE(output.find("[CONFIRMED]"), std::string::npos);
}

TEST_F(ApprovalMenuControllerTest, Run_번호선택후_재고부족하고Y입력시_PRODUCING으로전환되고_부족분실생산량생산시간이정확히출력된다) {
    Order order("ORD-001", "SMP-001", "홍길동", 200);
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 30);
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::RESERVED)).WillOnce(Return(std::vector<Order>{order}));
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillRepeatedly(Return(sample));
    EXPECT_CALL(orderRepository_, FindById(std::string("ORD-001"))).WillOnce(Return(order));
    EXPECT_CALL(orderRepository_, Update(Property(&Order::GetStatus, Eq(OrderStatus::PRODUCING)))).WillOnce(Return(true));

    std::istringstream in("1\nY\n0\n");
    std::ostringstream out;
    View::ApprovalMenuView view(out);
    OrderService orderService(orderRepository_, sampleRepository_);
    SampleService sampleService(sampleRepository_);
    Controller::ApprovalMenuController controller(in, out, view, orderService, sampleService);

    controller.Run();

    // 부족분 = 200 - 30 = 170, 실생산량 = ceil(170 / 0.8) = 213(0.8*212=169.6 -> 213), 생산시간 = 30 * 213 = 6390
    const std::string output = out.str();
    EXPECT_NE(output.find("170"), std::string::npos);
    EXPECT_NE(output.find("213"), std::string::npos);
    EXPECT_NE(output.find("6390"), std::string::npos);
    EXPECT_NE(output.find("[PRODUCING]"), std::string::npos);
}

TEST_F(ApprovalMenuControllerTest, Run_번호선택후_N입력시_거절되고결과가출력된다) {
    Order order("ORD-001", "SMP-001", "홍길동", 5);
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 10);
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::RESERVED)).WillOnce(Return(std::vector<Order>{order}));
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillRepeatedly(Return(sample));
    EXPECT_CALL(orderRepository_, FindById(std::string("ORD-001"))).WillOnce(Return(order));
    EXPECT_CALL(orderRepository_, Update(Property(&Order::GetStatus, Eq(OrderStatus::REJECTED)))).WillOnce(Return(true));

    std::istringstream in("1\nN\n0\n");
    std::ostringstream out;
    View::ApprovalMenuView view(out);
    OrderService orderService(orderRepository_, sampleRepository_);
    SampleService sampleService(sampleRepository_);
    Controller::ApprovalMenuController controller(in, out, view, orderService, sampleService);

    controller.Run();

    const std::string output = out.str();
    EXPECT_NE(output.find("ORD-001"), std::string::npos);
    EXPECT_NE(output.find("[REJECTED]"), std::string::npos);
}

TEST_F(ApprovalMenuControllerTest, Run_범위밖번호선택시_잘못된선택안내후_다시선택할수있다) {
    Order order("ORD-001", "SMP-001", "홍길동", 5);
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 10);
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::RESERVED)).WillOnce(Return(std::vector<Order>{order}));
    EXPECT_CALL(sampleRepository_, FindById(_)).Times(0);
    EXPECT_CALL(orderRepository_, FindById(_)).Times(0);
    EXPECT_CALL(orderRepository_, Update(_)).Times(0);

    std::istringstream in("5\n0\n");
    std::ostringstream out;
    View::ApprovalMenuView view(out);
    OrderService orderService(orderRepository_, sampleRepository_);
    SampleService sampleService(sampleRepository_);
    Controller::ApprovalMenuController controller(in, out, view, orderService, sampleService);

    controller.Run();
}

TEST_F(ApprovalMenuControllerTest, Run_0을입력하면_즉시반환한다) {
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::RESERVED)).WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(sampleRepository_, FindById(_)).Times(0);
    EXPECT_CALL(orderRepository_, FindById(_)).Times(0);

    std::istringstream in("0\n");
    std::ostringstream out;
    View::ApprovalMenuView view(out);
    OrderService orderService(orderRepository_, sampleRepository_);
    SampleService sampleService(sampleRepository_);
    Controller::ApprovalMenuController controller(in, out, view, orderService, sampleService);

    controller.Run();
}
