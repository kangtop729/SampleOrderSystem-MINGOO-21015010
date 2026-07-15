#pragma once

#include <istream>
#include <ostream>

#include "../Service/OrderService.h"
#include "../Service/SampleService.h"
#include "../View/OrderMenuView.h"

namespace Controller {

// 주문 접수 메뉴의 입출력 흐름을 담당하는 Controller.
class OrderMenuController {
public:
    OrderMenuController(std::istream& in, std::ostream& out, View::OrderMenuView& view,
                         Service::OrderService& orderService, Service::SampleService& sampleService);

    void Run();

private:
    std::istream& in_;
    std::ostream& out_;
    View::OrderMenuView& view_;
    Service::OrderService& orderService_;
    Service::SampleService& sampleService_;
};

}  // namespace Controller
