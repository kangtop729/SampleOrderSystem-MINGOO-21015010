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

ProductionLineService::OrderProductionCalculation ProductionLineService::CalculateForOrder(
    const Model::Order& order) const {
    auto sample = sampleRepository_.FindById(order.GetSampleId());
    if (!sample.has_value()) {
        throw std::logic_error("존재하지 않는 sampleId입니다: " + order.GetSampleId());
    }

    int shortfall = CalculateShortfall(order.GetQuantity(), sample->GetStock());
    int actualProductionQty = CalculateActualProductionQty(shortfall, sample->GetYield());

    return OrderProductionCalculation{sample.value(), shortfall, actualProductionQty};
}

Model::ProductionJob ProductionLineService::BuildProductionJob(const Model::Order& order, bool isFront) const {
    OrderProductionCalculation calculation = CalculateForOrder(order);
    double totalProductionMinutes = CalculateTotalProductionMinutes(
        calculation.sample.GetAverageProductionTimeMinutes(), calculation.actualProductionQty);

    // 단일 생산 라인에서는 대기열의 맨 앞 작업만 실제로 "생산 중"이다. 맨 앞 작업의 생산 시작 시각이
    // 아직 영속화되지 않았다면(AutoCompleteFinishedJobs가 아직 시작 시각을 기록하기 전이라면) 화면
    // 표시 목적으로만 "지금"을 임시로 사용한다(GetQueue는 const이므로 Repository에 쓰지 않는다).
    std::optional<std::chrono::system_clock::time_point> productionStartedAt = std::nullopt;
    if (isFront) {
        productionStartedAt = order.GetProductionStartedAt().has_value() ? order.GetProductionStartedAt()
                                                                          : std::chrono::system_clock::now();
    }

    return Model::ProductionJob(order.GetOrderNo(), order.GetSampleId(), order.GetCustomerName(), order.GetQuantity(),
                                 calculation.sample.GetStock(), calculation.shortfall, calculation.actualProductionQty,
                                 totalProductionMinutes, productionStartedAt);
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
    for (size_t i = 0; i < producingOrders.size(); ++i) {
        queue.push_back(BuildProductionJob(producingOrders[i], i == 0));
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

    OrderProductionCalculation calculation = CalculateForOrder(order);

    Model::Sample sample = calculation.sample;
    sample.IncreaseStock(calculation.actualProductionQty);
    sampleRepository_.Update(sample);

    order.CompleteProduction();
    orderRepository_.Update(order);
}

int ProductionLineService::AutoCompleteFinishedJobs() {
    int completedCount = 0;

    while (true) {
        std::vector<Model::Order> producingOrders = GetSortedProducingOrders();
        if (producingOrders.empty()) {
            break;
        }

        Model::Order front = producingOrders.front();

        if (!front.GetProductionStartedAt().has_value()) {
            // 앞 작업이 아직 시작되지 않았다면(이전 작업이 막 끝났거나 최초 진입) 지금 시작 시각을
            // 기록하고 영속화한 뒤, 이번 호출에서는 더 이상 진행하지 않는다(막 시작했으니 당연히 미완료).
            front.MarkProductionStarted(std::chrono::system_clock::now());
            orderRepository_.Update(front);
            break;
        }

        auto sampleOpt = sampleRepository_.FindById(front.GetSampleId());
        if (!sampleOpt.has_value()) {
            break;  // 방어적, 이론상 발생하지 않음
        }

        int shortfall = CalculateShortfall(front.GetQuantity(), sampleOpt->GetStock());
        int actualQty = CalculateActualProductionQty(shortfall, sampleOpt->GetYield());
        double totalMinutes =
            CalculateTotalProductionMinutes(sampleOpt->GetAverageProductionTimeMinutes(), actualQty);

        const auto elapsed = std::chrono::system_clock::now() - *front.GetProductionStartedAt();
        const double elapsedMinutes = std::chrono::duration<double, std::ratio<60>>(elapsed).count();

        if (elapsedMinutes < totalMinutes) {
            break;  // 아직 완료 안 됨, 더 처리할 것 없음
        }

        Model::Sample sample = *sampleOpt;
        sample.IncreaseStock(actualQty);
        sampleRepository_.Update(sample);

        front.CompleteProduction();
        orderRepository_.Update(front);
        ++completedCount;
        // 다음 대기 주문이 새 front가 되어 다음 반복에서 "시작 안 됨" 처리로 진입한다.
    }

    return completedCount;
}

}  // namespace Service
