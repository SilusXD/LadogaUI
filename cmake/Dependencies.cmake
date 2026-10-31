# Зависимости этапа 1.
# SDL3 качается CMake-ом и собирается статически. На Linux можно заранее
# поставить пакет SDL3 и включить LADOGA_USE_SYSTEM_SDL3=ON.

include(FetchContent)

option(LADOGA_USE_SYSTEM_SDL3 "Использовать системный SDL3 вместо FetchContent" OFF)

if(LADOGA_USE_SYSTEM_SDL3)
    find_package(SDL3 CONFIG REQUIRED)
else()
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SDL_INSTALL OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.18
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(SDL3)
endif()

find_package(OpenGL REQUIRED)

# XML-парсер: pugixml 1.16 (MIT).
option(LADOGA_USE_SYSTEM_PUGIXML "Использовать системный pugixml вместо FetchContent" OFF)

if(LADOGA_USE_SYSTEM_PUGIXML)
    find_package(pugixml CONFIG REQUIRED)
else()
    set(PUGIXML_BUILD_TESTS OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(pugixml
        GIT_REPOSITORY https://github.com/zeux/pugixml.git
        GIT_TAG v1.16
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(pugixml)
endif()
