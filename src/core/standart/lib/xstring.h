#pragma once

#include <string>

extern "C" {
#include <lua.h>
}

struct XString {
	char* string;
	size_t length;
};

XString* xstring_check(lua_State* L, int index);
std::string xstring_to_std_string(lua_State* L, int idx);

extern "C" int xenon_openlib_xstring(lua_State* L);