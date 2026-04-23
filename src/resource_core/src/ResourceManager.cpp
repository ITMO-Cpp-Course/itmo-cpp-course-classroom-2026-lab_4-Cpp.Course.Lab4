#include "lab4/resource/ResourceManager.hpp"
#include "lab4/resource/ResourceError.hpp"

namespace lab4::resource
{

void ResourceManager::clearExpired()
{
    for (auto it = data.begin(); it != data.end();)
    {
        if (it->second.expired())
        {
            it = data.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

std::shared_ptr<FileHandler> ResourceManager::getResource(const std::string& filepath_)
{
    auto it = data.find(filepath_);

    if (it != data.end())
    {
        std::shared_ptr<FileHandler> resource = it->second.lock();

        if (resource)
        {
            return resource;
        }
        else
        {
            data.erase(it);
        }
    }

    // try to open the file again if weak_ptr is expired or resource is not present in data
    try
    {
        std::shared_ptr<FileHandler> resource = std::make_shared<FileHandler>(filepath_);
        data[filepath_] = resource;
        return resource;
    }
    catch (const ResourceError& e)
    {
        throw e;
    }
}

void ResourceManager::deleteResource(const std::string& filepath_)
{
    auto it = data.find(filepath_);
    if (it != data.end())
    {
        data.erase(it);
    }
    else
    {
        throw ResourceError("File is not cached!");
    }
}

std::size_t ResourceManager::getDataSize() const
{
    return data.size();
}

} // namespace lab4::resource