#pragma once

#include <memory>
#include <string>

namespace lab4::resource
{

class FileHandler
{
  public:
    FileHandler(std::string filepath_);
    ~FileHandler() noexcept;

    FileHandler(const FileHandler&) = delete;
    FileHandler& operator=(const FileHandler&) = delete;

    FileHandler(FileHandler&& other) noexcept;
    FileHandler& operator=(FileHandler&& other) noexcept;

    void open(const std::string& filepath_);
    void close();

  private:
    std::FILE* file;
    std::string filepath;

    void openFile(const std::string& filepath_);
    void closeFile() noexcept;
};

} // namespace lab4::resource