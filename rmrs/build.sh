#!/bin/bash

set -e

echo "=== Сборка проекта ==="
cmake -B build -S .
cmake --build build --parallel

echo ""
echo "=== Запуск всех тестов ==="
cd build
ctest --output-on-failure -V

echo ""
echo "=== Интеграционное тестирование ==="
echo "Тест 1: Анализ sample_project"
./cpp-project-scanner ../tests/test_data/sample_project

echo ""
echo "Тест 2: Анализ mixed_project"  
./cpp-project-scanner ../tests/test_data/mixed_project

echo ""
echo "Тест 3: Анализ пустой директории"
./cpp-project-scanner ../tests/test_data/empty_project

echo ""
echo "Тест 4: Анализ текущей директории"
./cpp-project-scanner .