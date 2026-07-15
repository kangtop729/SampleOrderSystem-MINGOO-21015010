#include "ApprovalMenuController.h"

#include "ConsoleInput.h"

namespace Controller {

namespace {
const std::string kBackSentinel = "0";
constexpr int kActionApprove = 1;
constexpr int kActionReject = 2;
}  // namespace

ApprovalMenuController::ApprovalMenuController(std::istream& in, std::ostream& out, View::ApprovalMenuView& view,
                                                 Service::OrderService& orderService)
    : in_(in), out_(out), view_(view), orderService_(orderService) {}

void ApprovalMenuController::Run() {
    view_.ShowReservedOrders(orderService_.GetReservedOrders());

    while (true) {
        const std::string orderNo = ConsoleInput::ReadLine(in_, out_, "주문번호(0=뒤로) > ");
        if (orderNo == kBackSentinel) {
            return;
        }

        const auto action = ConsoleInput::ReadInt(in_, out_, "1=승인 2=거절 > ");
        if (!action.has_value()) {
            view_.ShowError("잘못된 선택입니다.");
            continue;
        }

        if (*action == kActionApprove) {
            HandleApprove(orderNo);
        } else if (*action == kActionReject) {
            HandleReject(orderNo);
        } else {
            view_.ShowError("잘못된 선택입니다.");
        }
    }
}

void ApprovalMenuController::HandleApprove(const std::string& orderNo) {
    try {
        const auto status = orderService_.ApproveOrder(orderNo);
        view_.ShowApprovalResult(orderNo, status);
    } catch (const std::exception& e) {
        view_.ShowError(e.what());
    }
}

void ApprovalMenuController::HandleReject(const std::string& orderNo) {
    try {
        orderService_.RejectOrder(orderNo);
        view_.ShowRejectionResult(orderNo);
    } catch (const std::exception& e) {
        view_.ShowError(e.what());
    }
}

}  // namespace Controller
