project(xenon-lua)

set(LUA_BIN ${CMAKE_CURRENT_SOURCE_DIR}/liblua.a)

add_custom_command(OUTPUT ${LUA_BIN}
        COMMAND make a
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PRE_BUILD
        COMMENT "Building LUA")

add_custom_target(xenon-lua-target ALL DEPENDS ${LUA_BIN})

add_library(${PROJECT_NAME} INTERFACE)

target_link_libraries(${PROJECT_NAME} INTERFACE ${LUA_BIN})