#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "../Model/Sample.h"

namespace View {

// 시료 관리 메뉴(등록/목록/검색) 화면을 출력하는 View.
class SampleMenuView {
public:
    explicit SampleMenuView(std::ostream& out = std::cout);

    void ShowMenu() const;
    void ShowRegisterSuccess(const Model::Sample& sample) const;
    void ShowSampleTable(const std::vector<Model::Sample>& samples) const;
    void ShowError(const std::string& message) const;

private:
    std::ostream& out_;
};

}  // namespace View
