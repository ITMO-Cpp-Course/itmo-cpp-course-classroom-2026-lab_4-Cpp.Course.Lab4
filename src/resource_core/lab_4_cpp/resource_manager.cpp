#include "lab4_hpp/resource_manager.hpp"
#include "lab4_hpp/file_handle.hpp"
#include "lab4_hpp/resource_error.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace lab4::resource
{
ResourceManager& ResourceManager::GetObject()
{
    static ResourceManager obj;
    return obj;
}
std::shared_ptr<FileHandle> ResourceManager::GetFile(const std::string& FileName_, std::ios::openmode mode)
{
    auto FilePtr = Data_.find(FileName_);

    if (FilePtr != Data_.end())
    {
        auto ptr = FilePtr->second.lock();

        if (ptr)
        {
            return ptr;
        }
        Data_.erase(FilePtr);
    }
    auto NewFilePtr = std::make_shared<FileHandle>(FileName_, mode);

    Data_[FileName_] = NewFilePtr;

    return NewFilePtr;
}
void ResourceManager::DeleteFile(const std::string& FileName_)
{
    auto FilePtr = Data_.find(FileName_);
    if (FilePtr != Data_.end())
    {
        Data_.erase(FilePtr);
    }
}
size_t ResourceManager::DataSize() const
{
    size_t count = 0;
    for (const auto& a : Data_)
    {
        if (!a.second.expired())
        {
            count++;
        }
    }
    return count;
}

void ResourceManager::ClearData()
{
    Data_.clear();
}
bool ResourceManager::IsData(const std::string& FileName_) const
{
    auto FilePtr = Data_.find(FileName_);
    if (FilePtr == Data_.end())
    {
        return false;
    }

    return !FilePtr->second.expired();
}

} // namespace lab4::resource