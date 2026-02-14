#pragma once

extern "C" {
#include <lua.h>
}

struct XString {
	char* string;
	size_t length;
};

extern "C" int luaopen_xstring(lua_State* L);