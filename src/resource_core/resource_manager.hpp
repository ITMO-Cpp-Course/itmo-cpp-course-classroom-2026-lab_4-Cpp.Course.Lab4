#pragma once

#include "handle.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace lab4::resource
{

class ResourceManager
{
    std::unordered_map<std::string, std::weak_ptr<FileHandle>> cache_;

  public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    ResourceManager(ResourceManager&&) = default;
    ResourceManager& operator=(ResourceManager&&) = default;

    std::shared_ptr<FileHandle> getResource(const std::string& name, const std::string& mode);
};

} // namespace lab4::resource