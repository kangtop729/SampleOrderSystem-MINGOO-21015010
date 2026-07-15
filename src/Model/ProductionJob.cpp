#include "ProductionJob.h"

namespace Model {

ProductionJob::ProductionJob(std::string orderNo, std::string sampleId, std::string customerName, int quantity,
                              int currentStock, int shortfall, int actualProductionQty, double totalProductionMinutes)
    : orderNo_(std::move(orderNo)),
      sampleId_(std::move(sampleId)),
      customerName_(std::move(customerName)),
      quantity_(quantity),
      currentStock_(currentStock),
      shortfall_(shortfall),
      actualProductionQty_(actualProductionQty),
      totalProductionMinutes_(totalProductionMinutes) {}

const std::string& ProductionJob::GetOrderNo() const {
    return orderNo_;
}

const std::string& ProductionJob::GetSampleId() const {
    return sampleId_;
}

const std::string& ProductionJob::GetCustomerName() const {
    return customerName_;
}

int ProductionJob::GetQuantity() const {
    return quantity_;
}

int ProductionJob::GetCurrentStock() const {
    return currentStock_;
}

int ProductionJob::GetShortfall() const {
    return shortfall_;
}

int ProductionJob::GetActualProductionQty() const {
    return actualProductionQty_;
}

double ProductionJob::GetTotalProductionMinutes() const {
    return totalProductionMinutes_;
}

}  // namespace Model
