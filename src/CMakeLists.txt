cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(xenon)

add_subdirectory(core)

add_executable(${PROJECT_NAME} 
main.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC xenon-core)