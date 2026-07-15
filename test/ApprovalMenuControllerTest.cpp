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
#include "../src/View/ApprovalMenuView.h"
#include "mocks/MockOrderRepository.h"
#include "mocks/MockSampleRepository.h"

using Model::Order;
using Model::OrderStatus;
using Model::Sample;
using Service::OrderService;
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

TEST_F(ApprovalMenuControllerTest, Run_승인선택시_재고충분하면_CONFIRMED로전환되고결과가출력된다) {
    Order order("ORD-001", "SMP-001", "홍길동", 5);
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 10);
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::RESERVED)).WillOnce(Return(std::vector<Order>{order}));
    EXPECT_CALL(orderRepository_, FindById(std::string("ORD-001"))).WillOnce(Return(order));
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillOnce(Return(sample));
    EXPECT_CALL(orderRepository_, Update(Property(&Order::GetStatus, Eq(OrderStatus::CONFIRMED)))).WillOnce(Return(true));

    std::istringstream in("ORD-001\n1\n0\n");
    std::ostringstream out;
    View::ApprovalMenuView view(out);
    OrderService service(orderRepository_, sampleRepository_);
    Controller::ApprovalMenuController controller(in, out, view, service);

    controller.Run();

    EXPECT_NE(out.str().find("ORD-001"), std::string::npos);
}

TEST_F(ApprovalMenuControllerTest, Run_승인선택시_재고부족하면_PRODUCING으로전환되고결과가출력된다) {
    Order order("ORD-001", "SMP-001", "홍길동", 10);
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 5);
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::RESERVED)).WillOnce(Return(std::vector<Order>{order}));
    EXPECT_CALL(orderRepository_, FindById(std::string("ORD-001"))).WillOnce(Return(order));
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillOnce(Return(sample));
    EXPECT_CALL(orderRepository_, Update(Property(&Order::GetStatus, Eq(OrderStatus::PRODUCING)))).WillOnce(Return(true));

    std::istringstream in("ORD-001\n1\n0\n");
    std::ostringstream out;
    View::ApprovalMenuView view(out);
    OrderService service(orderRepository_, sampleRepository_);
    Controller::ApprovalMenuController controller(in, out, view, service);

    controller.Run();

    EXPECT_NE(out.str().find("ORD-001"), std::string::npos);
}

TEST_F(ApprovalMenuControllerTest, Run_거절선택시_RejectOrder가호출되고결과가출력된다) {
    Order order("ORD-001", "SMP-001", "홍길동", 5);
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::RESERVED)).WillOnce(Return(std::vector<Order>{order}));
    EXPECT_CALL(orderRepository_, FindById(std::string("ORD-001"))).WillOnce(Return(order));
    EXPECT_CALL(orderRepository_, Update(Property(&Order::GetStatus, Eq(OrderStatus::REJECTED)))).WillOnce(Return(true));

    std::istringstream in("ORD-001\n2\n0\n");
    std::ostringstream out;
    View::ApprovalMenuView view(out);
    OrderService service(orderRepository_, sampleRepository_);
    Controller::ApprovalMenuController controller(in, out, view, service);

    controller.Run();

    EXPECT_NE(out.str().find("ORD-001"), std::string::npos);
}

TEST_F(ApprovalMenuControllerTest, Run_존재하지않는주문번호면_에러메시지가출력된다) {
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::RESERVED)).WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(orderRepository_, FindById(std::string("ORD-999"))).WillOnce(Return(std::nullopt));
    EXPECT_CALL(orderRepository_, Update(_)).Times(0);

    std::istringstream in("ORD-999\n1\n0\n");
    std::ostringstream out;
    View::ApprovalMenuView view(out);
    OrderService service(orderRepository_, sampleRepository_);
    Controller::ApprovalMenuController controller(in, out, view, service);

    controller.Run();

    EXPECT_NE(out.str().find("존재하지 않는"), std::string::npos);
}

TEST_F(ApprovalMenuControllerTest, Run_0을입력하면_즉시반환한다) {
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::RESERVED)).WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(orderRepository_, FindById(_)).Times(0);

    std::istringstream in("0\n");
    std::ostringstream out;
    View::ApprovalMenuView view(out);
    OrderService service(orderRepository_, sampleRepository_);
    Controller::ApprovalMenuController controller(in, out, view, service);

    controller.Run();
}
