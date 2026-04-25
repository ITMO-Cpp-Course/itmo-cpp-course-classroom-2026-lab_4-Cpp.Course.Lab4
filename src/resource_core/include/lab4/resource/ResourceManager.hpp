#pragma once
#include <lab4/resource/FileHandle.hpp>
#include <map>
#include <memory>
#include <string>
namespace lab4::resource
{
class ResourceManager
{
  private:
    std::map<std::string, std::weak_ptr<FileHandle>> cache_;

  public:
    ResourceManager() = default;
    std::shared_ptr<FileHandle> getResource(const std::string& path);
    unsigned int getResourceCount() const;
};
} // namespace lab4::resource
