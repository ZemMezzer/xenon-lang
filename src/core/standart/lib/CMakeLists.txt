project(xenon-std-lib)

add_library(${PROJECT_NAME} STATIC 
internal/stack_helper.cpp
internal/preprocessor.cpp
internal/xerror.hpp
type.cpp
pointer.cpp
safe_pointer.cpp
console.cpp
runtime.cpp
filesystem.cpp
xstring.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC xenon-lua)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/internal)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})