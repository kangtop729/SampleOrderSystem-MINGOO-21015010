#include "gtest/gtest.h"

#include <algorithm>
#include <chrono>
#include <regex>
#include <string>
#include <vector>

#include "../src/Model/OrderStatus.h"
#include "../src/Service/MonitoringService.h"
#include "../src/View/ConsoleFormat.h"

namespace {
constexpr const char* kAnsiEscapePrefix = "\x1b[";
}  // namespace

namespace {

// ---------------------------------------------------------------------------
// MakeDivider
// ---------------------------------------------------------------------------

TEST(ConsoleFormatTest, MakeDivider_기본값이면_길이60에_전부등호이다) {
    const std::string divider = View::ConsoleFormat::MakeDivider();

    ASSERT_EQ(divider.size(), 60u);
    EXPECT_EQ(divider.find_first_not_of('='), std::string::npos);
}

TEST(ConsoleFormatTest, MakeDivider_커스텀폭과문자를_지정하면_그대로반영된다) {
    const std::string divider = View::ConsoleFormat::MakeDivider(10, '-');

    ASSERT_EQ(divider.size(), 10u);
    EXPECT_EQ(divider.find_first_not_of('-'), std::string::npos);
}

// ---------------------------------------------------------------------------
// MakeTitleBanner
// ---------------------------------------------------------------------------

TEST(ConsoleFormatTest, MakeTitleBanner_결과에_제목문자열이_포함된다) {
    const std::string banner = View::ConsoleFormat::MakeTitleBanner("주문 현황");

    EXPECT_NE(banner.find("주문 현황"), std::string::npos);
}

TEST(ConsoleFormatTest, MakeTitleBanner_구분선이_위아래로_포함된다) {
    const std::string banner = View::ConsoleFormat::MakeTitleBanner("Title", 20);
    const std::string divider = View::ConsoleFormat::MakeDivider(20, '=');

    const size_t firstDividerPos = banner.find(divider);
    ASSERT_NE(firstDividerPos, std::string::npos);

    const size_t secondDividerPos = banner.find(divider, firstDividerPos + divider.size());
    EXPECT_NE(secondDividerPos, std::string::npos);
}

// ---------------------------------------------------------------------------
// RenderTable
// ---------------------------------------------------------------------------

TEST(ConsoleFormatTest, RenderTable_헤더와_셀값이_결과에_포함된다) {
    const std::vector<std::string> headers = {"ID"};
    const std::vector<std::vector<std::string>> rows = {{"SMP-001"}};

    const std::string table = View::ConsoleFormat::RenderTable(headers, rows);

    EXPECT_NE(table.find("ID"), std::string::npos);
    EXPECT_NE(table.find("SMP-001"), std::string::npos);
}

TEST(ConsoleFormatTest, RenderTable_컬럼폭이_가장긴셀값에_맞춰지고_잘리지않는다) {
    const std::vector<std::string> headers = {"ID"};
    const std::vector<std::vector<std::string>> rows = {{"SMP-001"}};

    const std::string table = View::ConsoleFormat::RenderTable(headers, rows);

    // 가장 긴 값 "SMP-001"(7자)이 잘리지 않고 통째로 존재해야 한다.
    const size_t pos = table.find("SMP-001");
    ASSERT_NE(pos, std::string::npos);
    EXPECT_EQ(table.substr(pos, 7), "SMP-001");
}

TEST(ConsoleFormatTest, RenderTable_행이여러개면_순서대로_모두포함된다) {
    const std::vector<std::string> headers = {"ID", "Name"};
    const std::vector<std::vector<std::string>> rows = {
        {"SMP-001", "Alpha"},
        {"SMP-002", "Beta"},
        {"SMP-003", "Gamma"},
    };

    const std::string table = View::ConsoleFormat::RenderTable(headers, rows);

    const size_t pos1 = table.find("SMP-001");
    const size_t pos2 = table.find("SMP-002");
    const size_t pos3 = table.find("SMP-003");

    ASSERT_NE(pos1, std::string::npos);
    ASSERT_NE(pos2, std::string::npos);
    ASSERT_NE(pos3, std::string::npos);
    EXPECT_LT(pos1, pos2);
    EXPECT_LT(pos2, pos3);

    EXPECT_NE(table.find("Alpha"), std::string::npos);
    EXPECT_NE(table.find("Beta"), std::string::npos);
    EXPECT_NE(table.find("Gamma"), std::string::npos);
}

// ---------------------------------------------------------------------------
// MakeProgressBar
// ---------------------------------------------------------------------------

TEST(ConsoleFormatTest, MakeProgressBar_ratio0이면_모두빈칸이고_0퍼센트가표시된다) {
    const std::string bar = View::ConsoleFormat::MakeProgressBar(0.0, 20);

    EXPECT_NE(bar.find(std::string(20, '-')), std::string::npos);
    EXPECT_NE(bar.find("0%"), std::string::npos);
}

TEST(ConsoleFormatTest, MakeProgressBar_ratio1이면_모두채워지고_100퍼센트가표시된다) {
    const std::string bar = View::ConsoleFormat::MakeProgressBar(1.0, 20);

    EXPECT_NE(bar.find(std::string(20, '#')), std::string::npos);
    EXPECT_NE(bar.find("100%"), std::string::npos);
}

TEST(ConsoleFormatTest, MakeProgressBar_ratio0_5면_절반씩채워지고_50퍼센트가표시된다) {
    const std::string bar = View::ConsoleFormat::MakeProgressBar(0.5, 20);

    EXPECT_NE(bar.find(std::string(10, '#') + std::string(10, '-')), std::string::npos);
    EXPECT_NE(bar.find("50%"), std::string::npos);
}

TEST(ConsoleFormatTest, MakeProgressBar_ratio음수면_0으로_클램프된다) {
    const std::string barNegative = View::ConsoleFormat::MakeProgressBar(-0.5, 20);
    const std::string barZero = View::ConsoleFormat::MakeProgressBar(0.0, 20);

    EXPECT_EQ(barNegative, barZero);
}

TEST(ConsoleFormatTest, MakeProgressBar_ratio1초과면_1로_클램프된다) {
    const std::string barOver = View::ConsoleFormat::MakeProgressBar(1.5, 20);
    const std::string barOne = View::ConsoleFormat::MakeProgressBar(1.0, 20);

    EXPECT_EQ(barOver, barOne);
}

// ---------------------------------------------------------------------------
// FormatThousands
// ---------------------------------------------------------------------------

TEST(ConsoleFormatTest, FormatThousands_0이면_0을반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatThousands(0), "0");
}

