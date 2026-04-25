#include "resource_core/resource_manager.hpp"
#include "resource_core/file_handle.hpp"

namespace lab4::resource {

std::shared_ptr<FileHandle> ResourceManager::acquire(const std::string& path, const std::string& mode) {
    auto it = cache_.find(path);
    if (it != cache_.end()) {
        auto existing = it->second.handle.lock();
        if (existing) {
            if (it->second.mode == mode) {
                return existing;
            }
            throw ResourceError("File '" + path + "' already opened with different mode");
        } else {
            cache_.erase(it);
        }
    }

    auto new_handle = std::make_shared<FileHandle>(path, mode);
    cache_.emplace(path, CacheEntry{new_handle, mode});
    return new_handle;
}

void ResourceManager::purge_expired() {
    for (auto it = cache_.begin(); it != cache_.end(); ) {
        if (it->second.handle.expired()) {
            it = cache_.erase(it);
        } else {
            ++it;
        }
    }
}

std::size_t ResourceManager::cache_size() const {
    return cache_.size();
}

bool ResourceManager::release(const std::string& path) {
    auto it = cache_.find(path);
    if (it != cache_.end()) {
        if (auto h = it->second.handle.lock()) {
            h->close();
        }
        cache_.erase(it);
        return true;
    }
    return false;
}

bool ResourceManager::contains(const std::string& path) const {
    auto it = cache_.find(path);
    if (it == cache_.end()) return false;
    return !it->second.expired();
}

} // namespace lab4::resource