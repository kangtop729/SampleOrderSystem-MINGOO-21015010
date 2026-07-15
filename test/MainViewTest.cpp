#include "gtest/gtest.h"

#include <regex>
#include <sstream>
#include <string>

#include "../src/View/ConsoleFormat.h"
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

TEST(MainViewTest, ShowMainMenu_PDF와동일한신규메뉴라벨을_사용한다) {
    std::ostringstream oss;
    View::MainView view(oss);
    SystemStatusSummary summary{0, 0, 0, 0};

    view.ShowMainMenu(summary);

    const std::string output = oss.str();
    EXPECT_NE(output.find("[1] 시료 관리"), std::string::npos);
    EXPECT_NE(output.find("[2] 시료 주문"), std::string::npos);
    EXPECT_NE(output.find("[3] 주문 승인/거절"), std::string::npos);
    EXPECT_NE(output.find("[4] 모니터링"), std::string::npos);
    EXPECT_NE(output.find("[5] 생산라인 조회"), std::string::npos);
    EXPECT_NE(output.find("[6] 출고 처리"), std::string::npos);
    EXPECT_NE(output.find("[0] 종료"), std::string::npos);
}

TEST(MainViewTest, ShowMainMenu_S_SEMI로고배너가_포함된다) {
    std::ostringstream oss;
    View::MainView view(oss);
    SystemStatusSummary summary{0, 0, 0, 0};

    view.ShowMainMenu(summary);

    EXPECT_NE(oss.str().find("S-SEMI"), std::string::npos);
}

TEST(MainViewTest, ShowMainMenu_현재시각이_YYYY_MM_DD_HH_MM_SS형식으로_포함된다) {
    std::ostringstream oss;
    View::MainView view(oss);
    SystemStatusSummary summary{0, 0, 0, 0};

    view.ShowMainMenu(summary);

    static const std::regex kPattern(R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})");
    EXPECT_TRUE(std::regex_search(oss.str(), kPattern)) << "actual: " << oss.str();
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
