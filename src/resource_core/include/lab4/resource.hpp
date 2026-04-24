#ifndef LAB_4_AXIS418_RESOURCE_HPP
#define LAB_4_AXIS418_RESOURCE_HPP

#include <cstdio>
#include <map>
#include <memory>
#include <string>

namespace lab4::resource
{

class ResourceError : public std::exception
{
  public:
    explicit ResourceError(const std::string& msg);
    const char* what() const noexcept override;

  private:
    std::string message;
};

class FileHandle
{
  public:
    explicit FileHandle(const std::string& filename, const std::string& mode = "r");

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle(FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;

    ~FileHandle();

    bool is_open() const noexcept;

    std::string name() const;

    std::string read_line();

    void write_line(const std::string& data);

    void close();

  private:
    std::string mode_;
    std::string filename_;
    std::FILE* file_;
    bool can_write() const
    {

        return mode_.find('w') != std::string::npos || mode_.find('a') != std::string::npos ||
               mode_.find('+') != std::string::npos;
    }
};

class ResourceManager
{
  public:
    std::shared_ptr<FileHandle> get(const std::string& filename, const std::string& mode = "r");

  private:
    std::map<std::string, std::weak_ptr<FileHandle>> cache_;
};

} // namespace lab4::resource

#endif
