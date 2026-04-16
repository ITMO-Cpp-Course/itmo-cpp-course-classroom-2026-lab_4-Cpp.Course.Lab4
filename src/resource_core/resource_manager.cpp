#include "resource_manager.hpp"
#include "resource_error.hpp"

namespace lab4::resource
{
std::shared_ptr<FileHandle> ResourceManager::getResource(const std::string& name, const std::string& mode)
{
    auto check_cache = cache_.find(name);
    if (check_cache != cache_.end())
    {
        auto resource = check_cache->second.lock();
        if (resource)
        {
            return resource;
        }
        else
        {
            cache_.erase(check_cache);
        }
    }
    auto resource = std::make_shared<FileHandle>(name, mode);
    cache_[name] = resource;
    return resource;
}
} // namespace lab4::resource