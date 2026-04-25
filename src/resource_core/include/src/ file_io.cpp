#include "resource_core/file_io.hpp"
#include "resource_core/resource_error.hpp"
#include <unistd.h>
#include <cerrno>
#include <vector>

namespace lab4::resource {

void write_all(FileHandle& fh, const std::string& data) {
    if (!fh.is_open()) {
        throw ResourceError("Cannot write: file is not open");
    }
    const char* ptr = data.data();
    std::size_t remaining = data.size();
    while (remaining > 0) {
        ssize_t written = ::write(fh.native_handle(), ptr, remaining);
        if (written == -1) {
            if (errno == EINTR) continue;
            throw ResourceError("Write error on '" + fh.path() + "'", errno);
        }
        ptr += written;
        remaining -= static_cast<std::size_t>(written);
    }
}

std::string read_all(FileHandle& fh) {
    if (!fh.is_open()) {
        throw ResourceError("Cannot read: file is not open");
    }
    std::string result;
    std::vector<char> buffer(4096);
    while (true) {
        ssize_t n = ::read(fh.native_handle(), buffer.data(), buffer.size());
        if (n == -1) {
            if (errno == EINTR) continue;
            throw ResourceError("Read error on '" + fh.path() + "'", errno);
        }
        if (n == 0) break; // EOF
        result.append(buffer.data(), static_cast<std::size_t>(n));
    }
    return result;
}

} // namespace lab4::resource