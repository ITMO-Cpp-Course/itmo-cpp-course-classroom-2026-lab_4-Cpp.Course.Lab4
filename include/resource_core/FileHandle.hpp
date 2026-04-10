#pragma once

#include <cstdio>
#include <string>

namespace lab4::resource
{
class FileHandle
{
  public:
    FileHandle();

    explicit FileHandle(const std::string& file_path,
                        const std::string& open_mode = "r"); // конструктор для открытия файла

    ~FileHandle();

    FileHandle(const FileHandle& other) =
        delete; // запрещаем копирование (и для конструктора, и для оператора присваивания)
    FileHandle& operator=(const FileHandle& other) = delete;

    FileHandle(FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept; // передать ресурс от одного объекта другому, не копируя его

    void open(const std::string& file_path, const std::string& open_mode = "r");
    void close();
    bool is_open() const;

    std::string read_line();

    void write_line(const std::string& line);

    std::FILE* get_native_handle() const;

  private:
    std::FILE* file_; // Указатель на файл
    std::string filename_;

    void check_open() const;
};

} // namespace lab4::resource