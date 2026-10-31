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

include(cmake/Dependencies.cmake)

# Все бинарники — в build/bin, чтобы путь запуска не зависел от папки исходника.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Запуск exe без CLion: не тащим libstdc++-6.dll и libgcc_s_seh-1.dll.
# libwinpthread-1.dll остаётся динамической — её копируем рядом с exe.
if(MINGW)
    add_link_options(-static-libgcc -static-libstdc++)
    get_filename_component(LADOGA_MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

    function(ladoga_copy_mingw_runtime target)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${LADOGA_MINGW_BIN_DIR}/libwinpthread-1.dll"
                "$<TARGET_FILE_DIR:${target}>"
            VERBATIM
        )
    endfunction()
endif()

# Пока это только каркас: виджеты и XML появятся на следующих этапах.
# Публичные заголовки лежат в include/ladoga, реализация — в src.
add_library(ladoga STATIC
    src/version.cpp
    src/widget.cpp
    src/stack_panel.cpp
    src/label.cpp
    src/button.cpp
    src/text_box.cpp
    src/check_box.cpp
    src/slider.cpp
    src/window.cpp
    src/app/application.cpp
    src/render/sdl3_backend.cpp
    src/render/opengl_renderer.cpp
    src/render/font.cpp
    src/xml/widget_factory.cpp
    src/xml/property_map.cpp
    src/xml/xml_loader.cpp
)

add_library(Ladoga::ladoga ALIAS ladoga)

target_include_directories(ladoga
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

target_compile_features(ladoga PUBLIC cxx_std_20)
target_compile_definitions(ladoga PRIVATE
    LADOGA_VERSION_STRING="${PROJECT_VERSION}"
    LADOGA_FONT_PATH="${CMAKE_CURRENT_SOURCE_DIR}/third_party/Roboto-Regular.ttf"
)
target_link_libraries(ladoga PRIVATE SDL3::SDL3 OpenGL::GL pugixml::pugixml)

# Шрифт рядом с exe: пригодится, если проект запускают не из дерева исходников.
add_custom_target(ladoga_fonts ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/fonts"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/Roboto-Regular.ttf"
        "${CMAKE_BINARY_DIR}/bin/fonts/Roboto-Regular.ttf"
    COMMENT "Копирование шрифта LadogaUI"
)

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
