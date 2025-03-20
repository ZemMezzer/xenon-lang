project(xenon)

add_subdirectory(core)
add_subdirectory(src/libs)

add_library(${PROJECT_NAME} STATIC Interpreter.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC xenon-core)
target_link_libraries(${PROJECT_NAME} PUBLIC xenon-libs)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})