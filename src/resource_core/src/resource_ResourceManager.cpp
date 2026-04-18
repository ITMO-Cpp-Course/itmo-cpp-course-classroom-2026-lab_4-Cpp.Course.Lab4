#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <lab4/resource/resource_FileHandle.hpp>
#include <lab4/resource/resource_ResourceError.hpp>
#include <lab4/resource/resource_ResourceManager.hpp>
#include <unistd.h>
#include <vector>
namespace lab4::resource
{
std::shared_ptr<FileHandle> ResourceManager::acquire(const std::string& path, int flags)
{
    auto it = cache_.find(path);
    if (it != cache_.end())
    {
        auto oldHandle = it->second.lock();
        if (oldHandle)
        {
            return oldHandle;
        }
        else
        {
            cache_.erase(it);
        }
    }
    auto newHandle = std::make_shared<FileHandle>(path, flags);
    cache_[path] = newHandle;
    return newHandle;
}

void ResourceManager::clear()
{
    cache_.clear();
}

void ResourceManager::release(const std::string& path)
{
    auto it = cache_.find(path);
    if (it != cache_.end() && it->second.expired())
    {
        cache_.erase(it);
    }
}

}; // namespace lab4::resource
