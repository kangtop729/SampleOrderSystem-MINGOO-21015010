#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "../src/Controller/SampleMenuController.h"
#include "../src/Model/Sample.h"
#include "../src/Service/SampleService.h"
#include "../src/View/SampleMenuView.h"
#include "mocks/MockSampleRepository.h"

using Model::Sample;
using Service::SampleService;
using Test::MockSampleRepository;
using ::testing::_;
using ::testing::Return;

class SampleMenuControllerTest : public ::testing::Test {
protected:
    MockSampleRepository repository_;
};

TEST_F(SampleMenuControllerTest, Run_등록메뉴에서_유효한값을입력하면_RegisterSample이_호출되고_등록성공메시지가출력된다) {
    EXPECT_CALL(repository_, FindById(std::string("SMP-001")))
        .WillOnce(Return(std::nullopt))
        .WillOnce(Return(Sample("SMP-001", "테스트시료", 30.0, 0.8, 10)));
    EXPECT_CALL(repository_, Save(_)).Times(1);

    std::istringstream in("1\nSMP-001\n테스트시료\n30\n0.8\n10\n0\n");
    std::ostringstream out;
    View::SampleMenuView view(out);
    SampleService service(repository_);
    Controller::SampleMenuController controller(in, out, view, service);

    controller.Run();

    EXPECT_NE(out.str().find("SMP-001"), std::string::npos);
}

TEST_F(SampleMenuControllerTest, Run_등록메뉴에서_숫자파싱실패시_에러메시지출력후_Save는호출되지않는다) {
    EXPECT_CALL(repository_, Save(_)).Times(0);

    std::istringstream in("1\nSMP-001\n테스트시료\nabc\n0.8\n10\n0\n");
    std::ostringstream out;
    View::SampleMenuView view(out);
    SampleService service(repository_);
    Controller::SampleMenuController controller(in, out, view, service);

    controller.Run();

    EXPECT_NE(out.str().find("숫자를 입력해주세요."), std::string::npos);
}

TEST_F(SampleMenuControllerTest, Run_목록메뉴선택시_GetAllSamples결과가_출력된다) {
    std::vector<Sample> samples{Sample("SMP-001", "웨이퍼A", 30.0, 0.8, 10)};
    EXPECT_CALL(repository_, FindAll()).WillOnce(Return(samples));

    std::istringstream in("2\n0\n");
    std::ostringstream out;
    View::SampleMenuView view(out);
    SampleService service(repository_);
    Controller::SampleMenuController controller(in, out, view, service);

    controller.Run();

    EXPECT_NE(out.str().find("SMP-001"), std::string::npos);
}

TEST_F(SampleMenuControllerTest, Run_검색메뉴선택시_keyword로_SearchByName이호출된다) {
    EXPECT_CALL(repository_, FindByName(std::string("웨이퍼"))).WillOnce(Return(std::vector<Sample>{}));

    std::istringstream in("3\n웨이퍼\n0\n");
    std::ostringstream out;
    View::SampleMenuView view(out);
    SampleService service(repository_);
    Controller::SampleMenuController controller(in, out, view, service);

    controller.Run();
}

TEST_F(SampleMenuControllerTest, Run_잘못된선택시_에러메시지출력후_루프가계속된다) {
    std::istringstream in("9\n0\n");
    std::ostringstream out;
    View::SampleMenuView view(out);
    SampleService service(repository_);
    Controller::SampleMenuController controller(in, out, view, service);

    controller.Run();

    EXPECT_NE(out.str().find("잘못된 선택입니다."), std::string::npos);
}

TEST_F(SampleMenuControllerTest, Run_0을입력하면_즉시반환한다) {
    EXPECT_CALL(repository_, FindAll()).Times(0);
    EXPECT_CALL(repository_, Save(_)).Times(0);

    std::istringstream in("0\n");
    std::ostringstream out;
    View::SampleMenuView view(out);
    SampleService service(repository_);
    Controller::SampleMenuController controller(in, out, view, service);

    controller.Run();
}
