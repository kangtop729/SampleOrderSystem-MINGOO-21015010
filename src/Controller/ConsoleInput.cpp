#include "ConsoleInput.h"

#include <exception>

namespace Controller {
namespace ConsoleInput {

std::string ReadLine(std::istream& in, std::ostream& out, const std::string& prompt) {
    out << prompt;
    std::string line;
    std::getline(in, line);
    return line;
}

std::optional<int> ReadInt(std::istream& in, std::ostream& out, const std::string& prompt) {
    const std::string line = ReadLine(in, out, prompt);
    try {
        size_t consumed = 0;
        const int value = std::stoi(line, &consumed);
        if (consumed != line.size()) {
            return std::nullopt;
        }
        return value;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<double> ReadDouble(std::istream& in, std::ostream& out, const std::string& prompt) {
    const std::string line = ReadLine(in, out, prompt);
    try {
        size_t consumed = 0;
        const double value = std::stod(line, &consumed);
        if (consumed != line.size()) {
            return std::nullopt;
        }
        return value;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

}  // namespace ConsoleInput
}  // namespace Controller
