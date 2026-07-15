#include "gtest/gtest.h"

#include <string>
#include <vector>

#include "../src/Model/OrderStatus.h"
#include "../src/Service/MonitoringService.h"
#include "../src/View/ConsoleFormat.h"

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

TEST(ConsoleFormatTest, FormatOrderStatusBadge_RESERVED는_대괄호RESERVED를반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatOrderStatusBadge(Model::OrderStatus::RESERVED), "[RESERVED]");
}

TEST(ConsoleFormatTest, FormatOrderStatusBadge_REJECTED는_대괄호REJECTED를반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatOrderStatusBadge(Model::OrderStatus::REJECTED), "[REJECTED]");
}

TEST(ConsoleFormatTest, FormatOrderStatusBadge_PRODUCING은_대괄호PRODUCING을반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatOrderStatusBadge(Model::OrderStatus::PRODUCING), "[PRODUCING]");
}

TEST(ConsoleFormatTest, FormatOrderStatusBadge_CONFIRMED는_대괄호CONFIRMED를반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatOrderStatusBadge(Model::OrderStatus::CONFIRMED), "[CONFIRMED]");
}

TEST(ConsoleFormatTest, FormatOrderStatusBadge_RELEASE는_대괄호RELEASE를반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatOrderStatusBadge(Model::OrderStatus::RELEASE), "[RELEASE]");
}

// ---------------------------------------------------------------------------
// FormatStockStatusBadge / FormatStockStatusKoreanLabel
// ---------------------------------------------------------------------------

TEST(ConsoleFormatTest, FormatStockStatusBadge_SUFFICIENT는_대괄호여유를반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatStockStatusBadge(Service::StockStatus::SUFFICIENT), "[여유]");
}

TEST(ConsoleFormatTest, FormatStockStatusBadge_SHORTAGE는_대괄호부족을반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatStockStatusBadge(Service::StockStatus::SHORTAGE), "[부족]");
}

TEST(ConsoleFormatTest, FormatStockStatusBadge_DEPLETED는_대괄호고갈을반환한다) {
    EXPECT_EQ(View::ConsoleFormat::FormatStockStatusBadge(Service::StockStatus::DEPLETED), "[고갈]");
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

}  // namespace
