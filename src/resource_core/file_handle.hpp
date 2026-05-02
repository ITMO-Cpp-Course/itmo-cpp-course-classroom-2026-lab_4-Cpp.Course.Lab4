#ifndef LAB_4_OVCINNIKOVIVAN86_FILE_HANDLE_HPP
#define LAB_4_OVCINNIKOVIVAN86_FILE_HANDLE_HPP
#include <string>
namespace lab4::resource
{
class FileHandle
{
  private:
    std::FILE* file_ptr;

  public:
    FileHandle(const std::string& path, const std::string& mode);
    ~FileHandle();
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
    FileHandle(FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;
    [[nodiscard]] std::FILE* Get() const;
};
} // namespace lab4::resource
#endif // LAB_4_OVCINNIKOVIVAN86_FILE_HANDLE_HPP