TEST(ConsoleFormatTest, FormatThousands_999이면_구분자없이999를반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatThousands(999), "999");
}

TEST(ConsoleFormatTest, FormatThousands_1000이면_1_000을반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatThousands(1000), "1,000");
}

TEST(ConsoleFormatTest, FormatThousands_1234567이면_1_234_567을반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatThousands(1234567), "1,234,567");
}

TEST(ConsoleFormatTest, FormatThousands_음수1234이면_마이너스1_234를반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatThousands(-1234), "-1,234");
}

// ---------------------------------------------------------------------------
// FormatOrderStatusBadge
// ---------------------------------------------------------------------------

TEST(ConsoleFormatTest, FormatOrderStatusBadge_RESERVED는_대괄호RESERVED를포함한다) {
    const std::string badge = View::ConsoleFormat::FormatOrderStatusBadge(Model::OrderStatus::RESERVED);
    EXPECT_NE(badge.find("[RESERVED]"), std::string::npos);
}

TEST(ConsoleFormatTest, FormatOrderStatusBadge_REJECTED는_대괄호REJECTED를포함한다) {
    const std::string badge = View::ConsoleFormat::FormatOrderStatusBadge(Model::OrderStatus::REJECTED);
    EXPECT_NE(badge.find("[REJECTED]"), std::string::npos);
}

