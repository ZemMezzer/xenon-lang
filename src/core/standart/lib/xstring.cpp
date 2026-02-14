#include "xstring.h"

int l_xstr(lua_State* L) {
    return 0;
}


extern "C" int luaopen_xstring(lua_State* L) {
	lua_pushcfunction(L, l_xstr);
	lua_setglobal(L, "_xstr");

	return 0;
}