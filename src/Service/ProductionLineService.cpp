#include "ProductionLineService.h"

#include <algorithm>
#include <cmath>

#include "../Model/Order.h"
#include "../Model/OrderStatus.h"
#include "../Model/Sample.h"

namespace Service {

namespace {

// CalculateActualProductionQty의 ceil 계산에서 부동소수점 오차를 보정하기 위한 여유값.
constexpr double kCeilEpsilon = 1e-9;

}  // namespace

NoProductionJobException::NoProductionJobException(const std::string& message) : std::logic_error(message) {}

ProductionLineService::ProductionLineService(Repository::IOrderRepository& orderRepository,
                                              Repository::ISampleRepository& sampleRepository)
    : orderRepository_(orderRepository), sampleRepository_(sampleRepository) {}

int ProductionLineService::CalculateShortfall(int quantity, int currentStock) {
    return std::max(0, quantity - currentStock);
}

int ProductionLineService::CalculateActualProductionQty(int shortfall, double yield) {
    if (shortfall <= 0) {
        return 0;
    }
    return static_cast<int>(std::ceil(shortfall / yield - kCeilEpsilon));
}

double ProductionLineService::CalculateTotalProductionMinutes(double averageProductionTimeMinutes,
                                                                int actualProductionQty) {
    return averageProductionTimeMinutes * actualProductionQty;
}

Model::ProductionJob ProductionLineService::BuildProductionJob(const Model::Order& order) const {
    auto sample = sampleRepository_.FindById(order.GetSampleId());
    if (!sample.has_value()) {
        throw std::logic_error("존재하지 않는 sampleId입니다: " + order.GetSampleId());
    }

    int shortfall = CalculateShortfall(order.GetQuantity(), sample->GetStock());
    int actualProductionQty = CalculateActualProductionQty(shortfall, sample->GetYield());
    double totalProductionMinutes =
        CalculateTotalProductionMinutes(sample->GetAverageProductionTimeMinutes(), actualProductionQty);

    return Model::ProductionJob(order.GetOrderNo(), order.GetSampleId(), order.GetCustomerName(), order.GetQuantity(),
                                 sample->GetStock(), shortfall, actualProductionQty, totalProductionMinutes);
}

std::vector<Model::Order> ProductionLineService::GetSortedProducingOrders() const {
    std::vector<Model::Order> producingOrders = orderRepository_.FindByStatus(Model::OrderStatus::PRODUCING);

    std::stable_sort(producingOrders.begin(), producingOrders.end(), [](const Model::Order& lhs, const Model::Order& rhs) {
        return lhs.GetCreatedAt() < rhs.GetCreatedAt();
    });

    return producingOrders;
}

std::vector<Model::ProductionJob> ProductionLineService::GetQueue() const {
    std::vector<Model::Order> producingOrders = GetSortedProducingOrders();

    std::vector<Model::ProductionJob> queue;
    queue.reserve(producingOrders.size());
    for (const Model::Order& order : producingOrders) {
        queue.push_back(BuildProductionJob(order));
    }
    return queue;
}

Model::ProductionJob ProductionLineService::GetCurrentJob() const {
    std::vector<Model::ProductionJob> queue = GetQueue();
    if (queue.empty()) {
        throw NoProductionJobException("생산 대기 중인 주문이 없습니다.");
    }
    return queue.front();
}

void ProductionLineService::CompleteCurrentJob() {
    std::vector<Model::Order> producingOrders = GetSortedProducingOrders();
    if (producingOrders.empty()) {
        throw NoProductionJobException("생산 대기 중인 주문이 없습니다.");
    }

    Model::Order order = producingOrders.front();

    auto sampleOpt = sampleRepository_.FindById(order.GetSampleId());
    if (!sampleOpt.has_value()) {
        throw std::logic_error("존재하지 않는 sampleId입니다: " + order.GetSampleId());
    }
    Model::Sample sample = sampleOpt.value();

    int shortfall = CalculateShortfall(order.GetQuantity(), sample.GetStock());
    int actualProductionQty = CalculateActualProductionQty(shortfall, sample.GetYield());

    sample.IncreaseStock(actualProductionQty);
    sampleRepository_.Update(sample);

    order.CompleteProduction();
    orderRepository_.Update(order);
}

}  // namespace Service
