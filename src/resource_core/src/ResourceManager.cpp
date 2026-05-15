#include "resource_core/ResourceManager.hpp"
namespace lab4::resource
{

std::shared_ptr<FileHandle> ResourceManager::getResource(const std::string& path)
{
    auto it = cache.find(path);

    if (it != cache.end())
    {
        if (auto shared_res = it->second.lock())
        {
            return shared_res;
        }
    }

    auto new_res = std::make_shared<FileHandle>(path);

    cache[path] = new_res;

    return new_res;
}

} // namespace lab4::resource