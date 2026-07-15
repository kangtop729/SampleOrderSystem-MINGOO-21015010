#include "ApprovalMenuView.h"

#include "ConsoleFormat.h"

namespace View {

ApprovalMenuView::ApprovalMenuView(std::ostream& out) : out_(out) {}

void ApprovalMenuView::ShowReservedOrders(const std::vector<Model::Order>& orders) const {
    const std::vector<std::string> headers = {"번호", "주문번호", "시료ID", "고객명", "수량", "상태"};
    std::vector<std::vector<std::string>> rows;
    rows.reserve(orders.size());
    for (size_t i = 0; i < orders.size(); ++i) {
        const auto& order = orders[i];
        rows.push_back({
            std::to_string(i + 1),
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

void ApprovalMenuView::ShowCheckingStock() const {
    out_ << "재고 확인 중...\n";
}

void ApprovalMenuView::ShowSufficientApprovalPrompt(int stock, int quantity) const {
    out_ << "재고 충분. 현재 재고 " << stock << " ea, 주문 수량 " << quantity << " ea. 승인하시겠습니까?\n";
    out_ << "[Y] 예  [N] 아니오\n";
}

void ApprovalMenuView::ShowShortageApprovalPrompt(int stock, int quantity, int shortfall, int actualProductionQty,
                                                  double totalProductionMinutes) const {
    out_ << "재고 부족. 현재 재고 " << stock << " ea, 주문 수량 " << quantity << " ea. 부족분 " << shortfall
         << " ea 승인하시겠습니까? (실생산량 " << actualProductionQty << " ea / " << totalProductionMinutes
         << " min)\n";
    out_ << "[Y] 예  [N] 아니오\n";
}

void ApprovalMenuView::ShowInvalidSelection() const {
    out_ << ConsoleFormat::FormatErrorMessage("잘못된 선택입니다.");
}

}  // namespace View
