#pragma once

#include <string>
#include <vector>

#include "../Model/OrderStatus.h"
#include "../Service/MonitoringService.h"

namespace View {
namespace ConsoleFormat {

std::string MakeDivider(int width = 60, char ch = '=');
std::string MakeTitleBanner(const std::string& title, int width = 60);
std::string RenderTable(const std::vector<std::string>& headers,
                         const std::vector<std::vector<std::string>>& rows);
std::string MakeProgressBar(double ratio, int width = 20);
std::string FormatThousands(long long value);
std::string FormatOrderStatusBadge(Model::OrderStatus status);
std::string FormatStockStatusBadge(Service::StockStatus status);
std::string FormatStockStatusKoreanLabel(Service::StockStatus status);
std::string FormatErrorMessage(const std::string& message);

}  // namespace ConsoleFormat
}  // namespace View
