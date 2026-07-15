#include "ConsoleFormat.h"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace View {
namespace ConsoleFormat {

namespace {

constexpr const char* kAnsiReset = "\x1b[0m";

const char* ColorToAnsiCode(Color color) {
    switch (color) {
        case Color::Red:
            return "\x1b[31m";
        case Color::Green:
            return "\x1b[32m";
        case Color::Yellow:
            return "\x1b[33m";
        case Color::Blue:
            return "\x1b[34m";
        case Color::Magenta:
            return "\x1b[35m";
        case Color::Cyan:
            return "\x1b[36m";
        case Color::BoldWhite:
            return "\x1b[1;37m";
        case Color::Default:
        default:
            return "";
    }
}

Color OrderStatusToColor(Model::OrderStatus status) {
    switch (status) {
        case Model::OrderStatus::RESERVED:
            return Color::Yellow;
        case Model::OrderStatus::CONFIRMED:
            return Color::Green;
        case Model::OrderStatus::PRODUCING:
            return Color::Cyan;
        case Model::OrderStatus::RELEASE:
            return Color::Magenta;
        case Model::OrderStatus::REJECTED:
            return Color::Red;
    }
    return Color::Default;
}

Color StockStatusToColor(Service::StockStatus status) {
    switch (status) {
        case Service::StockStatus::SUFFICIENT:
            return Color::Green;
        case Service::StockStatus::SHORTAGE:
            return Color::Yellow;
        case Service::StockStatus::DEPLETED:
            return Color::Red;
    }
    return Color::Default;
}

std::string FormatTimeStruct(const std::tm& timeInfo) {
    std::ostringstream oss;
    oss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

int Utf8DisplayLength(const std::string& text) {
    int length = 0;
    for (size_t i = 0; i < text.size();) {
        const unsigned char firstByte = static_cast<unsigned char>(text[i]);
        size_t charBytes = 1;
        if ((firstByte & 0x80) == 0x00) {
            charBytes = 1;
        } else if ((firstByte & 0xE0) == 0xC0) {
            charBytes = 2;
        } else if ((firstByte & 0xF0) == 0xE0) {
            charBytes = 3;
        } else if ((firstByte & 0xF8) == 0xF0) {
            charBytes = 4;
        }
        i += charBytes;
        length += (charBytes >= 3) ? 2 : 1;  // 한글 등 다바이트 문자는 폭 2로 취급
    }
    return length;
}

std::string PadToWidth(const std::string& text, int width) {
    const int displayLength = Utf8DisplayLength(text);
    const int padding = width - displayLength;
    if (padding <= 0) {
        return text;
    }
    return text + std::string(padding, ' ');
}

std::string OrderStatusToName(Model::OrderStatus status) {
    switch (status) {
        case Model::OrderStatus::RESERVED:
            return "RESERVED";
        case Model::OrderStatus::REJECTED:
            return "REJECTED";
        case Model::OrderStatus::PRODUCING:
            return "PRODUCING";
        case Model::OrderStatus::CONFIRMED:
            return "CONFIRMED";
        case Model::OrderStatus::RELEASE:
            return "RELEASE";
    }
    return "";
}

std::string StockStatusToKoreanLabel(Service::StockStatus status) {
    switch (status) {
        case Service::StockStatus::SUFFICIENT:
            return "여유";
        case Service::StockStatus::SHORTAGE:
            return "부족";
        case Service::StockStatus::DEPLETED:
            return "고갈";
    }
    return "";
}

}  // namespace

std::string MakeDivider(int width, char ch) {
    return std::string(static_cast<size_t>(width), ch);
}

std::string MakeTitleBanner(const std::string& title, int width) {
    const std::string divider = MakeDivider(width, '=');
    std::ostringstream oss;
    oss << divider << "\n" << title << "\n" << divider << "\n";
    return oss.str();
}

std::string RenderTable(const std::vector<std::string>& headers,
                         const std::vector<std::vector<std::string>>& rows) {
    const size_t columnCount = headers.size();
    std::vector<int> columnWidths(columnCount, 0);

    for (size_t col = 0; col < columnCount; ++col) {
        columnWidths[col] = Utf8DisplayLength(headers[col]);
    }
    for (const auto& row : rows) {
        for (size_t col = 0; col < columnCount && col < row.size(); ++col) {
            columnWidths[col] = std::max(columnWidths[col], Utf8DisplayLength(row[col]));
        }
    }

    constexpr int kColumnGap = 2;
    std::ostringstream oss;

    for (size_t col = 0; col < columnCount; ++col) {
        oss << PadToWidth(headers[col], columnWidths[col]);
        if (col + 1 < columnCount) {
            oss << std::string(kColumnGap, ' ');
        }
    }
    oss << "\n";

    for (const auto& row : rows) {
        for (size_t col = 0; col < columnCount; ++col) {
            const std::string cell = (col < row.size()) ? row[col] : "";
            oss << PadToWidth(cell, columnWidths[col]);
            if (col + 1 < columnCount) {
                oss << std::string(kColumnGap, ' ');
            }
        }
        oss << "\n";
    }

    return oss.str();
}

std::string MakeProgressBar(double ratio, int width) {
    const double clampedRatio = std::min(1.0, std::max(0.0, ratio));
    const int filled = static_cast<int>(std::lround(clampedRatio * width));
    const int empty = width - filled;
    const int percent = static_cast<int>(std::lround(clampedRatio * 100.0));

    std::ostringstream oss;
    oss << "[" << std::string(static_cast<size_t>(filled), '#')
        << std::string(static_cast<size_t>(empty), '-') << "] " << percent << "%";
    return oss.str();
}

std::string FormatThousands(long long value) {
    const bool isNegative = value < 0;
    unsigned long long absValue = isNegative ? static_cast<unsigned long long>(-value)
                                              : static_cast<unsigned long long>(value);

    std::string digits = std::to_string(absValue);
    std::string result;
    int countSinceComma = 0;

    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        if (countSinceComma != 0 && countSinceComma % 3 == 0) {
            result.push_back(',');
        }
        result.push_back(*it);
        ++countSinceComma;
    }

