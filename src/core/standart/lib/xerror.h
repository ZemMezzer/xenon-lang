#pragma once

extern "C" { 
#include "lua.h"
}

inline constexpr const char* value_out_of_range_message = "Value out of range";
inline constexpr const char* invalid_argument_message = "Invalid argument";
inline constexpr const char* invalid_path_exception_message = "Invalid path";

void xenon_raise(lua_State* L, int code);
void xenon_throw(lua_State* L);