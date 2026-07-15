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

TEST(OrderMenuViewTest, ShowInputSummary_시료ID_시료명_고객명_수량이_모두포함된다) {
    std::ostringstream oss;
    View::OrderMenuView view(oss);

    view.ShowInputSummary("SMP-001", "테스트시료", "홍길동", 5);

    const std::string output = oss.str();
    EXPECT_NE(output.find("SMP-001"), std::string::npos);
    EXPECT_NE(output.find("테스트시료"), std::string::npos);
    EXPECT_NE(output.find("홍길동"), std::string::npos);
    EXPECT_NE(output.find("5"), std::string::npos);
}

TEST(OrderMenuViewTest, ShowConfirmPrompt_YN안내문구가_포함된다) {
    std::ostringstream oss;
    View::OrderMenuView view(oss);

    view.ShowConfirmPrompt();

    const std::string output = oss.str();
    EXPECT_NE(output.find("Y"), std::string::npos);
    EXPECT_NE(output.find("N"), std::string::npos);
}

TEST(OrderMenuViewTest, ShowCancelled_취소안내문구가_출력된다) {
    std::ostringstream oss;
    View::OrderMenuView view(oss);

    view.ShowCancelled();

    EXPECT_NE(oss.str().find("취소"), std::string::npos);
}

}  // namespace
