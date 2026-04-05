#include "resource/ResourceManager.hpp"
#include "resource/ResourceError.hpp"

namespace lab4::resource
{

std::shared_ptr<FileHandle> ResourceManager::acquire(const std::string& path, const std::string& mode)
{
    std::unique_lock lock(mutex_);

    auto it = cache_.find(path);
    if (it != cache_.end())
    {
        if (auto existing = it->second.lock())
        {
            return existing;
        }
        cache_.erase(it);
    }

    auto handle = std::make_shared<FileHandle>(path, mode);
    cache_[path] = handle;
    return handle;
}

void ResourceManager::release(const std::string& path)
{
    std::unique_lock lock(mutex_);
    cache_.erase(path);
}

void ResourceManager::cleanup()
{
    std::unique_lock lock(mutex_);

    for (auto it = cache_.begin(); it != cache_.end();)
    {
        if (it->second.expired())
        {
            it = cache_.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

std::size_t ResourceManager::cache_size() const
{
    std::shared_lock lock(mutex_);

    std::size_t count = 0;
    for (const auto& [key, weak] : cache_)
    {
        if (!weak.expired())
        {
            ++count;
        }
    }
    return count;
}

bool ResourceManager::contains(const std::string& path) const
{
    std::shared_lock lock(mutex_);

    auto it = cache_.find(path);
    if (it == cache_.end())
    {
        return false;
    }
    return !it->second.expired();
}

} // namespace lab4::resource
