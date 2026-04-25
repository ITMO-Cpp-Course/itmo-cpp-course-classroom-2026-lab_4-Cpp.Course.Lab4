#include "resource_manager.hpp"

namespace lab4::resource
{

std::shared_ptr<FileHandle> ResourceManager::getFile(const std::string& path,
                                                     const std::string& mode) // Возвращает std::shared_ptr<FileHandle>
{
    // Проверяем, есть ли в кеше живой объект
    auto it = cache_.find(path); // ищет в таблице элемент с ключом path и возвращает итератор - который указывает на
                                 // пару ключ-значение внутри контейнера
    if (it != cache_.end())      // если ключ не не найден
    {
        if (auto shared = it->second.lock()) // пытаемся создать std::shared_ptr<FileHandle> из слабой ссылки
        {
            return shared; // используем уже открытый файл, разделяем владение
        }
    }

    auto newFile =
        std::make_shared<FileHandle>(path, mode); // создаем новый FileHandle (make выделяет память сразу на блок)
    cache_[path] = newFile;                       // weak_ptr автоматически создаётся из shared_ptr
    return newFile;
}

void ResourceManager::cleanExpired()
{
    for (auto it = cache_.begin();
         it != cache_.end();) // возвращаемся к первому элементу кэша и проверяем не дощли ли до конца
    {
        if (it->second.expired()) // если слабая ссылка сдохла
        {
            it = cache_.erase(it); // Удаляем элемент и идём некст
        }
        else
        {
            ++it;
        }
    }
}

} // namespace lab4::resource