#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <sstream>
#include <thread>
#include <vector>

#include "../src/Controller/ProductionLineController.h"
#include "../src/Model/Order.h"
#include "../src/Model/OrderStatus.h"
#include "../src/Model/Sample.h"
#include "../src/Service/ProductionLineService.h"
#include "../src/View/ProductionLineView.h"
#include "mocks/MockOrderRepository.h"
#include "mocks/MockSampleRepository.h"

using Model::Order;
using Model::OrderStatus;
using Model::Sample;
using Service::ProductionLineService;
using Test::MockOrderRepository;
using Test::MockSampleRepository;
using ::testing::Return;

namespace {

Order MakeProducingOrder(const std::string& orderNo, const std::string& sampleId, const std::string& customerName,
                          int quantity) {
    Order order(orderNo, sampleId, customerName, quantity);
    order.StartProducing();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    return order;
}

}  // namespace

class ProductionLineControllerTest : public ::testing::Test {
protected:
    MockOrderRepository orderRepository_;
    MockSampleRepository sampleRepository_;
};

TEST_F(ProductionLineControllerTest, Run_대기중인작업이있으면_현재작업과대기열이출력된다) {
    Sample sample("SMP-001", "테스트시료", 30.0, 0.8, 5);
    Order order = MakeProducingOrder("ORD-100", "SMP-001", "홍길동", 10);
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::PRODUCING))
        .WillRepeatedly(Return(std::vector<Order>{order}));
    EXPECT_CALL(sampleRepository_, FindById(std::string("SMP-001"))).WillRepeatedly(Return(sample));

    std::istringstream in("0\n");
    std::ostringstream out;
    View::ProductionLineView view(out);
    ProductionLineService service(orderRepository_, sampleRepository_);
    Controller::ProductionLineController controller(in, out, view, service);

    controller.Run();

    EXPECT_NE(out.str().find("ORD-100"), std::string::npos);
}

TEST_F(ProductionLineControllerTest, Run_대기중인작업이없으면_작업없음메시지가출력된다) {
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::PRODUCING)).WillRepeatedly(Return(std::vector<Order>{}));

    std::istringstream in("0\n");
    std::ostringstream out;
    View::ProductionLineView view(out);
    ProductionLineService service(orderRepository_, sampleRepository_);
    Controller::ProductionLineController controller(in, out, view, service);

    controller.Run();

    EXPECT_FALSE(out.str().empty());
}

TEST_F(ProductionLineControllerTest, Run_0을입력하면_즉시반환한다) {
    EXPECT_CALL(orderRepository_, FindByStatus(OrderStatus::PRODUCING)).WillRepeatedly(Return(std::vector<Order>{}));

    std::istringstream in("0\n");
    std::ostringstream out;
    View::ProductionLineView view(out);
    ProductionLineService service(orderRepository_, sampleRepository_);
    Controller::ProductionLineController controller(in, out, view, service);

    controller.Run();
}
