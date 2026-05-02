#include "resource_manager.hpp"
#include <utility>

namespace lab4::resource
{

FileHandle& ResourceManager::GetFile(const std::string& path, const std::string& mode)
{
    auto it = m_resources.find(path);

    if (it != m_resources.end())
    {
        return it->second;
    }

    FileHandle new_handle(path, mode);

    auto [inserted_it, success] = m_resources.insert({path, std::move(new_handle)});

    return inserted_it->second;
}

void ResourceManager::ReleaseFile(const std::string& path)
{

    m_resources.erase(path);
}

bool ResourceManager::IsOpened(const std::string& path) const
{
    return m_resources.find(path) != m_resources.end();
}

} // namespace lab4::resource