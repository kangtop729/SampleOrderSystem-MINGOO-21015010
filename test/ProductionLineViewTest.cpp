#include "gtest/gtest.h"

#include <chrono>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "../src/Model/ProductionJob.h"
#include "../src/View/ProductionLineView.h"

namespace {

TEST(ProductionLineViewTest, ShowCurrentJob_주문번호수량부족분생산량이_포함된다) {
    std::ostringstream oss;
    View::ProductionLineView view(oss);
    const Model::ProductionJob job("ORD-001", "SMP-001", "고객A", 10, 3, 7, 8, 40.0,
                                    std::optional<std::chrono::system_clock::time_point>(
                                        std::chrono::system_clock::now()));

    view.ShowCurrentJob(job);

    const std::string output = oss.str();
    EXPECT_NE(output.find("ORD-001"), std::string::npos);
    EXPECT_NE(output.find("10"), std::string::npos);
    EXPECT_NE(output.find("7"), std::string::npos);
    EXPECT_NE(output.find("8"), std::string::npos);
}

TEST(ProductionLineViewTest, ShowCurrentJob_생산시작전이면_대기중_메시지가_표시된다) {
    std::ostringstream oss;
    View::ProductionLineView view(oss);
    const Model::ProductionJob job("ORD-001", "SMP-001", "고객A", 10, 3, 7, 8, 40.0, std::nullopt);

    view.ShowCurrentJob(job);

    const std::string output = oss.str();
    EXPECT_NE(output.find("대기"), std::string::npos);
}

TEST(ProductionLineViewTest, ShowNoCurrentJob_출력이_비어있지않다) {
    std::ostringstream oss;
    View::ProductionLineView view(oss);

    view.ShowNoCurrentJob();

    EXPECT_FALSE(oss.str().empty());
}

TEST(ProductionLineViewTest, ShowPendingQueue_각job의주문번호가_포함된다) {
    std::ostringstream oss;
    View::ProductionLineView view(oss);
    const std::vector<Model::ProductionJob> queue = {
        Model::ProductionJob("ORD-001", "SMP-001", "고객A", 10, 3, 7, 8, 40.0,
                              std::optional<std::chrono::system_clock::time_point>(
                                  std::chrono::system_clock::now())),
        Model::ProductionJob("ORD-002", "SMP-002", "고객B", 5, 0, 5, 5, 25.0, std::nullopt),
    };

    view.ShowPendingQueue(queue);

    const std::string output = oss.str();
    EXPECT_NE(output.find("ORD-001"), std::string::npos);
    EXPECT_NE(output.find("ORD-002"), std::string::npos);
}

TEST(ProductionLineViewTest, ShowCompletionResult_주문번호가_포함된다) {
    std::ostringstream oss;
    View::ProductionLineView view(oss);

    view.ShowCompletionResult("ORD-001");

    EXPECT_NE(oss.str().find("ORD-001"), std::string::npos);
}

}  // namespace
