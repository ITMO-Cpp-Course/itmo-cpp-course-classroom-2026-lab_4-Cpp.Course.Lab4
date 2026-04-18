#pragma once
#include <cstddef>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
namespace lab4::resource
{
class ResourceManager
{
  public:
    std::shared_ptr<FileHandle> acquire(const std::string& path, int flags);
    void clear();
    void release(const std::string& path);

  private:
    std::unordered_map<std::string, std::weak_ptr<FileHandle>> cache_;
};

} // namespace lab4::resource
