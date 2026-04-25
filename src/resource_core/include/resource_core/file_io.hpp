#pragma once

#include "resource_core/file_handle.hpp"
#include <string>

namespace lab4::resource {

// Записывает строку в файл. Файл должен быть открыт для записи.
//@throw ResourceError при ошибке.
void write_all(FileHandle& fh, const std::string& data);

// Читает всё содержимое файла с текущей позиции до конца.
// @throw ResourceError при ошибке.
std::string read_all(FileHandle& fh);

} // namespace lab4::resource