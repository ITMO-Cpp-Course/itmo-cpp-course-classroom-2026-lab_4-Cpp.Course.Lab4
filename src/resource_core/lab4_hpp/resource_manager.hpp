#pragma once
#include "lab4_hpp/file_handle.hpp"
#include "lab4_hpp/resource_error.hpp"
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace lab4::resource
{
class ResourceManager
{
  public:
    static ResourceManager& GetObject();
    std::shared_ptr<FileHandle> GetFile(const std::string& FileName_, std::ios::openmode mode = std::ios::in);
    void DeleteFile(const std::string& FileName_);
    void ClearData();
    size_t DataSize() const;
    bool IsData(const std::string& FileName_) const;

  private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::unordered_map<std::string, std::weak_ptr<FileHandle>> Data_;
};

} // namespace lab4::resource