#pragma once

#include <istream>
#include <ostream>

#include <string>
#include <vector>

#include "../Model/Order.h"
#include "../Model/Sample.h"
#include "../Service/OrderService.h"
#include "../Service/SampleService.h"
#include "../View/ApprovalMenuView.h"

namespace Controller {

// 승인 처리 메뉴의 입출력 흐름을 담당하는 Controller.
class ApprovalMenuController {
public:
    ApprovalMenuController(std::istream& in, std::ostream& out, View::ApprovalMenuView& view,
                            Service::OrderService& orderService, Service::SampleService& sampleService);

    void Run();

private:
    void ShowApprovalPreview(const Model::Order& order, const Model::Sample& sample) const;
    void HandleDecision(const std::string& orderNo, const std::string& choice);
    void HandleApprove(const std::string& orderNo);
    void HandleReject(const std::string& orderNo);

    std::istream& in_;
    std::ostream& out_;
    View::ApprovalMenuView& view_;
    Service::OrderService& orderService_;
    Service::SampleService& sampleService_;
};

}  // namespace Controller
