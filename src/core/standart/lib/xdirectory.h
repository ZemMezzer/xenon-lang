#pragma once

extern "C" {
	#include "lauxlib.h"
	#include "lua.h"
}

#include <string>

static std::string home_path;

std::string xenon_make_absolute_path(lua_State* L, const std::string& path);
std::string xenon_get_absolute_path(const std::string& relative_path);
std::string xenon_get_directory_path(const std::string& path);
std::string xenon_get_home_directory(lua_State* L);

void xenon_set_home_path(lua_State* L, const std::string& hpath);

extern "C" int xenon_openlib_directory(lua_State* L);