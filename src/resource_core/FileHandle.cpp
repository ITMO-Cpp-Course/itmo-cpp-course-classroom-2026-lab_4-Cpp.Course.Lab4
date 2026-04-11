#include "FileHandle.hpp"
#include "ResourceError.hpp"
#include <iostream>

namespace lab4::resource
{

FileHandle::FileHandle(const std::string& path, const std::string& mode)
{
    handle = std::fopen(path.c_str(), mode.c_str());

    if (!handle)
    {
        throw ResourceError("Не удалось открыть файл по пути " + path);
    }

    std::cout << "Ресурс успешно захвачен " << path << "\n";
}

FileHandle::~FileHandle()
{
    if (handle)
    {
        std::fclose(handle);
        std::cout << "Ресурс освобожден деструктором\n";
    }
}

FileHandle::FileHandle(FileHandle&& other) noexcept : handle(other.handle)
{
    // зануляем забранный указатель у старого объекта,
    // чтобы его деструктор не закрыл наш файл.
    other.handle = nullptr;
}

FileHandle& FileHandle::operator=(FileHandle&& other) noexcept
{
    if (this != &other)
    {
        if (handle)
        {
            std::fclose(handle);
        }
        handle = other.handle;
        other.handle = nullptr;
    }
    return *this;
}

} // namespace lab4::resource