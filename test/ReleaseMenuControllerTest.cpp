#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <sstream>
#include <vector>

#include "../src/Controller/ReleaseMenuController.h"
#include "../src/Model/Order.h"
#include "../src/Model/OrderStatus.h"
#include "../src/Service/ReleaseService.h"
#include "../src/View/ReleaseMenuView.h"
#include "mocks/MockOrderRepository.h"

using Model::Order;
using Model::OrderStatus;
using Service::ReleaseService;
using Test::MockOrderRepository;
using ::testing::_;
using ::testing::Property;
using ::testing::Eq;
using ::testing::Return;

class ReleaseMenuControllerTest : public ::testing::Test {
protected:
    MockOrderRepository orderRepository_;
};

TEST_F(ReleaseMenuControllerTest, Run_정상출고시_ReleaseOrder가호출되고결과가출력된다) {
    Order order("ORD-001", "SMP-001", "홍길동", 5);
    order.Confirm();
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::CONFIRMED)).WillOnce(Return(std::vector<Order>{order}));
    EXPECT_CALL(orderRepository_, FindById(std::string("ORD-001"))).WillOnce(Return(order));
    EXPECT_CALL(orderRepository_, Update(Property(&Order::GetStatus, Eq(OrderStatus::RELEASE)))).WillOnce(Return(true));

    std::istringstream in("1\n0\n");
    std::ostringstream out;
    View::ReleaseMenuView view(out);
    ReleaseService service(orderRepository_);
    Controller::ReleaseMenuController controller(in, out, view, service);

    controller.Run();

    EXPECT_NE(out.str().find("ORD-001"), std::string::npos);
}

TEST_F(ReleaseMenuControllerTest, Run_범위밖번호를선택하면_잘못된선택메시지가출력된다) {
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::CONFIRMED)).WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(orderRepository_, FindById(_)).Times(0);
    EXPECT_CALL(orderRepository_, Update(_)).Times(0);

    std::istringstream in("1\n0\n");
    std::ostringstream out;
    View::ReleaseMenuView view(out);
    ReleaseService service(orderRepository_);
    Controller::ReleaseMenuController controller(in, out, view, service);

    controller.Run();

    EXPECT_NE(out.str().find("잘못된 선택"), std::string::npos);
}

TEST_F(ReleaseMenuControllerTest, Run_CONFIRMED가아닌주문이면_상태전이예외메시지가출력된다) {
    Order order("ORD-001", "SMP-001", "홍길동", 5);
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::CONFIRMED)).WillOnce(Return(std::vector<Order>{order}));
    EXPECT_CALL(orderRepository_, FindById(std::string("ORD-001"))).WillOnce(Return(order));
    EXPECT_CALL(orderRepository_, Update(_)).Times(0);

    std::istringstream in("1\n0\n");
    std::ostringstream out;
    View::ReleaseMenuView view(out);
    ReleaseService service(orderRepository_);
    Controller::ReleaseMenuController controller(in, out, view, service);

    controller.Run();

    EXPECT_FALSE(out.str().empty());
}

TEST_F(ReleaseMenuControllerTest, Run_0을입력하면_즉시반환한다) {
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::CONFIRMED)).WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(orderRepository_, FindById(_)).Times(0);

    std::istringstream in("0\n");
    std::ostringstream out;
    View::ReleaseMenuView view(out);
    ReleaseService service(orderRepository_);
    Controller::ReleaseMenuController controller(in, out, view, service);

    controller.Run();
}
