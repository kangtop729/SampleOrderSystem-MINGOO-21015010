#include "Sample.h"

#include <stdexcept>
#include <utility>

namespace Model {

Sample::Sample(std::string sampleId, std::string name, double averageProductionTimeMinutes, double yield, int stock)
    : sampleId_(std::move(sampleId)),
      name_(std::move(name)),
      averageProductionTimeMinutes_(averageProductionTimeMinutes),
      yield_(yield),
      stock_(stock) {}

const std::string& Sample::GetSampleId() const {
    return sampleId_;
}

const std::string& Sample::GetName() const {
    return name_;
}

double Sample::GetAverageProductionTimeMinutes() const {
    return averageProductionTimeMinutes_;
}

double Sample::GetYield() const {
    return yield_;
}

int Sample::GetStock() const {
    return stock_;
}

void Sample::IncreaseStock(int amount) {
    if (amount < 0) {
        throw std::invalid_argument("IncreaseStock: amount는 음수일 수 없습니다.");
    }
    stock_ += amount;
}

}  // namespace Model
