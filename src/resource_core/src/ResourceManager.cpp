#include <lab4/resource/FileHandle.hpp>
#include <lab4/resource/ResourceError.hpp>
#include <lab4/resource/ResourceManager.hpp>

namespace lab4::resource
{
std::shared_ptr<FileHandle> ResourceManager::getResource(const std::string& path)
{
    auto file = cache_.find(path);
    if (file != cache_.end())
    {
        std::weak_ptr<FileHandle> weak_Res = file->second;
        std::shared_ptr<FileHandle> shared = weak_Res.lock();
        if (shared != nullptr)
        {
            return shared;
        }
    }
    auto new_resource = std::make_shared<FileHandle>(path);

    cache_[path] = new_resource;

    return new_resource;
}
unsigned int ResourceManager::getResourceCount() const
{
    unsigned int count = 0;
    for (auto const& [path, weak_Res] : cache_)
    {
        if (!weak_Res.expired())
        {
            count++;
        }
    }
    return count;
}
} // namespace lab4::resource
