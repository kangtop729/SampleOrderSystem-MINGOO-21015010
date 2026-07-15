#pragma once

#include "../Model/Order.h"
#include "../Model/OrderStatus.h"
#include "../ThirdParty/nlohmann/json.hpp"

// Order 엔티티 <-> JSON 변환을 담당한다.
// JSON 스키마: {"orderNo":"...","sampleId":"...","customerName":"...","quantity":0,
//               "status":"RESERVED","createdAt":"2026-07-15T10:23:00Z",
//               "productionStartedAt":"2026-07-15T10:23:00Z"}
// status는 OrderStatus enum 이름 문자열, createdAt/productionStartedAt은 ISO8601 UTC(초 단위) 문자열이다.
// productionStartedAt은 아직 생산이 시작되지 않았으면 JSON null이며, 필드 자체가 없는(과거 버전) JSON도
// 하위 호환을 위해 nullopt로 안전하게 복원된다.
class OrderSerialization {
public:
    static nlohmann::json ToJson(const Model::Order& order);
    static Model::Order FromJson(const nlohmann::json& json);

private:
    static std::string StatusToString(Model::OrderStatus status);
    static Model::OrderStatus StatusFromString(const std::string& statusName);

    static std::string CreatedAtToIso8601(std::chrono::system_clock::time_point createdAt);
    static std::chrono::system_clock::time_point CreatedAtFromIso8601(const std::string& iso8601);
};
