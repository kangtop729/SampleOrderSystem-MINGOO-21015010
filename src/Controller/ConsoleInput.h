#pragma once

#include <istream>
#include <optional>
#include <ostream>
#include <string>

namespace Controller {
namespace ConsoleInput {

// prompt를 out에 출력한 뒤, in으로부터 한 줄을 읽어 그대로 반환한다.
std::string ReadLine(std::istream& in, std::ostream& out, const std::string& prompt);

// prompt를 출력하고 한 줄을 읽어 정수로 파싱한다.
// 잔여 문자가 있거나 파싱에 실패하면 std::nullopt를 반환한다.
std::optional<int> ReadInt(std::istream& in, std::ostream& out, const std::string& prompt);

// prompt를 출력하고 한 줄을 읽어 실수로 파싱한다.
// 잔여 문자가 있거나 파싱에 실패하면 std::nullopt를 반환한다.
std::optional<double> ReadDouble(std::istream& in, std::ostream& out, const std::string& prompt);

}  // namespace ConsoleInput
}  // namespace Controller
