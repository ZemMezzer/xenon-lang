#include <iostream>
#include "xenonstd.h"

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
    xenon::std::init(L);

    lua_close(L);
    return 0;
}
