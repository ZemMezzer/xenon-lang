#include "xstring.h"
#include "stack_helper.h"
#include <string.h>

extern "C" {
#include "lauxlib.h"
}

static const char* lib_name = "string";

static int l_xstr(lua_State* L) {
	
	size_t length = 0;
	const char* s = luaL_checklstring(L, 1, &length);

	XString* xs = (XString*)lua_newuserdata(L, sizeof(XString) + length + 1);
	xs->length = length;
	xs->string = (char*)(xs + 1);
	memcpy(xs->string, s, length);
	xs->string[length] = '\0';

	luaL_getmetatable(L, lib_name);
	lua_setmetatable(L, -2);
	return 1;
}

static int to_string(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	lua_pushlstring(L, xs->string, xs->length);
	return 1;
}

static int length(lua_State* L) {
	XString* xs = (XString*)luaL_checkudata(L, 1, lib_name);
	lua_pushinteger(L, xs->length);
	return 1;
}

static int equals(lua_State* L) {
	XString* xs1 = (XString*)luaL_checkudata(L, 1, lib_name);
	XString* xs2 = (XString*)luaL_checkudata(L, 2, lib_name);
	if (xs1->length != xs2->length) {
		lua_pushboolean(L, 0);
		return 1;
	}
	int cmp = memcmp(xs1->string, xs2->string, xs1->length);
	lua_pushboolean(L, cmp == 0);
	return 1;
}

extern "C" int luaopen_xstring(lua_State* L) {

	if (luaL_newmetatable(L, lib_name)) {
		lua_pushcfunction(L, to_string);
		lua_setfield(L, -2, "__tostring");

		lua_pushcfunction(L, length);
		lua_setfield(L, -2, "__len");

		lua_pushcfunction(L, equals);
		lua_setfield(L, -2, "__eq");
	}
	lua_pop(L, 1);

	lua_pushcfunction(L, l_xstr);
	lua_setglobal(L, "_xstr");

	return 1;
}