#include <catch2/catch_all.hpp>
#include <lab4/resource/ResourceError.hpp>
#include <lab4/resource/ResourceManager.hpp>
#include <lab4/resource/FileHandler.hpp>
#include <fstream>
#include <cstdio>

class TestFile
{
  public:
    TestFile(std::string filepath_, std::string content = "") : filepath{filepath_}
    {
        std::ofstream(filepath) << content;
    }

    ~TestFile()
    {
        std::remove(filepath.c_str());
    }

  private:
    std::string filepath;
};