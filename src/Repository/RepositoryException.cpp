#include "RepositoryException.h"

namespace Repository {

RepositoryException::RepositoryException(const std::string& message) : std::runtime_error(message) {}

}  // namespace Repository
