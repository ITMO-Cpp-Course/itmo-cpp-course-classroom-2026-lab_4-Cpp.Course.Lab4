#pragma once

#include <stdexcept>
#include <string>

namespace lab4::resource {

// Исключение, сигнализирующее об ошибке при работе с ресурсом.
class ResourceError : public std::runtime_error {
public:
    //message описание ошибки
    //code код ошибки (обычно errno)
    explicit ResourceError(const std::string& message, int code = 0);

    // Возвращает сохранённый код ошибки.
    int code() const noexcept { return code_; }

private:
    int code_;
};

} // namespace lab4::resource