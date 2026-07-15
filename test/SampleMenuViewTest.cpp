#include "gtest/gtest.h"

#include <sstream>
#include <string>
#include <vector>

#include "../src/Model/Sample.h"
#include "../src/View/SampleMenuView.h"

namespace {

TEST(SampleMenuViewTest, ShowMenu_메뉴항목들이_포함된다) {
    std::ostringstream oss;
    View::SampleMenuView view(oss);

    view.ShowMenu();

    const std::string output = oss.str();
    EXPECT_NE(output.find("[1] 시료 등록"), std::string::npos);
    EXPECT_NE(output.find("[2] 시료 목록"), std::string::npos);
    EXPECT_NE(output.find("[3] 시료 검색"), std::string::npos);
    EXPECT_NE(output.find("[0] 뒤로"), std::string::npos);
}

TEST(SampleMenuViewTest, ShowRegisterSuccess_등록된시료의ID가_포함된다) {
    std::ostringstream oss;
    View::SampleMenuView view(oss);
    const Model::Sample sample("SMP-001", "Alpha", 10.0, 0.9, 100);

    view.ShowRegisterSuccess(sample);

    EXPECT_NE(oss.str().find("SMP-001"), std::string::npos);
}

TEST(SampleMenuViewTest, ShowSampleTable_각샘플의ID와이름이_포함된다) {
    std::ostringstream oss;
    View::SampleMenuView view(oss);
    const std::vector<Model::Sample> samples = {
        Model::Sample("SMP-001", "Alpha", 10.0, 0.9, 100),
        Model::Sample("SMP-002", "Beta", 5.0, 0.8, 50),
    };

    view.ShowSampleTable(samples);

    const std::string output = oss.str();
    EXPECT_NE(output.find("SMP-001"), std::string::npos);
    EXPECT_NE(output.find("Alpha"), std::string::npos);
    EXPECT_NE(output.find("SMP-002"), std::string::npos);
    EXPECT_NE(output.find("Beta"), std::string::npos);
}

TEST(SampleMenuViewTest, ShowSampleTable_헤더에_요구된컬럼명들이_포함된다) {
    std::ostringstream oss;
    View::SampleMenuView view(oss);
    const std::vector<Model::Sample> samples = {
        Model::Sample("SMP-001", "Alpha", 10.0, 0.9, 100),
    };

    view.ShowSampleTable(samples);

    const std::string output = oss.str();
    EXPECT_NE(output.find("ID"), std::string::npos);
    EXPECT_NE(output.find("시료명"), std::string::npos);
    EXPECT_NE(output.find("평균생산시간"), std::string::npos);
    EXPECT_NE(output.find("수율"), std::string::npos);
    EXPECT_NE(output.find("현재 재고"), std::string::npos);
}

TEST(SampleMenuViewTest, ShowError_메시지가_그대로포함된다) {
    std::ostringstream oss;
    View::SampleMenuView view(oss);

    view.ShowError("시료를 찾을 수 없습니다.");

    EXPECT_NE(oss.str().find("시료를 찾을 수 없습니다."), std::string::npos);
}

}  // namespace