TEST(ConsoleFormatTest, FormatOrderStatusBadge_PRODUCING은_대괄호PRODUCING을포함한다) {
    const std::string badge = View::ConsoleFormat::FormatOrderStatusBadge(Model::OrderStatus::PRODUCING);
    EXPECT_NE(badge.find("[PRODUCING]"), std::string::npos);
}

TEST(ConsoleFormatTest, FormatOrderStatusBadge_CONFIRMED는_대괄호CONFIRMED를포함한다) {
    const std::string badge = View::ConsoleFormat::FormatOrderStatusBadge(Model::OrderStatus::CONFIRMED);
    EXPECT_NE(badge.find("[CONFIRMED]"), std::string::npos);
}

TEST(ConsoleFormatTest, FormatOrderStatusBadge_RELEASE는_대괄호RELEASE를포함한다) {
    const std::string badge = View::ConsoleFormat::FormatOrderStatusBadge(Model::OrderStatus::RELEASE);
    EXPECT_NE(badge.find("[RELEASE]"), std::string::npos);
}

// ---------------------------------------------------------------------------
// FormatStockStatusBadge / FormatStockStatusKoreanLabel
// ---------------------------------------------------------------------------

TEST(ConsoleFormatTest, FormatStockStatusBadge_SUFFICIENT는_대괄호여유를포함한다) {
    const std::string badge = View::ConsoleFormat::FormatStockStatusBadge(Service::StockStatus::SUFFICIENT);
    EXPECT_NE(badge.find("[여유]"), std::string::npos);
}

TEST(ConsoleFormatTest, FormatStockStatusBadge_SHORTAGE는_대괄호부족을포함한다) {
    const std::string badge = View::ConsoleFormat::FormatStockStatusBadge(Service::StockStatus::SHORTAGE);
    EXPECT_NE(badge.find("[부족]"), std::string::npos);
}

TEST(ConsoleFormatTest, FormatStockStatusBadge_DEPLETED는_대괄호고갈을포함한다) {
    const std::string badge = View::ConsoleFormat::FormatStockStatusBadge(Service::StockStatus::DEPLETED);
    EXPECT_NE(badge.find("[고갈]"), std::string::npos);
}

TEST(ConsoleFormatTest, FormatStockStatusKoreanLabel_SUFFICIENT는_대괄호없이여유를반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatStockStatusKoreanLabel(Service::StockStatus::SUFFICIENT), "여유");
}

TEST(ConsoleFormatTest, FormatStockStatusKoreanLabel_SHORTAGE는_대괄호없이부족을반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatStockStatusKoreanLabel(Service::StockStatus::SHORTAGE), "부족");
}

TEST(ConsoleFormatTest, FormatStockStatusKoreanLabel_DEPLETED는_대괄호없이고갈을반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatStockStatusKoreanLabel(Service::StockStatus::DEPLETED), "고갈");
}

// ---------------------------------------------------------------------------
// Colorize
// ---------------------------------------------------------------------------

TEST(ConsoleFormatTest, Colorize_결과에_ANSI이스케이프와_원본텍스트가_모두포함된다) {
    const std::string colored = View::ConsoleFormat::Colorize("여유", View::ConsoleFormat::Color::Green);

    EXPECT_NE(colored.find(kAnsiEscapePrefix), std::string::npos);
    EXPECT_NE(colored.find("여유"), std::string::npos);
}

TEST(ConsoleFormatTest, Colorize_다른색상이면_다른ANSI코드가_적용된다) {
    const std::string red = View::ConsoleFormat::Colorize("텍스트", View::ConsoleFormat::Color::Red);
    const std::string green = View::ConsoleFormat::Colorize("텍스트", View::ConsoleFormat::Color::Green);

    EXPECT_NE(red, green);
}

// ---------------------------------------------------------------------------
// MakeLogoBanner
// ---------------------------------------------------------------------------

