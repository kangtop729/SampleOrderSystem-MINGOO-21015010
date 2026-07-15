#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "../Model/ProductionJob.h"
#include "../Repository/IOrderRepository.h"
#include "../Repository/ISampleRepository.h"

namespace Service {

// 생산 라인 대기열이 비어 있는 상태에서 현재 작업을 조회/완료하려 할 때 던지는 예외.
class NoProductionJobException : public std::logic_error {
public:
    explicit NoProductionJobException(const std::string& message);
};

// 단일 생산 라인을 FIFO로 관리하는 도메인 로직을 담당한다.
// PRODUCING 상태의 주문들을 접수 시각(createdAt) 오름차순으로 정렬한 대기열을 구성하고,
// 맨 앞 작업을 완료 처리(재고 증가 + 주문 상태 전환)한다.
class ProductionLineService {
public:
    ProductionLineService(Repository::IOrderRepository& orderRepository, Repository::ISampleRepository& sampleRepository);

    // 부족분 = max(0, 주문 수량 - 현재 재고)
    static int CalculateShortfall(int quantity, int currentStock);

    // 실 생산량 = ceil(부족분 / 수율). 부족분이 0 이하이면 0을 반환한다.
    // 부동소수점 오차로 인한 ceil 오반올림을 방지하기 위해 epsilon 보정을 적용한다.
    static int CalculateActualProductionQty(int shortfall, double yield);

    // 총 생산 시간 = 평균 생산시간 곱하기 실 생산량
    static double CalculateTotalProductionMinutes(double averageProductionTimeMinutes, int actualProductionQty);

    // PRODUCING 상태의 주문들을 접수 시각(createdAt) 오름차순으로 정렬한 생산 대기열을 반환한다.
    std::vector<Model::ProductionJob> GetQueue() const;

    // 대기열의 맨 앞(가장 오래된) 작업을 반환한다. 대기열이 비어있으면 NoProductionJobException을 던진다.
    Model::ProductionJob GetCurrentJob() const;

    // 대기열의 맨 앞 작업을 완료 처리한다: 재고에 실 생산량을 더하고, 해당 주문을 PRODUCING -> CONFIRMED로
    // 전환한다. 대기열이 비어있으면 NoProductionJobException을 던지며, 어떠한 Repository 갱신도 수행하지
    // 않는다.
    void CompleteCurrentJob();

    // 단일 생산 라인을 순차(FIFO) 처리한다. 대기열의 맨 앞 주문이 아직 생산을 시작하지 않았다면(
    // productionStartedAt이 없다면) 지금 시각을 생산 시작 시각으로 기록하여 영속화하고 반환한다(이번
    // 호출에서는 그 이상 진행하지 않는다). 맨 앞 주문이 이미 시작된 상태라면, 생산 시작 시각 기준 경과
    // 시간이 총 생산 시간(분) 이상인 경우에만 완료 처리(재고 증가 + CONFIRMED 전환)하고, 그 다음 대기
    // 주문으로 넘어가 같은 과정을 반복한다. 뒤쪽 대기 주문들은 앞 작업이 끝나기 전까지는 전혀 진행되지
    // 않는다(진짜 순차 처리, 병렬 진행 없음).
    // 반환값: 자동 완료된 건수(0이면 완료 대상 없음).
    int AutoCompleteFinishedJobs();

private:
    // FindById로 조회한 Sample과, 해당 주문에 대한 부족분/실 생산량 계산 결과를 함께 묶어 반환한다.
    // BuildProductionJob과 CompleteCurrentJob이 동일한 조회/계산 로직을 중복하지 않도록 공유한다.
    struct OrderProductionCalculation {
        Model::Sample sample;
        int shortfall;
        int actualProductionQty;
    };

    std::vector<Model::Order> GetSortedProducingOrders() const;
    OrderProductionCalculation CalculateForOrder(const Model::Order& order) const;
    Model::ProductionJob BuildProductionJob(const Model::Order& order, bool isFront) const;

    Repository::IOrderRepository& orderRepository_;
    Repository::ISampleRepository& sampleRepository_;
};

}  // namespace Service
