#pragma once

#include <istream>
#include <ostream>

#include "../Service/OrderService.h"
#include "../View/ApprovalMenuView.h"

namespace Controller {

// 승인 처리 메뉴의 입출력 흐름을 담당하는 Controller.
class ApprovalMenuController {
public:
    ApprovalMenuController(std::istream& in, std::ostream& out, View::ApprovalMenuView& view,
                            Service::OrderService& orderService);

    void Run();

private:
    void HandleApprove(const std::string& orderNo);
    void HandleReject(const std::string& orderNo);

    std::istream& in_;
    std::ostream& out_;
    View::ApprovalMenuView& view_;
    Service::OrderService& orderService_;
};

}  // namespace Controller
