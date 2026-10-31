cmake_minimum_required(VERSION 3.24)

project(LadogaUI
    VERSION 0.1.0
    DESCRIPTION "Учебный C++ GUI-фреймворк с XML-разметкой"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

option(LADOGA_BUILD_EXAMPLES "Собрать примеры LadogaUI" ON)
option(LADOGA_BUILD_TESTS "Собрать тесты LadogaUI" ON)

# Пока это только каркас: ядро, рендерер и XML-парсер появятся на следующих этапах.
# Публичные заголовки лежат в include/ladoga, реализация — в src.
add_library(ladoga STATIC
    src/version.cpp
)

add_library(Ladoga::ladoga ALIAS ladoga)

target_include_directories(ladoga
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(ladoga PUBLIC cxx_std_20)
target_compile_definitions(ladoga PRIVATE LADOGA_VERSION_STRING="${PROJECT_VERSION}")

if(MSVC)
    target_compile_options(ladoga PRIVATE /W4 /permissive-)
else()
    target_compile_options(ladoga PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(LADOGA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(LADOGA_BUILD_TESTS AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
