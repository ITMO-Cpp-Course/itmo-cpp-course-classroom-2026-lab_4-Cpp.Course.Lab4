#include "resource/FileHandle.hpp"
#include "resource/ResourceError.hpp"

namespace lab4::resource
{

void FileHandle::FileCloser::operator()(FILE* f) const
{
    if (f)
    {
        std::fclose(f);
    }
}

FileHandle::FileHandle() = default;

FileHandle::FileHandle(const std::string& path, const std::string& mode)
{
    open(path, mode);
}

FileHandle::FileHandle(FileHandle&& /*other*/) noexcept = default;

FileHandle& FileHandle::operator=(FileHandle&& /*other*/) noexcept = default;

FileHandle::~FileHandle() = default;

void FileHandle::open(const std::string& path, const std::string& mode)
{
    if (is_open())
    {
        throw ResourceError("File is already open");
    }
    FILE* raw = std::fopen(path.c_str(), mode.c_str());
    if (!raw)
    {
        throw ResourceError("Failed to open file: " + path);
    }
    file_.reset(raw);
    path_ = path;
    mode_ = mode;
}

void FileHandle::close()
{
    if (!is_open())
    {
        throw ResourceError("File is not open");
    }
    file_.reset();
    mode_.clear();
}

bool FileHandle::is_open() const
{
    return file_ != nullptr;
}

const std::string& FileHandle::path() const
{
    return path_;
}

void FileHandle::write(const std::string& data)
{
    if (!file_)
    {
        throw ResourceError("File is not open");
    }
    if (mode_ == "r")
    {
        throw ResourceError("File opened for reading only");
    }
    if (std::fputs(data.c_str(), file_.get()) == EOF)
    {
        throw ResourceError("Failed to write to file");
    }
}

std::string FileHandle::read_all()
{
    if (!file_)
    {
        throw ResourceError("File is not open");
    }
    std::fseek(file_.get(), 0, SEEK_END);
    long size = std::ftell(file_.get());
    std::fseek(file_.get(), 0, SEEK_SET);
    std::string result(size, '\0');
    std::fread(result.data(), 1, size, file_.get());
    return result;
}

FILE* FileHandle::get() const
{
    return file_.get();
}

} // namespace lab4::resource
