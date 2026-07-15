#include "ApprovalMenuController.h"

#include "../Service/ProductionLineService.h"
#include "ConsoleInput.h"

namespace Controller {

namespace {
constexpr int kBackSelection = 0;
}  // namespace

ApprovalMenuController::ApprovalMenuController(std::istream& in, std::ostream& out, View::ApprovalMenuView& view,
                                                 Service::OrderService& orderService,
                                                 Service::SampleService& sampleService)
    : in_(in), out_(out), view_(view), orderService_(orderService), sampleService_(sampleService) {}

void ApprovalMenuController::Run() {
    const std::vector<Model::Order> reservedOrders = orderService_.GetReservedOrders();
    view_.ShowReservedOrders(reservedOrders);

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
        if (selection < 1 || static_cast<size_t>(selection) > reservedOrders.size()) {
            view_.ShowInvalidSelection();
            continue;
        }

        const Model::Order& order = reservedOrders[static_cast<size_t>(selection - 1)];
        const auto sample = sampleService_.FindOne(order.GetSampleId());
        if (!sample.has_value()) {
            view_.ShowError("등록되지 않은 시료입니다.");
            continue;
        }

        ShowApprovalPreview(order, *sample);

        const std::string choice = ConsoleInput::ReadLine(in_, out_, "선택 > ");
        HandleDecision(order.GetOrderNo(), choice);
    }
}

void ApprovalMenuController::ShowApprovalPreview(const Model::Order& order, const Model::Sample& sample) const {
    view_.ShowCheckingStock();

    const int stock = sample.GetStock();
    const int quantity = order.GetQuantity();
    const int shortfall = Service::ProductionLineService::CalculateShortfall(quantity, stock);

    if (shortfall == 0) {
        view_.ShowSufficientApprovalPrompt(stock, quantity);
        return;
    }

    const int actualProductionQty =
        Service::ProductionLineService::CalculateActualProductionQty(shortfall, sample.GetYield());
    const double totalProductionMinutes = Service::ProductionLineService::CalculateTotalProductionMinutes(
        sample.GetAverageProductionTimeMinutes(), actualProductionQty);
    view_.ShowShortageApprovalPrompt(stock, quantity, shortfall, actualProductionQty, totalProductionMinutes);
}

void ApprovalMenuController::HandleDecision(const std::string& orderNo, const std::string& choice) {
    if (choice == "Y" || choice == "y") {
        HandleApprove(orderNo);
    } else {
        HandleReject(orderNo);
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
