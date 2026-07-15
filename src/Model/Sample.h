#pragma once

#include <string>

namespace Model {

// 반도체 시료(Sample) 엔티티.
// 재고/생산 계산에 필요한 기준 정보(평균 생산시간, 수율)와 현재 재고를 보관한다.
class Sample {
public:
    Sample(std::string sampleId, std::string name, double averageProductionTimeMinutes, double yield, int stock);

    const std::string& GetSampleId() const;
    const std::string& GetName() const;
    double GetAverageProductionTimeMinutes() const;
    double GetYield() const;
    int GetStock() const;

    // amount가 음수이면 std::invalid_argument를 던진다.
    void IncreaseStock(int amount);

private:
    std::string sampleId_;
    std::string name_;
    double averageProductionTimeMinutes_;
    double yield_;
    int stock_;
};

}  // namespace Model
