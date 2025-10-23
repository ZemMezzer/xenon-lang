project(xenon-std-lib)

add_library(${PROJECT_NAME} STATIC 
pointer.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC xenon-lua)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})