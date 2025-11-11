#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

// Функция для запуска программы и получения вывода
std::string run_program(const std::filesystem::path &test_dir) {
  std::stringstream ss;
  ss << "../cpp-project-scanner " << test_dir;

  std::string command = ss.str();

  std::string result;
  char buffer[128];
  FILE *pipe = popen(command.c_str(), "r");
  if (!pipe)
    throw std::runtime_error("popen() failed!");

  try {
    while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
      result += buffer;
    }
  } catch (...) {
    pclose(pipe);
    throw;
  }
  pclose(pipe);
  return result;
}

TEST_CASE("Программа компилируется и запускается", "[basic]") {
  // Просто проверяем что программа запускается без падения
  std::string output = run_program(".");
  REQUIRE(!output.empty());
}

TEST_CASE("Анализ пустой директории", "[empty]") {
  std::string output = run_program("../../tests/test_data/empty_project");

  // Должны быть заголовки статистики, но 0 файлов
    REQUIRE(output.find("Total C++ files: 0") != std::string::npos);
    REQUIRE(output.find("0 bytes") != std::string::npos); // Должен быть 0 файлов
}

TEST_CASE("Анализ sample_project", "[sample]") {
  std::string output = run_program("../../tests/test_data/sample_project");

  // Проверяем основные метрики
  REQUIRE(output.find("C++") != std::string::npos);
  REQUIRE(output.find("files") != std::string::npos);
  REQUIRE(output.find("lines") != std::string::npos);
  REQUIRE(output.find("bytes") != std::string::npos);

  // Должно найти несколько C++ файлов
  REQUIRE(output.find("Total C++ files: 5") !=
          std::string::npos); // 5 .cpp/.h файлов
}

TEST_CASE("Анализ mixed_project", "[mixed]") {
  std::string output = run_program("../../tests/test_data/mixed_project");

  // Должен найти только C++ файлы, игнорируя .txt и .md
  bool hasCppFiles = (output.find("Total C++ files: 3") != std::string::npos);
  REQUIRE(hasCppFiles);
}

TEST_CASE("Проверка формата вывода", "[output_format]") {
  std::string output = run_program("../../tests/test_data/sample_project");

  // Проверяем что вывод содержит ключевые разделы
  REQUIRE(output.find("Total") != std::string::npos);
  REQUIRE(output.find("Largest") != std::string::npos);
  REQUIRE(output.find("Longest") != std::string::npos);

  // Проверяем что есть числовые значения
  bool hasNumbers = false;
  for (char c : output) {
    if (std::isdigit(c)) {
      hasNumbers = true;
      break;
    }
  }
  REQUIRE(hasNumbers);
}

TEST_CASE("Проверка граничных случаев", "[edge_cases]") {
  SECTION("Анализ текущей директории") {
    std::string output = run_program(".");
    REQUIRE(!output.empty());
  }

  SECTION("Анализ родительской директории") {
    std::string output = run_program("..");
    REQUIRE(!output.empty());
  }
}