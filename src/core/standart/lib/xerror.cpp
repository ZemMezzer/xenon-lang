#include "xerror.h"
#include <iostream>

void xenon_raise(lua_State* L, int code) {
    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "code");
        bool hasCode = lua_isinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "message");
        bool hasMsg = lua_isstring(L, -1);
        lua_pop(L, 1);

        if (hasCode && hasMsg) return;
    }

    const char* msg = lua_tostring(L, -1);
    lua_pop(L, 1);

    lua_createtable(L, 0, 2);
    lua_pushinteger(L, code);
    lua_setfield(L, -2, "code");
    lua_pushstring(L, msg ? msg : "unknown");
    lua_setfield(L, -2, "message");
}

void xenon_throw(lua_State* L) {
    int code = LUA_ERRRUN;
    const char* msg = NULL;

    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "code");
        if (lua_isinteger(L, -1)) code = (int)lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "message");
        msg = lua_tostring(L, -1);
        lua_pop(L, 1);
    }
    else {
        msg = lua_tostring(L, -1);
    }

    if (code == LUA_ERRSYNTAX) {
        std::cerr
            << std::endl
            << "Xenon Exception"
            << std::endl
            << "  Syntax Error: "
            << (msg ? msg : "unknown")
            << std::endl;
    }
    else {
        std::cerr
            << std::endl
            << "Xenon Exception"
            << std::endl
            << "  Uncaught Exception:"
            << std::endl
            << std::endl
            << (msg ? msg : "unknown")
            << std::endl;
    }
}