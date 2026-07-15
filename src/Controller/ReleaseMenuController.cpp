#include "ReleaseMenuController.h"

#include "ConsoleInput.h"

namespace Controller {

namespace {
constexpr int kBackSelection = 0;
}  // namespace

ReleaseMenuController::ReleaseMenuController(std::istream& in, std::ostream& out, View::ReleaseMenuView& view,
                                              Service::ReleaseService& releaseService)
    : in_(in), out_(out), view_(view), releaseService_(releaseService) {}

void ReleaseMenuController::Run() {
    const std::vector<Model::Order> confirmedOrders = releaseService_.GetConfirmedOrders();
    view_.ShowConfirmedOrders(confirmedOrders);

    while (true) {
        const auto selectionOpt = ConsoleInput::ReadInt(in_, out_, "번호(0=뒤로) > ");
        if (!selectionOpt.has_value()) {
            view_.ShowInvalidSelection();
            continue;
        }

        const int selection = *selectionOpt;
        if (selection == kBackSelection) {
            return;
        }
        if (selection < 1 || static_cast<size_t>(selection) > confirmedOrders.size()) {
            view_.ShowInvalidSelection();
            continue;
        }

        const std::string orderNo = confirmedOrders[static_cast<size_t>(selection - 1)].GetOrderNo();

        try {
            releaseService_.ReleaseOrder(orderNo);
            view_.ShowReleaseResult(orderNo);
        } catch (const std::exception& e) {
            view_.ShowError(e.what());
        }
    }
}

}  // namespace Controller
