#include "resource_manager.hpp"

namespace lab4::resource
{

std::shared_ptr<FileHandle> ResourceManager::open(const std::string& path, const std::string& mode)
{
    auto it = cache_.find(path);
    if (it != cache_.end())
    {
        if (auto handle = it->second.lock())
        {
            return handle;
        }
    }

    auto handle = std::make_shared<FileHandle>(path, mode);
    cache_[path] = handle; // неявно преобразовались в weak, все норм
    return handle;
}

} // namespace lab4::resource