cmake_minimum_required(VERSION 3.15)
project(doom_rl)

find_package(raylib 3.0 REQUIRED) # Requires at least version 3.0

set(CMAKE_C_STANDARD 99) # Requires C99 standard
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_EXTENSIONS False)

file(GLOB DOOM_SOURCE_FILES src/*.c )

include_directories( "${CMAKE_SOURCE_DIR}/header_libraries/" )

add_executable(${PROJECT_NAME} ${DOOM_SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} m)
target_link_libraries(${PROJECT_NAME} raylib)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
