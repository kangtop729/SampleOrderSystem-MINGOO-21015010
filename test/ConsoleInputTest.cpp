#include "gtest/gtest.h"

#include <optional>
#include <sstream>
#include <string>

#include "../src/Controller/ConsoleInput.h"

namespace {

TEST(ConsoleInputTest, ReadLine_prompt를출력하고_입력된한줄을그대로반환한다) {
    std::istringstream in("안녕하세요\n");
    std::ostringstream out;

    std::string result = Controller::ConsoleInput::ReadLine(in, out, "이름 > ");

    EXPECT_EQ(result, "안녕하세요");
    EXPECT_NE(out.str().find("이름 > "), std::string::npos);
}

TEST(ConsoleInputTest, ReadInt_숫자만입력하면_해당값을반환한다) {
    std::istringstream in("42\n");
    std::ostringstream out;

    std::optional<int> result = Controller::ConsoleInput::ReadInt(in, out, "선택 > ");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 42);
}

TEST(ConsoleInputTest, ReadInt_숫자가아니면_nullopt를반환한다) {
    std::istringstream in("abc\n");
    std::ostringstream out;

    std::optional<int> result = Controller::ConsoleInput::ReadInt(in, out, "선택 > ");

    EXPECT_FALSE(result.has_value());
}

TEST(ConsoleInputTest, ReadInt_뒤에잔여문자가있으면_nullopt를반환한다) {
    std::istringstream in("12abc\n");
    std::ostringstream out;

    std::optional<int> result = Controller::ConsoleInput::ReadInt(in, out, "선택 > ");

    EXPECT_FALSE(result.has_value());
}

TEST(ConsoleInputTest, ReadDouble_숫자만입력하면_해당값을반환한다) {
    std::istringstream in("3.14\n");
    std::ostringstream out;

    std::optional<double> result = Controller::ConsoleInput::ReadDouble(in, out, "값 > ");

    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(*result, 3.14);
}

TEST(ConsoleInputTest, ReadDouble_숫자가아니면_nullopt를반환한다) {
    std::istringstream in("xyz\n");
    std::ostringstream out;

    std::optional<double> result = Controller::ConsoleInput::ReadDouble(in, out, "값 > ");

    EXPECT_FALSE(result.has_value());
}

}  // namespace
