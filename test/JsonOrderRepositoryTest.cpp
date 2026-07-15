#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

#include "../src/Model/Order.h"
#include "../src/Model/OrderStatus.h"
#include "../src/Repository/JsonOrderRepository.h"
#include "../src/Repository/RepositoryException.h"

using Model::Order;
using Model::OrderStatus;
using Repository::JsonOrderRepository;
using Repository::RepositoryException;

namespace {

void WriteRawFile(const std::filesystem::path& path, const std::string& content) {
    std::ofstream out(path, std::ios::trunc);
    out << content;
}

Order MakeOrder(const std::string& orderNo = "ORD-0001", const std::string& sampleId = "SMP-001", int quantity = 50) {
    return Order(orderNo, sampleId, "ACME Corp", quantity);
}

bool ContainsOrderNo(const std::vector<Order>& orders, const std::string& orderNo) {
    return std::find_if(orders.begin(), orders.end(), [&](const Order& o) { return o.GetOrderNo() == orderNo; }) !=
           orders.end();
}

}  // namespace

class JsonOrderRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        const std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
        filePath_ = std::filesystem::temp_directory_path() / ("order_repo_test_" + testName + ".json");
        std::filesystem::remove(filePath_);
    }

    void TearDown() override {
        std::filesystem::remove(filePath_);
    }

    std::filesystem::path filePath_;
};

// ---- 파일 부재 ----

TEST_F(JsonOrderRepositoryTest, 파일이_없으면_FindAll은_빈_벡터를_반환한다) {
    JsonOrderRepository repository(filePath_.string());

    EXPECT_TRUE(repository.FindAll().empty());
}

// ---- 저장/조회 ----

TEST_F(JsonOrderRepositoryTest, Save_후_FindAll에서_저장한_주문을_조회할_수_있다) {
    JsonOrderRepository repository(filePath_.string());

    repository.Save(MakeOrder());

    std::vector<Order> all = repository.FindAll();
    ASSERT_EQ(all.size(), 1u);
    EXPECT_EQ(all[0].GetOrderNo(), "ORD-0001");
    EXPECT_EQ(all[0].GetSampleId(), "SMP-001");
    EXPECT_EQ(all[0].GetCustomerName(), "ACME Corp");
    EXPECT_EQ(all[0].GetQuantity(), 50);
    EXPECT_EQ(all[0].GetStatus(), OrderStatus::RESERVED);
}

TEST_F(JsonOrderRepositoryTest, Save_후_FindById로_저장한_주문을_조회할_수_있다) {
    JsonOrderRepository repository(filePath_.string());

    repository.Save(MakeOrder());

    std::optional<Order> found = repository.FindById("ORD-0001");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->GetOrderNo(), "ORD-0001");
}

TEST_F(JsonOrderRepositoryTest, 존재하지_않는_id로_FindById_호출시_nullopt를_반환한다) {
    JsonOrderRepository repository(filePath_.string());
    repository.Save(MakeOrder());

    EXPECT_FALSE(repository.FindById("ORD-9999").has_value());
}

// ---- 재로드 시 데이터 일치 ----

TEST_F(JsonOrderRepositoryTest, 저장후_새_Repository_인스턴스로_재로드해도_데이터가_일치한다) {
    {
        JsonOrderRepository repository(filePath_.string());
        Order producingOrder = MakeOrder("ORD-0002", "SMP-002", 30);
        producingOrder.StartProducing();
        repository.Save(MakeOrder("ORD-0001", "SMP-001", 50));
        repository.Save(producingOrder);
    }

    JsonOrderRepository reloaded(filePath_.string());
    std::vector<Order> all = reloaded.FindAll();

    ASSERT_EQ(all.size(), 2u);
    std::optional<Order> first = reloaded.FindById("ORD-0001");
    std::optional<Order> second = reloaded.FindById("ORD-0002");
    ASSERT_TRUE(first.has_value());
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(first->GetStatus(), OrderStatus::RESERVED);
    EXPECT_EQ(second->GetStatus(), OrderStatus::PRODUCING);
    EXPECT_EQ(second->GetQuantity(), 30);
}

// ---- Update ----

TEST_F(JsonOrderRepositoryTest, Update_존재하는_id면_true를_반환하고_필드가_반영된다) {
    JsonOrderRepository repository(filePath_.string());
    repository.Save(MakeOrder());

    Order confirmed = MakeOrder();
    confirmed.Confirm();
    bool updated = repository.Update(confirmed);

    EXPECT_TRUE(updated);
    std::optional<Order> found = repository.FindById("ORD-0001");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->GetStatus(), OrderStatus::CONFIRMED);
}

