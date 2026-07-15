#include "JsonOrderRepository.h"

#include <algorithm>
#include <fstream>

#include "../Serialization/OrderSerialization.h"
#include "RepositoryException.h"

using Model::Order;
using Model::OrderStatus;

namespace Repository {

JsonOrderRepository::JsonOrderRepository(std::string filePath) : filePath_(std::move(filePath)) {}

std::vector<Order> JsonOrderRepository::Load() const {
    std::ifstream in(filePath_);
    if (!in.is_open()) {
        return {};
    }

    nlohmann::json root;
    try {
        in >> root;
    } catch (const nlohmann::json::parse_error& e) {
        throw RepositoryException(std::string("JsonOrderRepository: JSON 파싱에 실패했습니다: ") + e.what());
    }

    std::vector<Order> orders;
    for (const nlohmann::json& item : root) {
        orders.push_back(OrderSerialization::FromJson(item));
    }
    return orders;
}

void JsonOrderRepository::Flush(const std::vector<Order>& orders) const {
    nlohmann::json root = nlohmann::json::array();
    for (const Order& order : orders) {
        root.push_back(OrderSerialization::ToJson(order));
    }

    std::ofstream out(filePath_, std::ios::trunc);
    out << root.dump(2);
}

void JsonOrderRepository::Save(const Order& order) {
    std::vector<Order> orders = Load();
    orders.push_back(order);
    Flush(orders);
}

std::vector<Order> JsonOrderRepository::FindAll() const {
    return Load();
}

std::optional<Order> JsonOrderRepository::FindById(const std::string& orderNo) const {
    std::vector<Order> orders = Load();
    auto it = std::find_if(orders.begin(), orders.end(),
                            [&](const Order& o) { return o.GetOrderNo() == orderNo; });
    if (it == orders.end()) {
        return std::nullopt;
    }
    return *it;
}

bool JsonOrderRepository::Update(const Order& order) {
    std::vector<Order> orders = Load();
    auto it = std::find_if(orders.begin(), orders.end(),
                            [&](const Order& o) { return o.GetOrderNo() == order.GetOrderNo(); });
    if (it == orders.end()) {
        return false;
    }
    *it = order;
    Flush(orders);
    return true;
}

bool JsonOrderRepository::Remove(const std::string& orderNo) {
    std::vector<Order> orders = Load();
    auto it = std::find_if(orders.begin(), orders.end(),
                            [&](const Order& o) { return o.GetOrderNo() == orderNo; });
    if (it == orders.end()) {
        return false;
    }
    orders.erase(it);
    Flush(orders);
    return true;
}

std::vector<Order> JsonOrderRepository::FindBySampleId(const std::string& sampleId) const {
    std::vector<Order> orders = Load();
    std::vector<Order> matched;
    std::copy_if(orders.begin(), orders.end(), std::back_inserter(matched),
                 [&](const Order& o) { return o.GetSampleId() == sampleId; });
    return matched;
}

std::vector<Order> JsonOrderRepository::FindByStatus(OrderStatus status) const {
    std::vector<Order> orders = Load();
    std::vector<Order> matched;
    std::copy_if(orders.begin(), orders.end(), std::back_inserter(matched),
                 [&](const Order& o) { return o.GetStatus() == status; });
    return matched;
}

}  // namespace Repository
