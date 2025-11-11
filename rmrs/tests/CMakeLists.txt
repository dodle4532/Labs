# Поиск Catch2
find_package(Catch2 REQUIRED)


# Файлы тестов
set(TEST_SOURCES
    test_main.cpp
)
# Создаем тестовый исполняемый файл
add_executable(run_tests ${TEST_SOURCES})

# Линкуем с основной программой и Catch2
target_link_libraries(run_tests PRIVATE Catch2::Catch2WithMain)

# Добавляем тест в CTest
include(Catch)
catch_discover_tests(run_tests)