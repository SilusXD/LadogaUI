# Бинарники тестов едут в <build>/bin (задано в корневом CMakeLists.txt).

add_executable(ladoga_smoke_test smoke_test.cpp)
target_link_libraries(ladoga_smoke_test PRIVATE Ladoga::ladoga)

add_test(NAME ladoga.smoke COMMAND ladoga_smoke_test)
set_tests_properties(ladoga.smoke PROPERTIES LABELS "smoke")
