#pragma once

#include "FileHandle.hpp"
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace lab4::resource
{
class ResourceManager
{
  public:
    ResourceManager();
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete; // запрет копирования
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::shared_ptr<FileHandle> get_file(const std::string& filename, const std::string& mode = "r");
    void cleanup();
    bool is_cached(const std::string& filename) const;
    bool evict(const std::string& filename);

  private:
    std::unordered_map<std::string, std::weak_ptr<FileHandle>> cache_;
};
} // namespace lab4::resource
