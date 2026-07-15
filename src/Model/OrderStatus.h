#pragma once

namespace Model {

// 주문 상태 머신
//
// RESERVED  --(거절)-------------------> REJECTED   (종단, 모니터링 제외)
// RESERVED  --(승인, 재고 충분)---------> CONFIRMED
// RESERVED  --(승인, 재고 부족)---------> PRODUCING --(생산 완료)--> CONFIRMED
// CONFIRMED --(출고)--------------------> RELEASE
enum class OrderStatus {
    RESERVED,
    REJECTED,
    PRODUCING,
    CONFIRMED,
    RELEASE
};

}  // namespace Model