TEST_F(JsonOrderRepositoryTest, Update_존재하지_않는_id면_false를_반환한다) {
    JsonOrderRepository repository(filePath_.string());
    repository.Save(MakeOrder());

    bool updated = repository.Update(MakeOrder("ORD-9999", "SMP-999", 1));

    EXPECT_FALSE(updated);
}

// ---- Remove ----

TEST_F(JsonOrderRepositoryTest, Remove_존재하는_id면_true를_반환하고_이후_조회되지_않는다) {
    JsonOrderRepository repository(filePath_.string());
    repository.Save(MakeOrder());

    bool removed = repository.Remove("ORD-0001");

    EXPECT_TRUE(removed);
    EXPECT_FALSE(repository.FindById("ORD-0001").has_value());
}

TEST_F(JsonOrderRepositoryTest, Remove_존재하지_않는_id면_false를_반환한다) {
    JsonOrderRepository repository(filePath_.string());
    repository.Save(MakeOrder());

    EXPECT_FALSE(repository.Remove("ORD-9999"));
}

// ---- FindBySampleId ----

TEST_F(JsonOrderRepositoryTest, FindBySampleId는_해당_시료의_주문만_반환한다) {
    JsonOrderRepository repository(filePath_.string());
    repository.Save(MakeOrder("ORD-0001", "SMP-001", 10));
    repository.Save(MakeOrder("ORD-0002", "SMP-001", 20));
    repository.Save(MakeOrder("ORD-0003", "SMP-002", 30));

    std::vector<Order> found = repository.FindBySampleId("SMP-001");

    ASSERT_EQ(found.size(), 2u);
    EXPECT_TRUE(ContainsOrderNo(found, "ORD-0001"));
    EXPECT_TRUE(ContainsOrderNo(found, "ORD-0002"));
    EXPECT_FALSE(ContainsOrderNo(found, "ORD-0003"));
}

TEST_F(JsonOrderRepositoryTest, FindBySampleId는_일치하는_주문이_없으면_빈_벡터를_반환한다) {
    JsonOrderRepository repository(filePath_.string());
    repository.Save(MakeOrder("ORD-0001", "SMP-001", 10));

    EXPECT_TRUE(repository.FindBySampleId("SMP-999").empty());
}

// ---- FindByStatus ----

TEST_F(JsonOrderRepositoryTest, FindByStatus는_해당_상태의_주문만_반환한다) {
    JsonOrderRepository repository(filePath_.string());
    Order reserved = MakeOrder("ORD-0001", "SMP-001", 10);
    Order rejected = MakeOrder("ORD-0002", "SMP-001", 20);
    rejected.Reject();
    Order producing = MakeOrder("ORD-0003", "SMP-002", 30);
    producing.StartProducing();

    repository.Save(reserved);
    repository.Save(rejected);
    repository.Save(producing);

    std::vector<Order> reservedOrders = repository.FindByStatus(OrderStatus::RESERVED);
    std::vector<Order> rejectedOrders = repository.FindByStatus(OrderStatus::REJECTED);
    std::vector<Order> producingOrders = repository.FindByStatus(OrderStatus::PRODUCING);

    ASSERT_EQ(reservedOrders.size(), 1u);
    EXPECT_EQ(reservedOrders[0].GetOrderNo(), "ORD-0001");
    ASSERT_EQ(rejectedOrders.size(), 1u);
    EXPECT_EQ(rejectedOrders[0].GetOrderNo(), "ORD-0002");
    ASSERT_EQ(producingOrders.size(), 1u);
    EXPECT_EQ(producingOrders[0].GetOrderNo(), "ORD-0003");
}

TEST_F(JsonOrderRepositoryTest, FindByStatus는_일치하는_주문이_없으면_빈_벡터를_반환한다) {
    JsonOrderRepository repository(filePath_.string());
    repository.Save(MakeOrder("ORD-0001", "SMP-001", 10));

    EXPECT_TRUE(repository.FindByStatus(OrderStatus::RELEASE).empty());
}

// ---- 손상된 파일 ----

TEST_F(JsonOrderRepositoryTest, 손상된_JSON_파일을_읽으면_RepositoryException을_던진다) {
    WriteRawFile(filePath_, "{ this is not valid json !!!");

    EXPECT_THROW(
        {
            JsonOrderRepository repository(filePath_.string());
            repository.FindAll();
        },
        RepositoryException);
}
