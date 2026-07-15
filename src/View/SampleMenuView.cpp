#include "SampleMenuView.h"

#include "ConsoleFormat.h"

namespace View {

SampleMenuView::SampleMenuView(std::ostream& out) : out_(out) {}

void SampleMenuView::ShowMenu() const {
    out_ << ConsoleFormat::MakeTitleBanner("시료 관리");
    out_ << "[1] 시료 등록\n";
    out_ << "[2] 시료 목록\n";
    out_ << "[3] 시료 검색\n";
    out_ << "[0] 뒤로\n";
    out_ << "선택 > ";
}

void SampleMenuView::ShowRegisterSuccess(const Model::Sample& sample) const {
    out_ << "시료가 등록되었습니다. (ID: " << sample.GetSampleId() << ", 이름: " << sample.GetName() << ")\n";
}

void SampleMenuView::ShowSampleTable(const std::vector<Model::Sample>& samples) const {
    const std::vector<std::string> headers = {"시료ID", "이름", "평균생산시간(분)", "수율", "재고"};
    std::vector<std::vector<std::string>> rows;
    rows.reserve(samples.size());
    for (const auto& sample : samples) {
        rows.push_back({
            sample.GetSampleId(),
            sample.GetName(),
            std::to_string(sample.GetAverageProductionTimeMinutes()),
            std::to_string(sample.GetYield()),
            std::to_string(sample.GetStock()),
        });
    }
    out_ << ConsoleFormat::RenderTable(headers, rows);
}

void SampleMenuView::ShowError(const std::string& message) const {
    out_ << ConsoleFormat::FormatErrorMessage(message);
}

}  // namespace View
