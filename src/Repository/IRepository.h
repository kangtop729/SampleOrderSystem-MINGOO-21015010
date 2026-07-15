#pragma once

#include <optional>
#include <string>
#include <vector>

namespace Repository {

// 엔티티 타입 T에 대한 공통 CRUD 인터페이스.
// 도메인 특화 조회(예: 이름으로 검색)는 하위 인터페이스(ISampleRepository 등)에서 추가한다.
template <typename T>
class IRepository {
public:
    virtual ~IRepository() = default;

    virtual void Save(const T& entity) = 0;
    virtual std::vector<T> FindAll() const = 0;
    virtual std::optional<T> FindById(const std::string& id) const = 0;
    virtual bool Update(const T& entity) = 0;
    virtual bool Remove(const std::string& id) = 0;
};

}  // namespace Repository
