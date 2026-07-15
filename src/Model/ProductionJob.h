#pragma once

#include <string>

namespace Model {

// 생산 라인 대기열의 한 항목(ProductionJob)을 나타내는 불변 값 타입.
// 주문 정보와 함께, 해당 주문을 생산 완료하는 데 필요한 계산 결과(부족분/실 생산량/총 생산 시간)를 보관한다.
class ProductionJob {
public:
    ProductionJob(std::string orderNo, std::string sampleId, std::string customerName, int quantity,
                  int currentStock, int shortfall, int actualProductionQty, double totalProductionMinutes);

    const std::string& GetOrderNo() const;
    const std::string& GetSampleId() const;
    const std::string& GetCustomerName() const;
    int GetQuantity() const;
    int GetCurrentStock() const;
    int GetShortfall() const;
    int GetActualProductionQty() const;
    double GetTotalProductionMinutes() const;

private:
    std::string orderNo_;
    std::string sampleId_;
    std::string customerName_;
    int quantity_;
    int currentStock_;
    int shortfall_;
    int actualProductionQty_;
    double totalProductionMinutes_;
};

}  // namespace Model
