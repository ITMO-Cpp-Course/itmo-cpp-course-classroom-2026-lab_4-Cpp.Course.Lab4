#pragma once

#include <string>

namespace lab4::resource {

// RAII-обёртка над файловым дескриптором (POSIX).

class FileHandle {
public:
    //создаёт неоткрытый объект
    FileHandle() noexcept = default;



    explicit FileHandle(const std::string& path, const std::string& mode);

    //закрывает файл
    ~FileHandle();


    FileHandle(FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;

    //не
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;


    void close();

    // @return true, если файл открыт
     bool is_open() const noexcept { return fd_ != -1; }

    // Возвращает сырой файловый дескриптор
    int native_handle() const noexcept { return fd_; }

    // Возвращает путь к файлу
    const std::string& path() const noexcept { return path_; }

private:
    int fd_ = -1;
    std::string path_;
};

} // namespace lab4::resource