#include "gtest/gtest.h"

#include <sstream>
#include <string>
#include <vector>

#include "../src/Model/Order.h"
#include "../src/Model/OrderStatus.h"
#include "../src/View/ApprovalMenuView.h"

namespace {

TEST(ApprovalMenuViewTest, ShowReservedOrders_각주문번호가_포함된다) {
    std::ostringstream oss;
    View::ApprovalMenuView view(oss);
    const std::vector<Model::Order> orders = {
        Model::Order("ORD-001", "SMP-001", "고객A", 10),
        Model::Order("ORD-002", "SMP-002", "고객B", 5),
    };

    view.ShowReservedOrders(orders);

    const std::string output = oss.str();
    EXPECT_NE(output.find("ORD-001"), std::string::npos);
    EXPECT_NE(output.find("ORD-002"), std::string::npos);
}

TEST(ApprovalMenuViewTest, ShowApprovalResult_주문번호와새상태배지가_포함된다) {
    std::ostringstream oss;
    View::ApprovalMenuView view(oss);

    view.ShowApprovalResult("ORD-001", Model::OrderStatus::CONFIRMED);

    const std::string output = oss.str();
    EXPECT_NE(output.find("ORD-001"), std::string::npos);
    EXPECT_NE(output.find("[CONFIRMED]"), std::string::npos);
}

TEST(ApprovalMenuViewTest, ShowRejectionResult_주문번호와REJECTED배지가_포함된다) {
    std::ostringstream oss;
    View::ApprovalMenuView view(oss);

    view.ShowRejectionResult("ORD-001");

    const std::string output = oss.str();
    EXPECT_NE(output.find("ORD-001"), std::string::npos);
    EXPECT_NE(output.find("[REJECTED]"), std::string::npos);
}

TEST(ApprovalMenuViewTest, ShowError_메시지가_그대로포함된다) {
    std::ostringstream oss;
    View::ApprovalMenuView view(oss);

    view.ShowError("승인 대상 주문이 없습니다.");

    EXPECT_NE(oss.str().find("승인 대상 주문이 없습니다."), std::string::npos);
}

}  // namespace
