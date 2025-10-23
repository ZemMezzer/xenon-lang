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

    luaL_dostring(L, "local p = ptr.new(1)\n p:set_byte(0, 1)\n print(p:get_byte(0))\n local p2 = ptr.from(p) \n print(p2:get_byte(0))");

    lua_close(L);
    return 0;
}
