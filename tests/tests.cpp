#include "../src/resource_core/resource_error.hpp"
#include "../src/resource_core/resource_manager.hpp"
#include <iostream>

using namespace lab4::resource;

int main()
{

    try
    {
        ResourceManager manager;
        std::cout << "--- Тест 1: Успешное открытие ---" << std::endl;

        FileHandle& file = manager.GetFile("test.txt", "w");

        std::fprintf(file.Get(), "Hello, RAII world!\n");
        std::cout << "Данные успешно записаны в test.txt" << std::endl;

        std::cout << "\n--- Тест 2: Повторное получение ---" << std::endl;
        if (manager.IsOpened("test.txt"))
        {
            std::cout << "Менеджер подтверждает: файл test.txt уже открыт." << std::endl;
        }

        std::cout << "\n--- Тест 3: Обработка ошибки ---" << std::endl;
        manager.GetFile("non_existent_file.txt", "r");
    }
    catch (const ResourceError& e)
    {

        std::cerr << "Поймали исключение: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Неизвестная ошибка: " << e.what() << std::endl;
    }

    std::cout << "\n--- Тест 4: Завершение программы ---" << std::endl;
    std::cout << "Сейчас менеджер выйдет из области видимости и сам закроет все файлы." << std::endl;

    return 0;
}