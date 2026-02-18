#include "registry.h"

lua_Integer xenon_registry_get_state_number(lua_State* L, const char* key, const std::string& rk) {
    lua_getfield(L, LUA_REGISTRYINDEX, key);
    if (!lua_istable(L, -1)) { lua_pop(L, 1); return 0; }

    lua_pushlstring(L, rk.data(), rk.size());
    lua_gettable(L, -2);
    lua_Integer r = lua_isinteger(L, -1) ? lua_tointeger(L, -1) : 0;
    lua_pop(L, 2);
    return r;
}

void xenon_registry_set_state_number(lua_State* L, const char* key, const std::string& rk, lua_Integer value) {
    lua_getfield(L, LUA_REGISTRYINDEX, key);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setfield(L, LUA_REGISTRYINDEX, key);
    }

    lua_pushlstring(L, rk.data(), rk.size());
    if (value == 0) lua_pushnil(L);
    else lua_pushinteger(L, value);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

std::string xenon_registry_get_state_string(lua_State* L, const char* key, const std::string& rk) {
    lua_getfield(L, LUA_REGISTRYINDEX, key);
    if (!lua_istable(L, -1)) { lua_pop(L, 1); return {}; }

    lua_pushlstring(L, rk.data(), rk.size());
    lua_gettable(L, -2);

    size_t len = 0;
    const char* s = lua_tolstring(L, -1, &len);
    std::string r = s ? std::string(s, len) : std::string();

    lua_pop(L, 2);
    return r;
}

void xenon_registry_set_state_string(lua_State* L, const char* key, const std::string& rk, const std::string& value) {
    lua_getfield(L, LUA_REGISTRYINDEX, key);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setfield(L, LUA_REGISTRYINDEX, key);
    }

    lua_pushlstring(L, rk.data(), rk.size());
    lua_pushlstring(L, value.data(), value.size());
    lua_settable(L, -3);

    lua_pop(L, 1);
}