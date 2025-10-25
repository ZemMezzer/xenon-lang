#include <string>
extern "C" {
#include "lua.h"
}

extern "C" int luaopen_runtime(lua_State* L);
extern "C" int lua_do_file(lua_State* L, const std::string& file_name);

void lua_register_module(const std::string& module_name, lua_CFunction func);