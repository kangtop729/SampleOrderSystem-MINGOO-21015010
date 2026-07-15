#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "../Model/Order.h"

namespace View {

// 출고 처리 메뉴 화면을 출력하는 View.
class ReleaseMenuView {
public:
    explicit ReleaseMenuView(std::ostream& out = std::cout);

    void ShowConfirmedOrders(const std::vector<Model::Order>& orders) const;
    void ShowReleaseResult(const std::string& orderNo) const;
    void ShowError(const std::string& message) const;

private:
    std::ostream& out_;
};

}  // namespace View
