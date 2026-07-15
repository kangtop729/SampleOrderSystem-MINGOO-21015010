#pragma once

#include <istream>
#include <ostream>

#include "../Service/SampleService.h"
#include "../View/SampleMenuView.h"

namespace Controller {

// 시료 관리 메뉴(등록/목록/검색)의 입출력 흐름을 담당하는 Controller.
class SampleMenuController {
public:
    SampleMenuController(std::istream& in, std::ostream& out, View::SampleMenuView& view,
                          Service::SampleService& sampleService);

    void Run();

private:
    void HandleRegister();
    void HandleList();
    void HandleSearch();

    std::istream& in_;
    std::ostream& out_;
    View::SampleMenuView& view_;
    Service::SampleService& sampleService_;
};

}  // namespace Controller
