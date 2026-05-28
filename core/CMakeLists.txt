# 1. Интерфейсная библиотека для шаблонов (.hpp, .tpp)
# Шаблоны не компилируются сами по себе, они компилируются там, где используются.
add_library(seq_lib INTERFACE
        SequenceIO.hpp)

# Говорим CMake, что все, кто подключит seq_lib,
# должны искать заголовочные файлы в текущей папке (core)
target_include_directories(seq_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 2. Статическая библиотека для обычного C++ кода (.cpp)
# Сюда добавляем файлы, которые нужно физически скомпилировать
add_library(bit_lib STATIC
        BitSequence.cpp
)

# Связываем bit_lib и seq_lib.
# Ключевое слово PUBLIC означает: "bit_lib использует seq_lib,
# и любой, кто использует bit_lib, тоже получит доступ к seq_lib".
target_link_libraries(bit_lib PUBLIC seq_lib)