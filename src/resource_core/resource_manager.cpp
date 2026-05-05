#include "resource_manager.hpp"
#include <utility>

namespace lab4::resource
{

std::shared_ptr<FileHandle> ResourceManager::GetFile(const std::string& path, const std::string& mode)
{
    auto it = m_resources.find(path);

    if (it != m_resources.end())
    {
        if (std::shared_ptr<FileHandle> shared = it->second.lock())
        {
            return shared;
        }
    }

    auto new_handle = std::make_shared<FileHandle>(path, mode);
    m_resources[path] = new_handle;
    return new_handle;
}

void ResourceManager::ReleaseFile(const std::string& path)
{

    m_resources.erase(path);
}

bool ResourceManager::IsOpened(const std::string& path) const
{
    auto it = m_resources.find(path);
    if (it != m_resources.end())
    {
        return !it->second.expired();
    }
    return false;
}

} // namespace lab4::resource