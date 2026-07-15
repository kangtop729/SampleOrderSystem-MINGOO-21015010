#pragma once

#include <stdexcept>
#include <string>

namespace Repository {

// Repository 계층에서 발생하는 예외(예: JSON 파일 손상으로 인한 파싱 실패)를 나타낸다.
class RepositoryException : public std::runtime_error {
public:
    explicit RepositoryException(const std::string& message);
};

}  // namespace Repository
