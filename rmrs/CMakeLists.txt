cmake_minimum_required(VERSION 3.20)
project(
  CppProjectScanner
  VERSION 1.0
  LANGUAGES CXX)

# Требуем современный C++ стандарт
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Включить автоматическое определение исходных файлов в src/
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.cc" "src/*.cxx")
file(GLOB_RECURSE HEADERS "src/*.h" "src/*.hh" "src/*.hpp" "src/*.hxx")

# Основная программа студента
add_executable(cpp-project-scanner ${SOURCES} ${HEADERS})
target_compile_options(cpp-project-scanner PRIVATE "-std=c++20")

include(CTest)
enable_testing()
# Подключение тестов
add_subdirectory(tests)
