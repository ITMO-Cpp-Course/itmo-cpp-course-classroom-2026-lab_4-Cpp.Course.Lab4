#pragma once
#include <exception> // для what(), в базовом классе он виртуальный
#include <string>

namespace lab4::resource
{

class ResourceError : public std::exception // наследуемся от стандартного класса исключений
{
  public:
    explicit ResourceError(const std::string& message)
        : msg_(message) {} // explicit запрещает неявное преобразование из std::string в ResourceError с помощью
                           // конструктора чтобы не было неожиданного поведения.

    const char* what()
        const noexcept override // без переопределения мы бы получили неинформативную строку базового класса
    {
        return msg_.c_str();
    }

  private:
    std::string msg_;
};

} // namespace lab4::resource