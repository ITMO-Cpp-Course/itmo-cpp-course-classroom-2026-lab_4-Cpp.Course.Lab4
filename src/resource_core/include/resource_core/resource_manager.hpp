#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace lab4::resource {

class FileHandle;

class ResourceManager {
public:
    std::shared_ptr<FileHandle> acquire(const std::string& path, const std::string& mode = "r");
    void purge_expired();
    std::size_t cache_size() const;
    bool release(const std::string& path);
    bool contains(const std::string& path) const;

private:
    struct CacheEntry {
        std::weak_ptr<FileHandle> handle;
        std::string mode;
    };

    std::unordered_map<std::string, CacheEntry> cache_;
};

} // namespace lab4::resource