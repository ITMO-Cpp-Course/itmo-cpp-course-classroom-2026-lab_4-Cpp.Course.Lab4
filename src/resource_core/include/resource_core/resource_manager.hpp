#pragma once

#include "file_handle.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace lab4::resource
{

class ResourceManager
{
  public:
    std::shared_ptr<FileHandle> open(const std::string& path, const std::string& mode = "r");

    [[nodiscard]] std::size_t cache_size() const noexcept;
    void purge_expired() noexcept;

  private:
    std::unordered_map<std::string, std::weak_ptr<FileHandle>> cache_;
};

} // namespace lab4::resource
