#pragma once
#include <cstdio>
#include <memory>
#include <string> 

namespace lab4::resource
{

class FileHandle
{
  public:
    FileHandle(const std::string& path, const std::string& mode);

    bool is_open(); 

    void close();

  private:
    struct FileCloser
    {
        void operator()(std::FILE* f) const // оператор означает, что можно использовать как функцию
        {
            if (f) // проверка чтто указатель не нулевой
                std::fclose(f);
        }
    };
    std::unique_ptr<std::FILE, FileCloser> file_;
};

} // namespace lab4::resource