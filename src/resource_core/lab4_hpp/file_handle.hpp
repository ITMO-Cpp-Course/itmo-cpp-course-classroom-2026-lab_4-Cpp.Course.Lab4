#pragma once
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace lab4::resource
{
class FileHandle
{
  public:
    FileHandle() = default;
    explicit FileHandle(const std::string FileName, std::ios::openmode mode = std::ios::in) {};
    ~FileHandle();
    FileHandle(const FileHandle& other) = delete;
    FileHandle operator==(const FileHandle& other) = delete;
    FileHandle(FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;

    void Open(const std::string& FileName_, std::ios::openmode mode = std::ios::in);
    bool IsOpen() const;
    void Close();
    void EnsureOpen() const;

    std::string ReadLine();
    void WriteLine(const std::string& line);
    void Write(const std::string& data);
    std::string ReadAll();
    const std::string& GetFileName() const;

  private:
    std::string FileName;
    std::fstream File;
    bool openned;
};

} // namespace lab4::resource