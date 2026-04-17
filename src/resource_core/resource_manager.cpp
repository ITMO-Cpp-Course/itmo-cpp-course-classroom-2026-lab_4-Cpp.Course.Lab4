#include "resource_manager.hpp"
#include "file_handle.hpp"

namespace lab4::resource
{

std::shared_ptr<FileHandle> ResourceManager::get(const std::string& path, const std::string& mode)
{
    auto it = cache_.find(path);
    if (it != cache_.end())
    {
        auto ptr = it->second.lock();
        if (ptr)
        {
            return ptr;
        }
        else
        {
            cache_.erase(it);
        }
    }
    auto newFile = std::make_shared<FileHandle>(path, mode);
    cache_[path] = newFile;
    return newFile;
}

void ResourceManager::remove(const std::string& path)
{
    cache_.erase(path);
}

} // namespace lab4::resource
