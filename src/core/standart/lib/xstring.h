extern "C" {
#include <lua.h>
}

struct XString
{
	char* string;

};


extern "C" int luaopen_xstring(lua_State* L);