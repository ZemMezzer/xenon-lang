#pragma once

extern "C" {
#include <lua.h>
}

struct XString {
	char* string;
	int length;
};

extern "C" int luaopen_xstring(lua_State* L);