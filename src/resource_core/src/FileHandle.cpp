#include "resource/FileHandle.hpp"

namespace lab4::resource {

void FileHandle::FileCloser::operator()(FILE* f) const {
    if (f) {
        std::fclose(f);
    }
}

FileHandle::FileHandle() = default;

FileHandle::FileHandle(const std::string& /*path*/, const std::string& /*mode*/) {}

FileHandle::FileHandle(FileHandle&& /*other*/) noexcept = default;

FileHandle& FileHandle::operator=(FileHandle&& /*other*/) noexcept = default;

FileHandle::~FileHandle() = default;

} // namespace lab4::resource
