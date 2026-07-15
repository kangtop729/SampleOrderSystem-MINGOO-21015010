#include "OrderMenuView.h"

#include "ConsoleFormat.h"

namespace View {

OrderMenuView::OrderMenuView(std::ostream& out) : out_(out) {}

void OrderMenuView::ShowOrderPlaced(const Model::Order& order) const {
    out_ << "주문이 접수되었습니다. (주문번호: " << order.GetOrderNo() << ", 고객명: " << order.GetCustomerName()
         << ") " << ConsoleFormat::FormatOrderStatusBadge(order.GetStatus()) << "\n";
}

void OrderMenuView::ShowError(const std::string& message) const {
    out_ << ConsoleFormat::FormatErrorMessage(message);
}

void OrderMenuView::ShowInputSummary(const std::string& sampleId, const std::string& sampleName,
                                     const std::string& customerName, int quantity) const {
    const std::vector<std::string> headers = {"시료ID", "시료명", "고객명", "수량"};
    const std::vector<std::vector<std::string>> rows = {
        {sampleId, sampleName, customerName, std::to_string(quantity)},
    };
    out_ << ConsoleFormat::MakeDivider() << "\n";
    out_ << ConsoleFormat::RenderTable(headers, rows);
    out_ << ConsoleFormat::MakeDivider() << "\n";
}

void OrderMenuView::ShowConfirmPrompt() const {
    out_ << "[Y] 예약 접수  [N] 취소\n";
}

void OrderMenuView::ShowCancelled() const {
    out_ << "주문 접수가 취소되었습니다.\n";
}

}  // namespace View
