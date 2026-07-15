#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "../Model/Order.h"
#include "../Model/OrderStatus.h"

namespace View {

// 승인 처리 메뉴 화면을 출력하는 View.
class ApprovalMenuView {
public:
    explicit ApprovalMenuView(std::ostream& out = std::cout);

    void ShowReservedOrders(const std::vector<Model::Order>& orders) const;
    void ShowApprovalResult(const std::string& orderNo, Model::OrderStatus newStatus) const;
    void ShowRejectionResult(const std::string& orderNo) const;
    void ShowError(const std::string& message) const;

private:
    std::ostream& out_;
};

}  // namespace View
