#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "../Model/ProductionJob.h"

namespace View {

// 생산 라인 현황(현재 작업/대기열) 화면을 출력하는 View.
class ProductionLineView {
public:
    explicit ProductionLineView(std::ostream& out = std::cout);

    void ShowCurrentJob(const Model::ProductionJob& job) const;
    void ShowNoCurrentJob() const;
    void ShowPendingQueue(const std::vector<Model::ProductionJob>& queue) const;
    void ShowCompletionResult(const std::string& orderNo) const;

private:
    std::ostream& out_;
};

}  // namespace View
