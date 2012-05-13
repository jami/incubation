include_directories(${CMAKE_SOURCE_DIR}/src/include/program ${CMAKE_SOURCE_DIR}/src/include/lib)

add_executable(program
    main.c
    intro_state.c
    menu_state.c
)

SET_TARGET_PROPERTIES(program PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_RUNTIME_OUTPUT_DIRECTORY}")

target_link_libraries(program
    tdengine
#    input
#    window
#    system
#    graphic
#    gui
#    storage
)

install_programs(/bin FILES program)
