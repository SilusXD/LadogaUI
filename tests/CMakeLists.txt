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

add_executable(ladoga_events_test events_test.cpp)
target_link_libraries(ladoga_events_test PRIVATE Ladoga::ladoga)
if(COMMAND ladoga_copy_mingw_runtime)
    ladoga_copy_mingw_runtime(ladoga_events_test)
endif()

add_test(NAME ladoga.events COMMAND ladoga_events_test)
set_tests_properties(ladoga.events PROPERTIES LABELS "events")

add_executable(ladoga_font_test font_test.cpp)
target_link_libraries(ladoga_font_test PRIVATE Ladoga::ladoga)
target_include_directories(ladoga_font_test PRIVATE "${CMAKE_SOURCE_DIR}/src")
if(COMMAND ladoga_copy_mingw_runtime)
    ladoga_copy_mingw_runtime(ladoga_font_test)
endif()

add_test(NAME ladoga.font COMMAND ladoga_font_test)
set_tests_properties(ladoga.font PROPERTIES LABELS "font")

add_executable(ladoga_widgets_test widgets_test.cpp)
target_link_libraries(ladoga_widgets_test PRIVATE Ladoga::ladoga)
if(COMMAND ladoga_copy_mingw_runtime)
    ladoga_copy_mingw_runtime(ladoga_widgets_test)
endif()

add_test(NAME ladoga.widgets COMMAND ladoga_widgets_test)
set_tests_properties(ladoga.widgets PROPERTIES LABELS "widgets")
