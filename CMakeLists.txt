# Name the project
PROJECT(Incubation)

# -------------------------

# A warning appeared because minimum version must be
# explicited.
# The warning told me to add this :
cmake_minimum_required(VERSION 2.6)

# A warning appeared because cmake must look for cppunit.
# The warning told me to add this :
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

SET(COMMON_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
SET(COMMON_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/libs")
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmakescripts)

# build type
SET(CMAKE_BUILD_TYPE Debug)

if(NOT WIN32)
  set(COMPILE_FLAGS "-g -ansi -pedantic -Wall -Werror")
endif(NOT WIN32)

# ZLib
FIND_PACKAGE(ZLIB REQUIRED)

# SDL
FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(SDL_image REQUIRED)

# Workaround for the non-working REQUIRED flag
if (NOT SDL_FOUND)
   message(FATAL_ERROR "SDL not found!")
endif(NOT SDL_FOUND)

# opengl
FIND_PACKAGE(OpenGL REQUIRED)

# cunit
include(FindCUnit)
find_package(CUnit REQUIRED)
if (NOT CUNIT_FOUND)
    message(FATAL_ERROR "CUnit not found!")
endif(NOT CUNIT_FOUND)

link_libraries(
    ${SDL_LIBRARY}
    ${SDLIMAGE_LIBRARY} # if using SDL_image, obviously
    SDLmain # Sadly not included in SDL_LIBRARY variable
    ${OPENGL_LIBRARY}
    ${CUNIT_LIBRARIES}
    ${ZLIB_LIBRARIES}
)

include_directories(
    ${SDL_INCLUDE_DIRS}
    ${CUNIT_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIR}
)

# -------------------------
# Set data mounting node

if (DEFINED DATAPATH)
  add_definitions(-DDATAPATH=${DATAPATH})
endif (DEFINED DATAPATH)

# -------------------------

# Add config.h to the include folder
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/include/lib/config.h)

# Tell CMake to process the sub-directories
add_subdirectory(src)
add_subdirectory(tests)

# -------------------------


