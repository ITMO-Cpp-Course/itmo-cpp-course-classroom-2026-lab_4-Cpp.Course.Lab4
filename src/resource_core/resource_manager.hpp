#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "file_handle.hpp"

namespace lab4::resource
{

class ResourceManager
{
  public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    std::shared_ptr<FileHandle> get(const std::string& name, const std::string& mode = "r");

    void evict(const std::string& name);
    void cleanup();
    bool has_cached(const std::string& name) const;
    std::size_t active_count() const;

  private:
    std::unordered_map<std::string, std::weak_ptr<FileHandle>> cache_;
    mutable std::mutex mutex_;
};

} // namespace lab4::resource