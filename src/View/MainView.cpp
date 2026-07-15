#include "MainView.h"

#include "ConsoleFormat.h"

namespace View {

namespace {
constexpr int kBannerWidth = 60;
}  // namespace

MainView::MainView(std::ostream& out) : out_(out) {}

void MainView::ShowMainMenu(const SystemStatusSummary& summary) const {
    out_ << ConsoleFormat::MakeTitleBanner("반도체 시료 생산주문관리 시스템", kBannerWidth);
    out_ << "등록 시료 수      : " << summary.sampleCount << "\n";
    out_ << "총 재고           : " << ConsoleFormat::FormatThousands(summary.totalStock) << "\n";
    out_ << "총 주문 수        : " << summary.totalOrderCount << "\n";
    out_ << "생산 대기 건수    : " << summary.pendingProductionCount << "\n";
    out_ << ConsoleFormat::MakeDivider(kBannerWidth) << "\n";
    out_ << "[1] 시료 관리\n";
    out_ << "[2] 주문 접수\n";
    out_ << "[3] 승인 처리\n";
    out_ << "[4] 생산 현황\n";
    out_ << "[5] 모니터링\n";
    out_ << "[6] 출고 처리\n";
    out_ << "[0] 종료\n";
    out_ << "선택 > ";
}

void MainView::ShowInvalidSelectionMessage() const {
    out_ << "잘못된 선택입니다. 다시 입력해주세요.\n";
}

void MainView::ShowExitMessage() const {
    out_ << "시스템을 종료합니다.\n";
}

}  // namespace View
