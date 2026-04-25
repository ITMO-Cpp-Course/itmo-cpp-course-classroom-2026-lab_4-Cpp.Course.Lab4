#pragma once
#include "file_handle.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace lab4::resource
{

class ResourceManager
{
  public:
    std::shared_ptr<FileHandle> getFile(
        const std::string& path, const std::string& mode); // Получить разделяемый доступ к файлу. path - путь к файлу
    // Проверяем, есть ли в кеше запись для path и жив ли ещё weak_ptr.Если да, возвращаем shared_ptr,полученный через
    // lock(). Иначе создаём новый FileHandle, оборачиваем в shared_ptr, кладём в кеш weak_ptr и возвращаем Это всё для
    // разделяемого использования
    void cleanExpired(); // Очищает кеш от дохлых слабых ссылок(у которых weak_ptr::expired() == true)

    size_t cacheSize() const // Количество элементов в кеше , нужен в тестах
    {
        return cache_.size();
    }

  private:
    std::unordered_map<std::string, std::weak_ptr<FileHandle>>
        cache_; // ассоц контейнер для пар путь к файлу-значение файла. Здесь слабую ссылку
};

} // namespace lab4::resource