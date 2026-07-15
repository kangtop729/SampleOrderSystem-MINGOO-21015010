#include "MonitoringView.h"

#include "ConsoleFormat.h"

namespace View {

MonitoringView::MonitoringView(std::ostream& out) : out_(out) {}

void MonitoringView::ShowOrderCounts(const Service::OrderStatusCounts& counts) const {
    out_ << ConsoleFormat::MakeTitleBanner("주문 현황");
    out_ << "RESERVED  : " << counts.reserved << "\n";
    out_ << "CONFIRMED : " << counts.confirmed << "\n";
    out_ << "PRODUCING : " << counts.producing << "\n";
    out_ << "RELEASED  : " << counts.released << "\n";
}

void MonitoringView::ShowStockStatuses(const std::vector<Service::SampleStockStatus>& statuses) const {
    const std::vector<std::string> headers = {"시료ID", "이름", "재고", "미해결수요", "상태"};
    std::vector<std::vector<std::string>> rows;
    rows.reserve(statuses.size());
    for (const auto& status : statuses) {
        rows.push_back({
            status.sampleId,
            status.name,
            std::to_string(status.stock),
            std::to_string(status.pendingDemand),
            ConsoleFormat::FormatStockStatusKoreanLabel(status.status),
        });
    }
    out_ << ConsoleFormat::MakeTitleBanner("재고 현황");
    out_ << ConsoleFormat::RenderTable(headers, rows);
}

}  // namespace View
