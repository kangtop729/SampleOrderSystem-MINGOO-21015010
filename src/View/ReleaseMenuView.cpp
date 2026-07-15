#include "ReleaseMenuView.h"

#include "ConsoleFormat.h"

namespace View {

ReleaseMenuView::ReleaseMenuView(std::ostream& out) : out_(out) {}

void ReleaseMenuView::ShowConfirmedOrders(const std::vector<Model::Order>& orders) const {
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
    out_ << ConsoleFormat::MakeTitleBanner("출고 처리");
    out_ << ConsoleFormat::RenderTable(headers, rows);
}

void ReleaseMenuView::ShowReleaseResult(const std::string& orderNo) const {
    out_ << "주문 " << orderNo << "이(가) 출고되었습니다. "
         << ConsoleFormat::FormatOrderStatusBadge(Model::OrderStatus::RELEASE) << "\n";
}

void ReleaseMenuView::ShowError(const std::string& message) const {
    out_ << ConsoleFormat::FormatErrorMessage(message);
}

void ReleaseMenuView::ShowInvalidSelection() const {
    out_ << ConsoleFormat::FormatErrorMessage("잘못된 선택입니다.");
}

}  // namespace View
