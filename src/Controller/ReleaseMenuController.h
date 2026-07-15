#pragma once

#include <istream>
#include <ostream>

#include "../Service/ReleaseService.h"
#include "../View/ReleaseMenuView.h"

namespace Controller {

// 출고 처리 메뉴의 입출력 흐름을 담당하는 Controller.
class ReleaseMenuController {
public:
    ReleaseMenuController(std::istream& in, std::ostream& out, View::ReleaseMenuView& view,
                           Service::ReleaseService& releaseService);

    void Run();

private:
    std::istream& in_;
    std::ostream& out_;
    View::ReleaseMenuView& view_;
    Service::ReleaseService& releaseService_;
};

}  // namespace Controller
