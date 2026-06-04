cmake_minimum_required(VERSION 3.14)
project(LazySequence_Lab CXX)

# Задаем стандарт C++17 (оптимально для Google Test и современного C++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================================================
# 1. СБОРКА ЯДРА ПРОЕКТА
# =========================================================

# Интерфейсная библиотека для шаблонов (.hpp, .tpp)
# Шаблоны не компилируются сами по себе, они компилируются там, где используются.
add_library(seq_lib INTERFACE)

# Говорим CMake, что все, кто подключит seq_lib,
# должны искать заголовочные файлы от корня проекта
target_include_directories(seq_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Статическая библиотека для обычного C++ кода (.cpp)
# Внимание: укажи правильный путь до BitSequence.cpp (скорее всего он в папке core)
add_library(bit_lib STATIC
        core/BitSequence.cpp
)

# Связываем bit_lib и seq_lib
target_link_libraries(bit_lib PUBLIC seq_lib)


# =========================================================
# 2. ИНТЕГРАЦИЯ GOOGLE TEST (FetchContent)
# =========================================================

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
# Фикс для Windows (MSVC/MinGW), чтобы избежать конфликтов Runtime Library
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


# =========================================================
# 3. СБОРКА ТЕСТОВ (Бизнес-логика)
# =========================================================

# Включаем поддержку тестирования в CMake (команда ctest)
enable_testing()

# Создаем исполняемый файл для тестов, перечисляем все 7 файлов
add_executable(run_tests
        tests/TestOrdinal.cpp
        tests/TestFunctionGenerator.cpp
        tests/TestSmartDataGenerator.cpp
        tests/TestDecorators.cpp
        tests/TestLazySequence.cpp
        tests/TestSequenceStreams.cpp
        tests/TestLazyStreams.cpp
)

# Линкуем тесты с нашей библиотекой и с Google Test.
# Использование GTest::gtest_main избавляет от необходимости писать int main() в тестах
target_link_libraries(run_tests
        PRIVATE
        bit_lib
        GTest::gtest_main
)

# Автоматически обнаруживаем тесты для CTest и CLion
include(GoogleTest)
gtest_discover_tests(run_tests)


# =========================================================
# 4. СБОРКА ПОЛЬЗОВАТЕЛЬСКОГО ИНТЕРФЕЙСА (REPL APP)
# =========================================================

# Создаем главное приложение из точки входа и класса UI
add_executable(app
        main.cpp
        ui/ConsoleUI.cpp
)

# Линкуем приложение с библиотекой ядра
target_link_libraries(app PRIVATE bit_lib)