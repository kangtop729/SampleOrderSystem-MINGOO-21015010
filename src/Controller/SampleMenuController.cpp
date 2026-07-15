#include "SampleMenuController.h"

#include "ConsoleInput.h"

namespace Controller {

namespace {
constexpr int kMenuRegister = 1;
constexpr int kMenuList = 2;
constexpr int kMenuSearch = 3;
constexpr int kMenuExit = 0;
}  // namespace

SampleMenuController::SampleMenuController(std::istream& in, std::ostream& out, View::SampleMenuView& view,
                                             Service::SampleService& sampleService)
    : in_(in), out_(out), view_(view), sampleService_(sampleService) {}

void SampleMenuController::Run() {
    while (true) {
        view_.ShowMenu();
        const auto choice = ConsoleInput::ReadInt(in_, out_, "");
        if (!choice.has_value()) {
            view_.ShowError("잘못된 선택입니다.");
            continue;
        }

        if (*choice == kMenuExit) {
            return;
        } else if (*choice == kMenuRegister) {
            HandleRegister();
        } else if (*choice == kMenuList) {
            HandleList();
        } else if (*choice == kMenuSearch) {
            HandleSearch();
        } else {
            view_.ShowError("잘못된 선택입니다.");
        }
    }
}

void SampleMenuController::HandleRegister() {
    const std::string sampleId = ConsoleInput::ReadLine(in_, out_, "시료ID > ");
    const std::string name = ConsoleInput::ReadLine(in_, out_, "이름 > ");
    const auto averageProductionTimeMinutes = ConsoleInput::ReadDouble(in_, out_, "평균 생산시간(분) > ");
    if (!averageProductionTimeMinutes.has_value()) {
        view_.ShowError("숫자를 입력해주세요.");
        return;
    }
    const auto yield = ConsoleInput::ReadDouble(in_, out_, "수율 > ");
    if (!yield.has_value()) {
        view_.ShowError("숫자를 입력해주세요.");
        return;
    }
    const auto initialStock = ConsoleInput::ReadInt(in_, out_, "초기재고 > ");
    if (!initialStock.has_value()) {
        view_.ShowError("숫자를 입력해주세요.");
        return;
    }

    try {
        sampleService_.RegisterSample(sampleId, name, *averageProductionTimeMinutes, *yield, *initialStock);
        const auto sample = sampleService_.FindOne(sampleId);
        if (sample) {
            view_.ShowRegisterSuccess(*sample);
        }
    } catch (const std::exception& e) {
        view_.ShowError(e.what());
    }
}

void SampleMenuController::HandleList() {
    view_.ShowSampleTable(sampleService_.GetAllSamples());
}

void SampleMenuController::HandleSearch() {
    const std::string keyword = ConsoleInput::ReadLine(in_, out_, "검색어 > ");
    view_.ShowSampleTable(sampleService_.SearchByName(keyword));
}

}  // namespace Controller
