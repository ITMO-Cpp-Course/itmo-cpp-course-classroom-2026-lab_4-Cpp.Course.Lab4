#pragma once
#include <string>

namespace lab4::resource
{
class FileHandle
{
  private:
    std::FILE* handle = nullptr;

  public:
    explicit FileHandle(const std::string& path, const std::string& mode = "r");
    // Устанавливаем режим по умолчанию 'r'ead - чтение. в работе с файлами нам может пригодиться и write/append,
    // что очень удобно. в данной лабе мы их только читаем
    ~FileHandle();

    // Запрет копирования
    // копирование привело бы к тому, что два деструктора попытались бы закрыть один и тот же файл - двойное
    // освобождение памяти
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    // Передача владения позволяет в отсутствии копирования эффективно передавать файлы между частями программы.
    // noexcept гарантирует, что функция завершится успешно или вернет управление
    // без вылета программы: перемещение указателя не может вызвать ошибку
    FileHandle(FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;

    std::FILE* get() const
    {
        return handle;
    }
    bool is_valid() const
    {
        return handle != nullptr;
    }
};

} // namespace lab4::resource