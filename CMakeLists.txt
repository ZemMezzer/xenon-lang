cmake_minimum_required(VERSION 3.29.0)

project(xenon-lang)

add_subdirectory(Xenon/Interpreter)

add_executable(${PROJECT_NAME} Xenon/InterpreterTest/InterpreterTest.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE xenon)