#include "gtest/gtest.h"

#include <sstream>
#include <string>

#include "../src/Model/Order.h"
#include "../src/View/OrderMenuView.h"

namespace {

TEST(OrderMenuViewTest, ShowOrderPlaced_주문번호와RESERVED배지가_포함된다) {
    std::ostringstream oss;
    View::OrderMenuView view(oss);
    const Model::Order order("ORD-001", "SMP-001", "고객A", 10);

    view.ShowOrderPlaced(order);

    const std::string output = oss.str();
    EXPECT_NE(output.find("ORD-001"), std::string::npos);
    EXPECT_NE(output.find("[RESERVED]"), std::string::npos);
}

TEST(OrderMenuViewTest, ShowError_메시지가_그대로포함된다) {
    std::ostringstream oss;
    View::OrderMenuView view(oss);

    view.ShowError("수량은 0보다 커야 합니다.");

    EXPECT_NE(oss.str().find("수량은 0보다 커야 합니다."), std::string::npos);
}

}  // namespace
