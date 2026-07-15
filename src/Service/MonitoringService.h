#pragma once

#include <string>
#include <vector>

#include "../Model/Order.h"
#include "../Repository/IOrderRepository.h"
#include "../Repository/ISampleRepository.h"

namespace Service {

// 상태별 주문 개수. REJECTED는 모니터링 대상에서 제외되므로 포함하지 않는다.
struct OrderStatusCounts {
    int reserved = 0;
    int confirmed = 0;
    int producing = 0;
    int released = 0;
};

// 재고 상태 분류: 여유(SUFFICIENT) / 부족(SHORTAGE) / 고갈(DEPLETED)
enum class StockStatus { SUFFICIENT, SHORTAGE, DEPLETED };

// 시료 하나에 대한 재고 상태 스냅샷.
struct SampleStockStatus {
    std::string sampleId;
    std::string name;
    int stock;
    int pendingDemand;
    StockStatus status;
};

// 주문 현황과 시료 재고 현황을 집계하는 모니터링 도메인 로직을 담당한다.
class MonitoringService {
public:
    MonitoringService(Repository::IOrderRepository& orderRepository, Repository::ISampleRepository& sampleRepository);

    // 상태별 주문 개수를 집계한다. REJECTED는 집계하지 않는다.
    OrderStatusCounts CountOrdersByStatus() const;

    // 전체 시료에 대해 재고 상태(여유/부족/고갈)를 분류하여 반환한다.
    std::vector<SampleStockStatus> GetSampleStockStatuses() const;

    // 재고 상태 분류 규칙:
    //   stock == 0 이면 pendingDemand와 무관하게 DEPLETED(고갈)
    //   stock >= pendingDemand 이면 SUFFICIENT(여유)
    //   그 외(0 < stock < pendingDemand)에는 SHORTAGE(부족)
    static StockStatus ClassifyStockStatus(int stock, int pendingDemand);

private:
    // 해당 시료에 대한 미해결 수요(RESERVED, PRODUCING 상태 주문 수량의 합)를 계산한다.
    int CalculatePendingDemand(const std::string& sampleId) const;

    Repository::IOrderRepository& orderRepository_;
    Repository::ISampleRepository& sampleRepository_;
};

}  // namespace Service
