#pragma once

#include "resource/FileHandle.hpp"

#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>

namespace lab4::resource
{

class ResourceManager
{
  public:
    ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    ResourceManager(ResourceManager&&) noexcept = default;
    ResourceManager& operator=(ResourceManager&&) noexcept = default;

    std::shared_ptr<FileHandle> acquire(const std::string& path, const std::string& mode);
    void release(const std::string& path);
    void cleanup();
    std::size_t cache_size() const;
    bool contains(const std::string& path) const;

  private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, std::weak_ptr<FileHandle>> cache_;
};

} // namespace lab4::resource