    std::reverse(result.begin(), result.end());
    return isNegative ? ("-" + result) : result;
}

std::string FormatOrderStatusBadge(Model::OrderStatus status) {
    return Colorize("[" + OrderStatusToName(status) + "]", OrderStatusToColor(status));
}

std::string FormatStockStatusBadge(Service::StockStatus status) {
    return Colorize("[" + StockStatusToKoreanLabel(status) + "]", StockStatusToColor(status));
}

std::string FormatStockStatusKoreanLabel(Service::StockStatus status) {
    return StockStatusToKoreanLabel(status);
}

std::string FormatErrorMessage(const std::string& message) {
    return "[오류] " + message + "\n";
}

std::string Colorize(const std::string& text, Color color) {
    if (color == Color::Default) {
        return text;
    }
    return std::string(ColorToAnsiCode(color)) + text + kAnsiReset;
}

std::string MakeLogoBanner() {
    std::ostringstream oss;
    oss << MakeDivider(60, '=') << "\n";
    oss << Colorize("S-SEMI", Color::BoldWhite) << "\n";
    oss << Colorize("반도체 시료 생산주문관리 시스템", Color::Cyan) << "\n";
    oss << MakeDivider(60, '=') << "\n";
    return oss.str();
}

std::string FormatCurrentDateTime() {
    const std::time_t now = std::time(nullptr);
    std::tm timeInfo{};
    localtime_s(&timeInfo, &now);
    return FormatTimeStruct(timeInfo);
}

std::string FormatTimePoint(std::chrono::system_clock::time_point tp) {
    const std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm timeInfo{};
    localtime_s(&timeInfo, &time);
    return FormatTimeStruct(timeInfo);
}

std::string MakeColoredProgressBar(double ratio, int width) {
    const double clampedRatio = std::min(1.0, std::max(0.0, ratio));
    constexpr double kLowThreshold = 0.3;
    constexpr double kHighThreshold = 0.7;

    Color color = Color::Green;
    if (clampedRatio < kLowThreshold) {
        color = Color::Red;
    } else if (clampedRatio < kHighThreshold) {
        color = Color::Yellow;
    }

    return Colorize(MakeProgressBar(clampedRatio, width), color);
}

}  // namespace ConsoleFormat
}  // namespace View
