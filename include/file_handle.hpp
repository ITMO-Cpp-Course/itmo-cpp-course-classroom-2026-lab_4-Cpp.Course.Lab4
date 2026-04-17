#pragma once

#include <cstdio> // для FILE*, fopen, fclose
#include <string>

namespace lab4::resource
{

class FileHandle
{
  public:
    FileHandle();

    FileHandle(const std::string& path, const std::string& mode);

    ~FileHandle();

    // Move-конструктор и move-присваивание (передача владения)
    FileHandle(FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept; // выбрасывает свой файл и забирает файл у другоо

    // Запрещаем копирование (нельзя скопировать владение файлом)
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    // Методы для работы с файлом
    void open(const std::string& path, const std::string& mode);
    void close();
    bool is_open() const;

  private:
    FILE* file_; // указатель на открытый файл или nullptr
};

} // namespace lab4::resource
