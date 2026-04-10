#include "resource_core/ResourceManager.hpp"
#include "resource_core/ResourceError.hpp"

namespace lab4::resource
{
ResourceManager::ResourceManager() = default;

std::shared_ptr<FileHandle> ResourceManager::get_file(const std::string& filename, const std::string& mode)
{

    auto iterator = cache_.find(filename); // ищем файл в хеш-таблице
    if (iterator != cache_.end())
    { // если нашли
        auto shared_pointer =
            iterator->second.lock(); // превращаем weak_ptr в shared_ptr. lock() вернет nullptr, если объект уже удален
        if (shared_pointer)
        {
            return shared_pointer;
        }
    }
    auto new_file = std::make_shared<FileHandle>(filename, mode);
    cache_[filename] = new_file; // кладём  в файл значения
    return new_file;
}

void ResourceManager::cleanup()
{ // очистка кеша

    auto iterator = cache_.begin();
    while (iterator != cache_.end())
    {
        if (iterator->second.expired())
        {                                      // если объект удален
            iterator = cache_.erase(iterator); // удаляем запись из кэша
        }
        else
        {
            ++iterator;
        }
    }
}

// есть ли файл в кеше
bool ResourceManager::is_cached(const std::string& filename) const
{
    auto iterator = cache_.find(filename);
    return iterator != cache_.end() && !iterator->second.expired(); // если нашли и объект не удалён
}
} // namespace lab4::resource