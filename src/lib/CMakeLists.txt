
include_directories(${CMAKE_SOURCE_DIR}/src/include/lib/)

ADD_LIBRARY(tdengine SHARED
    log.c
    event.c
    configuration.c
    graphic.c
    state.c
    timer.c
    image.c
    json.c
    ressource.c
)

SET_TARGET_PROPERTIES(tdengine PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${COMMON_LIBRARY_OUTPUT_DIRECTORY}")

# make sure the library gets installed
install_targets(/lib tdengine)