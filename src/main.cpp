#include "xenonstd.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

int main() {
    lua_State* L = luaL_newstate();

    xenon::std::init(L);

    lua_close(L);
    return 0;
}
