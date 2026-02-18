project(xenon-std-lib)

add_library(${PROJECT_NAME} STATIC
internal/stack_helper.cpp
internal/preprocessor.cpp

console.cpp
runtime.cpp
xfile.cpp
xstring.cpp
registry.cpp
xdirectory.cpp
xerror.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC xenon-lua)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/internal)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})