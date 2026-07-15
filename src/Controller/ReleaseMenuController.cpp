#include "ReleaseMenuController.h"

#include "ConsoleInput.h"

namespace Controller {

namespace {
const std::string kBackSentinel = "0";
}  // namespace

ReleaseMenuController::ReleaseMenuController(std::istream& in, std::ostream& out, View::ReleaseMenuView& view,
                                              Service::ReleaseService& releaseService)
    : in_(in), out_(out), view_(view), releaseService_(releaseService) {}

void ReleaseMenuController::Run() {
    view_.ShowConfirmedOrders(releaseService_.GetConfirmedOrders());

    while (true) {
        const std::string orderNo = ConsoleInput::ReadLine(in_, out_, "주문번호(0=뒤로) > ");
        if (orderNo == kBackSentinel) {
            return;
        }

        try {
            releaseService_.ReleaseOrder(orderNo);
            view_.ShowReleaseResult(orderNo);
        } catch (const std::exception& e) {
            view_.ShowError(e.what());
        }
    }
}

}  // namespace Controller
