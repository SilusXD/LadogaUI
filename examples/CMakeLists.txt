# Бинарники всех примеров едут в <build>/bin.

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

add_executable(ladoga_02_layout 02_layout/main.cpp)
target_link_libraries(ladoga_02_layout PRIVATE Ladoga::ladoga)
if(COMMAND ladoga_copy_mingw_runtime)
    ladoga_copy_mingw_runtime(ladoga_02_layout)
endif()

add_executable(ladoga_03_xml 03_xml/main.cpp)
target_link_libraries(ladoga_03_xml PRIVATE Ladoga::ladoga)
target_compile_definitions(ladoga_03_xml PRIVATE
    LADOGA_EXAMPLE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/03_xml"
)
add_custom_command(TARGET ladoga_03_xml POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/03_xml/main.xml"
        "$<TARGET_FILE_DIR:ladoga_03_xml>"
)
if(COMMAND ladoga_copy_mingw_runtime)
    ladoga_copy_mingw_runtime(ladoga_03_xml)
endif()
