#include "gtest/gtest.h"

#include <sstream>
#include <string>
#include <vector>

#include "../src/Model/Order.h"
#include "../src/View/ReleaseMenuView.h"

namespace {

TEST(ReleaseMenuViewTest, ShowConfirmedOrders_각주문번호가_포함된다) {
    std::ostringstream oss;
    View::ReleaseMenuView view(oss);
    const std::vector<Model::Order> orders = {
        Model::Order("ORD-001", "SMP-001", "고객A", 10),
        Model::Order("ORD-002", "SMP-002", "고객B", 5),
    };

    view.ShowConfirmedOrders(orders);

    const std::string output = oss.str();
    EXPECT_NE(output.find("ORD-001"), std::string::npos);
    EXPECT_NE(output.find("ORD-002"), std::string::npos);
}

TEST(ReleaseMenuViewTest, ShowReleaseResult_주문번호와RELEASE배지가_포함된다) {
    std::ostringstream oss;
    View::ReleaseMenuView view(oss);

    view.ShowReleaseResult("ORD-001");

    const std::string output = oss.str();
    EXPECT_NE(output.find("ORD-001"), std::string::npos);
    EXPECT_NE(output.find("[RELEASE]"), std::string::npos);
}

TEST(ReleaseMenuViewTest, ShowError_메시지가_그대로포함된다) {
    std::ostringstream oss;
    View::ReleaseMenuView view(oss);

    view.ShowError("출고 대상 주문이 없습니다.");

    EXPECT_NE(oss.str().find("출고 대상 주문이 없습니다."), std::string::npos);
}

}  // namespace
