#pragma once

#include "file_handle.hpp"
#include <map>
#include <memory>
#include <string>

namespace lab4::resource
{

class ResourceManager
{
  public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::shared_ptr<FileHandle> open(const std::string& path, const std::string& mode);

  private:
    std::map<std::string, std::weak_ptr<FileHandle>> cache_;
};

} // namespace lab4::resource