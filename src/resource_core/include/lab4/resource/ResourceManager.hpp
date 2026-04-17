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

    std::shared_ptr<FileHandler> getResource(std::string filepath_);
    void deleteResource(std::string filepath_);
    void clearExpired();

  private:
    std::unordered_map<std::string, std::weak_ptr<FileHandler>> data;
};

} // namespace lab4::resource