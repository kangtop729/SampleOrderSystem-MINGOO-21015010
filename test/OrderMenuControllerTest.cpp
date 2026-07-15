#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <sstream>
#include <string>

#include "../src/Controller/OrderMenuController.h"
#include "../src/Model/Sample.h"
#include "../src/Service/OrderService.h"
#include "../src/Service/SampleService.h"
#include "../src/View/OrderMenuView.h"
#include "mocks/MockOrderRepository.h"
#include "mocks/MockSampleRepository.h"

using Model::Sample;
using Service::OrderService;
using Service::SampleService;
using Test::MockOrderRepository;
using Test::MockSampleRepository;
using ::testing::_;
using ::testing::Return;

class OrderMenuControllerTest : public ::testing::Test {
protected:
    MockOrderRepository orderRepository_;
    MockSampleRepository sampleRepository_;
};

TEST_F(OrderMenuControllerTest, Run_정상흐름에서_입력확인화면후Y입력시_주문이접수된다) {
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 10);
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillRepeatedly(Return(sample));
    EXPECT_CALL(orderRepository_, Save(_)).Times(1);

    std::istringstream in("SMP-001\n홍길동\n5\nY\n0\n");
    std::ostringstream out;
    View::OrderMenuView view(out);
    OrderService orderService(orderRepository_, sampleRepository_);
    SampleService sampleService(sampleRepository_);
    Controller::OrderMenuController controller(in, out, view, orderService, sampleService);

    controller.Run();

    const std::string output = out.str();
    EXPECT_NE(output.find("SMP-001"), std::string::npos);
    EXPECT_NE(output.find("테스트시료"), std::string::npos);
    EXPECT_NE(output.find("홍길동"), std::string::npos);
    EXPECT_NE(output.find("주문이 접수되었습니다"), std::string::npos);
}

TEST_F(OrderMenuControllerTest, Run_입력확인화면후N입력시_취소되고주문이생성되지않는다) {
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 10);
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillRepeatedly(Return(sample));
    EXPECT_CALL(orderRepository_, Save(_)).Times(0);

    std::istringstream in("SMP-001\n홍길동\n5\nN\n0\n");
    std::ostringstream out;
    View::OrderMenuView view(out);
    OrderService orderService(orderRepository_, sampleRepository_);
    SampleService sampleService(sampleRepository_);
    Controller::OrderMenuController controller(in, out, view, orderService, sampleService);

    controller.Run();

    EXPECT_NE(out.str().find("취소"), std::string::npos);
}

TEST_F(OrderMenuControllerTest, Run_존재하지않는sampleId면_에러메시지가출력되고_고객명수량을묻지않는다) {
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-999"))).WillOnce(Return(std::nullopt));
    EXPECT_CALL(orderRepository_, Save(_)).Times(0);

    std::istringstream in("SMP-999\n0\n");
    std::ostringstream out;
    View::OrderMenuView view(out);
    OrderService orderService(orderRepository_, sampleRepository_);
    SampleService sampleService(sampleRepository_);
    Controller::OrderMenuController controller(in, out, view, orderService, sampleService);

    controller.Run();

    EXPECT_NE(out.str().find("등록되지 않은 시료입니다."), std::string::npos);
}

TEST_F(OrderMenuControllerTest, Run_quantity파싱실패시_에러메시지가출력되고_주문이생성되지않는다) {
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 10);
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillRepeatedly(Return(sample));
    EXPECT_CALL(orderRepository_, Save(_)).Times(0);

    std::istringstream in("SMP-001\n홍길동\nabc\n0\n");
    std::ostringstream out;
    View::OrderMenuView view(out);
    OrderService orderService(orderRepository_, sampleRepository_);
    SampleService sampleService(sampleRepository_);
    Controller::OrderMenuController controller(in, out, view, orderService, sampleService);

    controller.Run();

    EXPECT_NE(out.str().find("수량은 숫자로 입력해주세요."), std::string::npos);
}

TEST_F(OrderMenuControllerTest, Run_0을입력하면_즉시반환한다) {
    EXPECT_CALL(sampleRepository_, FindById(_)).Times(0);
    EXPECT_CALL(orderRepository_, Save(_)).Times(0);

    std::istringstream in("0\n");
    std::ostringstream out;
    View::OrderMenuView view(out);
    OrderService orderService(orderRepository_, sampleRepository_);
    SampleService sampleService(sampleRepository_);
    Controller::OrderMenuController controller(in, out, view, orderService, sampleService);

    controller.Run();
}
