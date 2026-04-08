#include "ResourceManager.hpp"

namespace lab4::resource
{
std::shared_ptr<FileHandle> ResourceManager::get_resource(const std::string& path)
{
    auto it = cache.find(path);

    if (it != cache.end())
    {
        return it->second;
    }

    auto resource = std::make_shared<FileHandle>(path);

    // Сохраняем его в кеш, чтобы в следующий раз отдать мгновенно.
    cache[path] = resource;

    return resource;
}

void ResourceManager::collect_garbage()
{
    cache.clear();
}

} // namespace lab4::resource