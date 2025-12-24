cmake_minimum_required(VERSION 3.10)
project(opengl_demo)

set(CMAKE_CXX_STANDARD 17)

add_executable(opengl_demo
    src/main.cpp
    src/glad.c
)

target_include_directories(opengl_demo PRIVATE include)
target_link_libraries(opengl_demo glfw GL)
