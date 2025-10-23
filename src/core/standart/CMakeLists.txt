project(xenon-std)

add_subdirectory(lib)

add_library(${PROJECT_NAME} STATIC 
xenonstd.cpp 
)

target_link_libraries(${PROJECT_NAME} PUBLIC 
xenon-lua
xenon-std-lib
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})