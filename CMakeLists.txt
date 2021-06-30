cmake_minimum_required(VERSION 3.15)
project(sed)

find_package(raylib 3.0 REQUIRED) # Requires at least version 3.0

set(CMAKE_CXX_STANDARD 11) # Requires C++11 standard

add_executable(${PROJECT_NAME} src/main.cpp src/render.cpp)

target_link_libraries(${PROJECT_NAME} raylib)

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
