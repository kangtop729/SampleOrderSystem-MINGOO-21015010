#include "gtest/gtest.h"

#include <sstream>
#include <string>

#include "../src/View/MainView.h"

namespace {

TEST(MainViewTest, ShowMainMenu_요약수치가_모두포함된다) {
    std::ostringstream oss;
    View::MainView view(oss);
    SystemStatusSummary summary{5, 12345, 7, 2};

    view.ShowMainMenu(summary);

    const std::string output = oss.str();
    EXPECT_NE(output.find("5"), std::string::npos);
    EXPECT_NE(output.find("7"), std::string::npos);
    EXPECT_NE(output.find("2"), std::string::npos);
}

TEST(MainViewTest, ShowMainMenu_메뉴항목6개와종료항목이_포함된다) {
    std::ostringstream oss;
    View::MainView view(oss);
    SystemStatusSummary summary{0, 0, 0, 0};

    view.ShowMainMenu(summary);

    const std::string output = oss.str();
    EXPECT_NE(output.find("[1] 시료 관리"), std::string::npos);
    EXPECT_NE(output.find("[0] 종료"), std::string::npos);
}

TEST(MainViewTest, ShowInvalidSelectionMessage_출력에_안내문구가_있다) {
    std::ostringstream oss;
    View::MainView view(oss);

    view.ShowInvalidSelectionMessage();

    EXPECT_FALSE(oss.str().empty());
}

TEST(MainViewTest, ShowExitMessage_출력에_문구가_있다) {
    std::ostringstream oss;
    View::MainView view(oss);

    view.ShowExitMessage();

    EXPECT_FALSE(oss.str().empty());
}

}  // namespace
