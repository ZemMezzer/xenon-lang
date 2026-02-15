#include <string>
extern "C" {
#include "lua.h"
}
extern "C" void xenon_throw(lua_State* L);
extern "C" int luaopen_runtime(lua_State* L);
extern "C" int xenon_do_file(lua_State* L, const std::string& file_name);
extern "C" int xenon_loadfile(lua_State* L, const std::string& file_name);
extern "C" int xenon_pcall(lua_State* L, int nargs, int nresults);

void lua_register_module(const std::string& module_name, lua_CFunction func);