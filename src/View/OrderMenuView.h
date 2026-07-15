#pragma once

#include <iostream>
#include <ostream>
#include <string>

#include "../Model/Order.h"

namespace View {

// 주문 접수 메뉴 화면을 출력하는 View.
class OrderMenuView {
public:
    explicit OrderMenuView(std::ostream& out = std::cout);

    void ShowOrderPlaced(const Model::Order& order) const;
    void ShowError(const std::string& message) const;
    void ShowInputSummary(const std::string& sampleId, const std::string& sampleName,
                          const std::string& customerName, int quantity) const;
    void ShowConfirmPrompt() const;
    void ShowCancelled() const;

private:
    std::ostream& out_;
};

}  // namespace View
