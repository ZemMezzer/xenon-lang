#pragma once

#include <string>

extern "C" {
#include <lua.h>
}

struct XString {
	char* string;
	size_t length;

	operator std::string() const {
		return std::string(string, length);
	}

	std::string to_std_string() const {
		return std::string(string, length);
	}
};

XString* xstring_check(lua_State* L, int index);
bool is_xstring(lua_State* L, int index);
std::string xstring_to_std_string(lua_State* L, int idx);
void xstring_push(lua_State* L, const char* str, size_t length);

extern "C" int xenon_openlib_xstring(lua_State* L);