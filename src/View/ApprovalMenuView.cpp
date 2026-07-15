#include "ApprovalMenuView.h"

#include "ConsoleFormat.h"

namespace View {

ApprovalMenuView::ApprovalMenuView(std::ostream& out) : out_(out) {}

void ApprovalMenuView::ShowReservedOrders(const std::vector<Model::Order>& orders) const {
    const std::vector<std::string> headers = {"주문번호", "시료ID", "고객명", "수량", "상태"};
    std::vector<std::vector<std::string>> rows;
    rows.reserve(orders.size());
    for (const auto& order : orders) {
        rows.push_back({
            order.GetOrderNo(),
            order.GetSampleId(),
            order.GetCustomerName(),
            std::to_string(order.GetQuantity()),
            ConsoleFormat::FormatOrderStatusBadge(order.GetStatus()),
        });
    }
    out_ << ConsoleFormat::RenderTable(headers, rows);
}

void ApprovalMenuView::ShowApprovalResult(const std::string& orderNo, Model::OrderStatus newStatus) const {
    out_ << "주문 " << orderNo << "이(가) 승인되었습니다. " << ConsoleFormat::FormatOrderStatusBadge(newStatus)
         << "\n";
}

void ApprovalMenuView::ShowRejectionResult(const std::string& orderNo) const {
    out_ << "주문 " << orderNo << "이(가) 거절되었습니다. "
         << ConsoleFormat::FormatOrderStatusBadge(Model::OrderStatus::REJECTED) << "\n";
}

void ApprovalMenuView::ShowError(const std::string& message) const {
    out_ << ConsoleFormat::FormatErrorMessage(message);
}

}  // namespace View
