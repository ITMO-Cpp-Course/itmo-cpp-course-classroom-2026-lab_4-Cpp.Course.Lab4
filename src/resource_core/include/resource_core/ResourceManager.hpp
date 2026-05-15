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
    std::map<std::string, std::weak_ptr<FileHandle>> cache;

  public:
   // ResourceManager() = default;

        std::shared_ptr<FileHandle> getResource(const std::string& path);
};

} // namespace lab4::resource