#include "resource_core/resource_manager.hpp"

namespace lab4::resource {

std::shared_ptr<FileHandle> ResourceManager::open(const std::string& path, const std::string& mode) {
    auto it = cache_.find(path);
    if (it != cache_.end()) {
        if (auto handle = it->second.lock()) {
            return handle;
        }
    }

    auto handle = std::make_shared<FileHandle>(path, mode);
    cache_[path] = handle;
    return handle;
}

std::size_t ResourceManager::cache_size() const noexcept {
    return cache_.size();
}

void ResourceManager::purge_expired() noexcept {
    for (auto it = cache_.begin(); it != cache_.end(); ) {
        if (it->second.expired()) {
            it = cache_.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace lab4::resource
