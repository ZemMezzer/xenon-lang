#pragma once

extern "C" {
    #include "lua.h"
    #include <lauxlib.h>
}

#include <string>

std::string stack_value_to_string(lua_State* L, int idx);