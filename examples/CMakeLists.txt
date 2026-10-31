# Бинарники всех примеров едут в <build>/bin (задано в корневом CMakeLists.txt).

add_executable(ladoga_00_smoke 00_smoke/main.cpp)
target_link_libraries(ladoga_00_smoke PRIVATE Ladoga::ladoga)
if(COMMAND ladoga_copy_mingw_runtime)
    ladoga_copy_mingw_runtime(ladoga_00_smoke)
endif()

add_executable(ladoga_01_window 01_window/main.cpp)
target_link_libraries(ladoga_01_window PRIVATE Ladoga::ladoga)
if(COMMAND ladoga_copy_mingw_runtime)
    ladoga_copy_mingw_runtime(ladoga_01_window)
endif()
