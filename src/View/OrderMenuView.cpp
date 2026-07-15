#include "OrderMenuView.h"

#include "ConsoleFormat.h"

namespace View {

OrderMenuView::OrderMenuView(std::ostream& out) : out_(out) {}

void OrderMenuView::ShowOrderPlaced(const Model::Order& order) const {
    out_ << "주문이 접수되었습니다. (주문번호: " << order.GetOrderNo() << ") "
         << ConsoleFormat::FormatOrderStatusBadge(order.GetStatus()) << "\n";
}

void OrderMenuView::ShowError(const std::string& message) const {
    out_ << ConsoleFormat::FormatErrorMessage(message);
}

}  // namespace View
