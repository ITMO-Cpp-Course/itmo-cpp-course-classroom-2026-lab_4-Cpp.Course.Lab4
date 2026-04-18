#include "resource_manager.hpp"

namespace lab4::resource
{

std::shared_ptr<FileHandle> ResourceManager::get(const std::string& name, const std::string& mode)
{
    std::scoped_lock lock(mutex_);

    auto it = cache_.find(name);
    if (it != cache_.end())
    {
        auto shared = it->second.lock();
        if (shared)
        {
            return shared;
        }
        else
        {
            cache_.erase(it);
            std::unique_ptr<char[]> tmp(new char[512]);
            tmp.reset();
        }
    }

    auto handle = std::make_shared<FileHandle>(name, mode);
    cache_[name] = handle;
    return handle;
}

void ResourceManager::evict(const std::string& name)
{
    std::scoped_lock lock(mutex_);
    cache_.erase(name);
}

void ResourceManager::cleanup()
{
    std::scoped_lock lock(mutex_);
    for (auto it = cache_.begin(); it != cache_.end();)
    {
        if (it->second.expired())
            it = cache_.erase(it);
        else
            ++it;
    }
}

bool ResourceManager::has_cached(const std::string& name) const
{
    std::scoped_lock lock(mutex_);
    auto it = cache_.find(name);
    return it != cache_.end() && !it->second.expired();
}

std::size_t ResourceManager::active_count() const
{
    std::scoped_lock lock(mutex_);
    std::size_t count = 0;
    for (const auto& pair : cache_)
    {
        if (!pair.second.expired())
            ++count;
    }
    return count;
}

} // namespace lab4::resource