#ifndef LAB4_RESOURCE_MANAGER_HPP
#define LAB4_RESOURCE_MANAGER_HPP
#include "file_handle.hpp"
#include <map>
#include <memory>
#include <string>
namespace lab4::resource
{
class ResourceManager
{
  private:
    std::map<std::string, std::weak_ptr<FileHandle>> m_resources;

  public:
    ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) noexcept = default;
    ResourceManager& operator=(ResourceManager&&) noexcept = default;
    std::shared_ptr<FileHandle> GetFile(const std::string& path, const std::string& mode);
    void ReleaseFile(const std::string& path);
    [[nodiscard]] bool IsOpened(const std::string& path) const;
};
} // namespace lab4::resource
#endif // LAB4_RESOURCE_MANAGER_HPP
