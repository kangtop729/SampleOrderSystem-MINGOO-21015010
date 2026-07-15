#pragma once

#include <iostream>
#include <ostream>

namespace View {

// 메인 화면에 표시할 시스템 현황 요약.
struct SystemStatusSummary {
    int sampleCount;
    long long totalStock;
    int totalOrderCount;
    int pendingProductionCount;
};

// 메인 메뉴 화면(시스템 현황 요약 + 메뉴 목록)을 출력하는 View.
class MainView {
public:
    explicit MainView(std::ostream& out = std::cout);

    void ShowMainMenu(const SystemStatusSummary& summary) const;
    void ShowInvalidSelectionMessage() const;
    void ShowExitMessage() const;

private:
    std::ostream& out_;
};

}  // namespace View

// 다른 계층(View::)과의 네임스페이스 컨벤션은 유지하되, 기존 사용처와의 호환을 위해
// 전역 네임스페이스에서도 동일한 타입을 참조할 수 있도록 별칭을 둔다.
using View::SystemStatusSummary;
