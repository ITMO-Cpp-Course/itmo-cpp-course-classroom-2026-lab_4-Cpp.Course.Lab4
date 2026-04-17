#pragma once
#include "file_handle.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace lab4::resource
{

class FileHandle; // предварительное объявление

class ResourceManager
{
  public:
    std::shared_ptr<FileHandle> get(const std::string& path, const std::string& mode = "r");

    void remove(const std::string& path); // Удаляет запись из кеша, если она есть

  private:
    std::unordered_map<std::string, std::weak_ptr<FileHandle>> cache_;
};

} // namespace lab4::resource
