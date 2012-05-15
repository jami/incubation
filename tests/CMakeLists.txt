include_directories(${CMAKE_SOURCE_DIR}/src/include/program ${CMAKE_SOURCE_DIR}/src/include/lib)

enable_testing()

add_executable(test_lib 
    test_lib.c
    event_manager_test.c
    configuration_test.c
    state_test.c
    json_test.c
    ressource_test.c
    image_test.c
    gui_test.c
)

set_target_properties(test_lib
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_RUNTIME_OUTPUT_DIRECTORY}"
)

target_link_libraries(test_lib
    tdengine
)

set_target_properties(test_lib PROPERTIES LINK_FLAGS -Wl,-lcunit)

add_test(test_lib ${EXECUTABLE_OUTPUT_PATH}/test_lib)