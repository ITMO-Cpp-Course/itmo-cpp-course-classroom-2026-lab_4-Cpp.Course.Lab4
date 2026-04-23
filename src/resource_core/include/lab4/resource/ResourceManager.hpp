#pragma once

#include <lab4/resource/FileHandler.hpp>
#include <memory>
#include <unordered_map>

namespace lab4::resource
{

class ResourceManager
{
  public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    std::shared_ptr<FileHandler> getResource(const std::string& filepath_);
    void deleteResource(const std::string& filepath_); // only removes weak_ptr, handlers remain valid
    void clearExpired();

    std::size_t getDataSize() const;

  private:
    std::unordered_map<std::string, std::weak_ptr<FileHandler>> data;
};

} // namespace lab4::resource