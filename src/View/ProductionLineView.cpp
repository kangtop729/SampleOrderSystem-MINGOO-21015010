#include "ProductionLineView.h"

#include "ConsoleFormat.h"

namespace View {

ProductionLineView::ProductionLineView(std::ostream& out) : out_(out) {}

void ProductionLineView::ShowCurrentJob(const Model::ProductionJob& job) const {
    out_ << ConsoleFormat::MakeTitleBanner("생산 현황");
    out_ << "주문번호        : " << job.GetOrderNo() << "\n";
    out_ << "시료ID          : " << job.GetSampleId() << "\n";
    out_ << "고객명          : " << job.GetCustomerName() << "\n";
    out_ << "주문 수량       : " << job.GetQuantity() << "\n";
    out_ << "현재 재고       : " << job.GetCurrentStock() << "\n";
    out_ << "부족분          : " << job.GetShortfall() << "\n";
    out_ << "실 생산량       : " << job.GetActualProductionQty() << "\n";
    out_ << "총 생산 시간(분): " << job.GetTotalProductionMinutes() << "\n";
}

void ProductionLineView::ShowNoCurrentJob() const {
    out_ << "현재 진행 중인 생산 작업이 없습니다.\n";
}

void ProductionLineView::ShowPendingQueue(const std::vector<Model::ProductionJob>& queue) const {
    const std::vector<std::string> headers = {"주문번호", "시료ID", "고객명", "부족분", "실생산량"};
    std::vector<std::vector<std::string>> rows;
    rows.reserve(queue.size());
    for (const auto& job : queue) {
        rows.push_back({
            job.GetOrderNo(),
            job.GetSampleId(),
            job.GetCustomerName(),
            std::to_string(job.GetShortfall()),
            std::to_string(job.GetActualProductionQty()),
        });
    }
    out_ << ConsoleFormat::MakeTitleBanner("생산 대기열");
    out_ << ConsoleFormat::RenderTable(headers, rows);
}

void ProductionLineView::ShowCompletionResult(const std::string& orderNo) const {
    out_ << "주문 " << orderNo << "의 생산이 완료되었습니다.\n";
}

}  // namespace View
