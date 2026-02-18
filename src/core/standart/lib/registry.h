#pragma once

extern "C" {
#include <lua.h>
}

#include <string>

inline constexpr const char* XENON_STATE = "xenon.state";

lua_Integer xenon_registry_get_state_number(lua_State* L, const char* key, const std::string& rk);
void xenon_registry_set_state_number(lua_State* L, const char* key, const std::string& rk, lua_Integer value);

std::string xenon_registry_get_state_string(lua_State* L, const char* key, const std::string& rk);
void xenon_registry_set_state_string(lua_State* L, const char* key, const std::string& rk, const std::string& value);