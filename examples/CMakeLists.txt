# Бинарники всех примеров едут в <build>/bin (задано в корневом CMakeLists.txt).
# Запуск: cmake-build-debug/bin/ladoga_00_smoke.exe.

add_executable(ladoga_00_smoke 00_smoke/main.cpp)
target_link_libraries(ladoga_00_smoke PRIVATE Ladoga::ladoga)