TEST(ConsoleFormatTest, MakeLogoBanner_S_SEMI로고와부제가_포함된다) {
    const std::string banner = View::ConsoleFormat::MakeLogoBanner();

    EXPECT_NE(banner.find("S-SEMI"), std::string::npos);
    EXPECT_NE(banner.find("반도체 시료 생산주문관리 시스템"), std::string::npos);
}

// ---------------------------------------------------------------------------
// FormatCurrentDateTime
// ---------------------------------------------------------------------------

TEST(ConsoleFormatTest, FormatCurrentDateTime_YYYY_MM_DD_HH_MM_SS패턴을_따른다) {
    const std::string formatted = View::ConsoleFormat::FormatCurrentDateTime();

    static const std::regex kPattern(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$)");
    EXPECT_TRUE(std::regex_match(formatted, kPattern)) << "actual: " << formatted;
}

// ---------------------------------------------------------------------------
// FormatTimePoint
// ---------------------------------------------------------------------------

TEST(ConsoleFormatTest, FormatTimePoint_길이19인_YYYY_MM_DD_HH_MM_SS형식을_반환한다) {
    const auto timePoint = std::chrono::system_clock::from_time_t(static_cast<std::time_t>(0));

    const std::string formatted = View::ConsoleFormat::FormatTimePoint(timePoint);

    static const std::regex kPattern(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$)");
    ASSERT_EQ(formatted.size(), 19u);
    EXPECT_TRUE(std::regex_match(formatted, kPattern)) << "actual: " << formatted;
}

// ---------------------------------------------------------------------------
// MakeColoredProgressBar
// ---------------------------------------------------------------------------

TEST(ConsoleFormatTest, MakeColoredProgressBar_ratio0_1이면_빨강계열ANSI코드를포함한다) {
    const std::string bar = View::ConsoleFormat::MakeColoredProgressBar(0.1, 20);

    EXPECT_NE(bar.find(kAnsiEscapePrefix), std::string::npos);
    EXPECT_NE(bar.find("10%"), std::string::npos);
}

TEST(ConsoleFormatTest, MakeColoredProgressBar_ratio0_5이면_노랑계열ANSI코드를포함한다) {
    const std::string bar = View::ConsoleFormat::MakeColoredProgressBar(0.5, 20);

    EXPECT_NE(bar.find(kAnsiEscapePrefix), std::string::npos);
    EXPECT_NE(bar.find("50%"), std::string::npos);
}

TEST(ConsoleFormatTest, MakeColoredProgressBar_ratio0_9이면_초록계열ANSI코드를포함한다) {
    const std::string bar = View::ConsoleFormat::MakeColoredProgressBar(0.9, 20);

    EXPECT_NE(bar.find(kAnsiEscapePrefix), std::string::npos);
    EXPECT_NE(bar.find("90%"), std::string::npos);
}

TEST(ConsoleFormatTest, MakeColoredProgressBar_세가지ratio에서_서로다른색상코드가적용된다) {
    const std::string barRed = View::ConsoleFormat::MakeColoredProgressBar(0.1, 20);
    const std::string barYellow = View::ConsoleFormat::MakeColoredProgressBar(0.5, 20);
    const std::string barGreen = View::ConsoleFormat::MakeColoredProgressBar(0.9, 20);

    EXPECT_NE(barRed, barYellow);
    EXPECT_NE(barYellow, barGreen);
    EXPECT_NE(barRed, barGreen);
}

TEST(ConsoleFormatTest, MakeColoredProgressBar_채워진칸과빈칸문자가_MakeProgressBar와동일한개수규칙을따른다) {
    const std::string bar = View::ConsoleFormat::MakeColoredProgressBar(0.5, 20);

    const size_t hashCount = std::count(bar.begin(), bar.end(), '#');
    const size_t dashCount = std::count(bar.begin(), bar.end(), '-');

    EXPECT_EQ(hashCount, 10u);
    EXPECT_EQ(dashCount, 10u);
}

}  // namespace
