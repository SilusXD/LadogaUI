# Бинарники тестов едут в <build>/bin.

add_executable(ladoga_smoke_test smoke_test.cpp)
target_link_libraries(ladoga_smoke_test PRIVATE Ladoga::ladoga)
if(COMMAND ladoga_copy_mingw_runtime)
    ladoga_copy_mingw_runtime(ladoga_smoke_test)
endif()

add_test(NAME ladoga.smoke COMMAND ladoga_smoke_test)
set_tests_properties(ladoga.smoke PROPERTIES LABELS "smoke")

add_executable(ladoga_layout_test layout_test.cpp)
target_link_libraries(ladoga_layout_test PRIVATE Ladoga::ladoga)
if(COMMAND ladoga_copy_mingw_runtime)
    ladoga_copy_mingw_runtime(ladoga_layout_test)
endif()

add_test(NAME ladoga.layout COMMAND ladoga_layout_test)
set_tests_properties(ladoga.layout PROPERTIES LABELS "layout")

add_executable(ladoga_xml_test xml_test.cpp)
target_link_libraries(ladoga_xml_test PRIVATE Ladoga::ladoga)
if(COMMAND ladoga_copy_mingw_runtime)
    ladoga_copy_mingw_runtime(ladoga_xml_test)
endif()

add_test(NAME ladoga.xml COMMAND ladoga_xml_test)
set_tests_properties(ladoga.xml PROPERTIES LABELS "xml")
