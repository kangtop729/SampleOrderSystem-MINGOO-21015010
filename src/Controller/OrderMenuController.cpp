#include "OrderMenuController.h"

#include "ConsoleInput.h"

namespace Controller {

namespace {
const std::string kBackSentinel = "0";
}  // namespace

OrderMenuController::OrderMenuController(std::istream& in, std::ostream& out, View::OrderMenuView& view,
                                            Service::OrderService& orderService)
    : in_(in), out_(out), view_(view), orderService_(orderService) {}

void OrderMenuController::Run() {
    while (true) {
        const std::string sampleId = ConsoleInput::ReadLine(in_, out_, "시료ID(0=뒤로) > ");
        if (sampleId == kBackSentinel) {
            return;
        }

        const std::string customerName = ConsoleInput::ReadLine(in_, out_, "고객명 > ");
        const auto quantity = ConsoleInput::ReadInt(in_, out_, "수량 > ");
        if (!quantity.has_value()) {
            view_.ShowError("수량은 숫자로 입력해주세요.");
            continue;
        }

        try {
            const auto order = orderService_.PlaceOrder(sampleId, customerName, *quantity);
            view_.ShowOrderPlaced(order);
        } catch (const std::exception& e) {
            view_.ShowError(e.what());
        }
    }
}

}  // namespace Controller
