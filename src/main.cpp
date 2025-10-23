#include <iostream>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

int main() {
    lua_State* L = luaL_newstate();
    if (!L) {
        std::cerr << "Unable to create lua state!\n";
        return 1;
    }

    luaL_openlibs(L);

    if (luaL_dostring(L, "print('Hello from Lua!')") != LUA_OK) {
        std::cerr << lua_tostring(L, -1) << "\n";
        lua_close(L);
        return 1;
    }

    lua_close(L);
    return 0;
}
