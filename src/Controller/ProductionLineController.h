#pragma once

#include <istream>
#include <ostream>

#include "../Service/ProductionLineService.h"
#include "../View/ProductionLineView.h"

namespace Controller {

// 생산 현황 메뉴(현재 작업/대기열 조회, 완료 처리)의 입출력 흐름을 담당하는 Controller.
class ProductionLineController {
public:
    ProductionLineController(std::istream& in, std::ostream& out, View::ProductionLineView& view,
                              Service::ProductionLineService& productionLineService);

    void Run();

private:
    void ShowSubMenu() const;
    void HandleComplete();

    std::istream& in_;
    std::ostream& out_;
    View::ProductionLineView& view_;
    Service::ProductionLineService& productionLineService_;
};

}  // namespace Controller
