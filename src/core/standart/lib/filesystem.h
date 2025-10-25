#pragma once

#include <string>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}


std::string lua_get_absolute_path(const std::string& relative_path);
std::string lua_get_directory_path(const std::string& path);
std::string lua_get_home_directory();

void lua_set_home_path(const std::string& home_path);
extern "C" int luaopen_filesystem(lua_State* L);