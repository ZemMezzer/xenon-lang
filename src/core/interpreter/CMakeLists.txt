project(xenon-interpreter)

add_library(${PROJECT_NAME} STATIC 
sources/arg.cpp
sources/interpreter.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC 
xenon-lua
xenon-std-lib
xenon-std
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)