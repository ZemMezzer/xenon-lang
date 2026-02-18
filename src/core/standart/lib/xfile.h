#pragma once

#include <string>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

bool xenon_file_exists(const std::string& path);
extern "C" int xenon_openlib_file(lua_State* L);