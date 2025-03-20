project(xenon-libs)

set(XENON_LIBS
src/Lib.cpp
src/StandartLib.cpp)

add_library(${PROJECT_NAME} STATIC ${XENON_LIBS})

target_link_libraries(${PROJECT_NAME} PUBLIC xenon-core)

target_include_directories(${PROJECT_NAME} PUBLIC include)