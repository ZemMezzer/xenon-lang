project(xenon-lang)
cmake_minimum_required(VERSION 3.29)

add_subdirectory(core)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC xenon-core)