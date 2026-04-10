#pragma once

#include "FileHandle.hpp"
#include <map>
#include <memory>
#include <string>

namespace lab4::resource
{

class ResourceManager
{
  private:
    // Мы используем shared_ptr, умный указатель, чтобы несколько частей программы
    // могли безопасно использовать один и тот же файл.
    std::map<std::string, std::shared_ptr<FileHandle>> cache;

  public:
    ResourceManager() = default;
    std::shared_ptr<FileHandle> get_resource(const std::string& path);

    // Метод для очистки кеша (принудительное закрытие всех файлов)
    void clear_everything();

    // Запрещаем копирование менеджера, потому что менеджер ресурсов должен быть один
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
};

} // namespace lab4::resource