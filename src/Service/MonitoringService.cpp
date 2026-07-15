#include "MonitoringService.h"

#include "../Model/OrderStatus.h"
#include "../Model/Sample.h"

namespace Service {

MonitoringService::MonitoringService(Repository::IOrderRepository& orderRepository,
                                      Repository::ISampleRepository& sampleRepository)
    : orderRepository_(orderRepository), sampleRepository_(sampleRepository) {}

StockStatus MonitoringService::ClassifyStockStatus(int stock, int pendingDemand) {
    if (stock == 0) {
        return StockStatus::DEPLETED;
    }
    if (stock >= pendingDemand) {
        return StockStatus::SUFFICIENT;
    }
    return StockStatus::SHORTAGE;
}

OrderStatusCounts MonitoringService::CountOrdersByStatus() const {
    OrderStatusCounts counts;
    counts.reserved = static_cast<int>(orderRepository_.FindByStatus(Model::OrderStatus::RESERVED).size());
    counts.confirmed = static_cast<int>(orderRepository_.FindByStatus(Model::OrderStatus::CONFIRMED).size());
    counts.producing = static_cast<int>(orderRepository_.FindByStatus(Model::OrderStatus::PRODUCING).size());
    counts.released = static_cast<int>(orderRepository_.FindByStatus(Model::OrderStatus::RELEASE).size());
    return counts;
}

int MonitoringService::CalculatePendingDemand(const std::string& sampleId) const {
    std::vector<Model::Order> orders = orderRepository_.FindBySampleId(sampleId);

    int pendingDemand = 0;
    for (const Model::Order& order : orders) {
        if (order.GetStatus() == Model::OrderStatus::RESERVED || order.GetStatus() == Model::OrderStatus::PRODUCING) {
            pendingDemand += order.GetQuantity();
        }
    }
    return pendingDemand;
}

std::vector<SampleStockStatus> MonitoringService::GetSampleStockStatuses() const {
    std::vector<Model::Sample> samples = sampleRepository_.FindAll();

    std::vector<SampleStockStatus> statuses;
    statuses.reserve(samples.size());
    for (const Model::Sample& sample : samples) {
        int pendingDemand = CalculatePendingDemand(sample.GetSampleId());
        StockStatus status = ClassifyStockStatus(sample.GetStock(), pendingDemand);

        statuses.push_back(SampleStockStatus{sample.GetSampleId(), sample.GetName(), sample.GetStock(), pendingDemand,
                                              status});
    }
    return statuses;
}

}  // namespace Service
