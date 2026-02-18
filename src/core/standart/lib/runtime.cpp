#include "runtime.h"
#include "stack_helper.h"
#include "xstring.h"
#include "preprocessor.h"
#include "xdirectory.h"
#include "registry.h"
#include "xerror.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

extern "C" {
    #include "lauxlib.h"
    #include "llex.h"
    #include "lua.h"
}

static constexpr const char* XENON_INCLUDE_STATE = "xenon.include_state";
static const char* invalid_arg_exception_message = "Invalid argument";

static std::unordered_map<std::string, lua_CFunction> xenon_modules;

static int xenon_try_include_builtin(lua_State* L, const std::string& module_name) {
    auto it = xenon_modules.find(module_name);
    if (it == xenon_modules.end())
        return 0;

    int rc = it->second(L);
    if (rc > 0) lua_pop(L, rc);

    return 1;
}

static std::string xenon_resolve_path(lua_State* L, const std::string& file) {
    std::filesystem::path p(file);

    if (p.is_absolute())
        return p.lexically_normal().generic_string();

    std::filesystem::path candidate = (std::filesystem::path(xenon_get_home_directory(L)) / p).lexically_normal();

    std::error_code ec;
    if (std::filesystem::exists(candidate, ec) && !ec)
        return candidate.generic_string();

    return candidate.generic_string();
}

static int xenon_include_file(lua_State* L, const std::string& file) {
    std::string resolved = xenon_resolve_path(L, file);

    lua_Integer st = xenon_registry_get_state_number(L, XENON_INCLUDE_STATE, resolved);
    if (st == 1) return luaL_error(L, "Circular include detected: %s", resolved.c_str());
    if (st == 2) return 0;

    xenon_registry_set_state_number(L, XENON_INCLUDE_STATE, resolved, 1);
    int rc = xenon_do_file(L, resolved);

    if (rc == LUA_OK) {
        xenon_registry_set_state_number(L, XENON_INCLUDE_STATE, resolved, 2);
        return 0;
    }

    xenon_registry_set_state_number(L, XENON_INCLUDE_STATE, resolved, 0);
    return lua_error(L);
}

static int xl_runtime_import(lua_State* L) {
    int top_index = lua_gettop(L);
    std::string file = xstring_to_std_string(L, top_index);
    std::string full_path = xenon_resolve_path(L, file);

    int baseTop = lua_gettop(L);

    // 1) load chunk
    int rc = xenon_loadfile(L, full_path.c_str());
    if (rc != LUA_OK) {
        return lua_error(L);
    }

    // stack: ... [chunk]

    // stack: ... [chunk][env]
    lua_newtable(L); // create env
    
    // stack: ... [chunk][env][mt]
	lua_newtable(L); // create mt fallback table

	// stack: ... [chunk][env][mt][_G]
    lua_pushglobaltable(L);

	// stack: ... [chunk][env][mt]
	lua_setfield(L, -2, "__index"); // mt.__index = _G, fallback
    
    // stack: ... [chunk][env]
	lua_setmetatable(L, -2); // set metatable for env; pops mt, env is still at -1

    // stack: ... [chunk][env][env]
    lua_pushvalue(L, -1);

    // stack: ... [chunk][env]
	int envRef = luaL_ref(L, LUA_REGISTRYINDEX); // get reference to env and pop it; env is still at -1, but we have its ref in envRef

    // stack: ... [chunk][env][env]
    lua_pushvalue(L, -1); // push env (value for upvalue)

    // stack: ... [chunk][env]
    lua_setupvalue(L, -3, 1); // set upvalue #1 of chunk; pops pushed env

    // stack: ... [chunk]
    lua_pop(L, 1); // remove env from stack, chunk is now on top

    rc = xenon_pcall(L, 0, LUA_MULTRET);
    if (rc != LUA_OK) {
        luaL_unref(L, LUA_REGISTRYINDEX, envRef);
        return lua_error(L);
    }

    int nret = lua_gettop(L) - baseTop;

    if (nret == 0) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, envRef);
		nret = 1; // return env if no return values, to allow module to export via env table instead of return statement
    }

    luaL_unref(L, LUA_REGISTRYINDEX, envRef);
	return nret; //return number of return values from chunk
}

static int xl_xenon_include(lua_State* L) {
    std::string name = xstring_to_std_string(L, 1);

    if (xenon_try_include_builtin(L, name)) {
        return 0;
    }

    return xenon_include_file(L, name);
}

static int xl_pack(lua_State* L) {
    int n = lua_gettop(L);

    lua_createtable(L, n, 0);
    for (int i = 1; i <= n; i++) {
        lua_pushvalue(L, i);
        lua_rawseti(L, -2, i);
    }
    return 1;
}

static const luaL_Reg lib[] = {
    {NULL, NULL}
};

extern "C" int xenon_openlib_runtime(lua_State* L) {
    
    lua_pushcfunction(L, xl_pack);
    lua_setglobal(L, XENON_PACK);

    lua_pushcfunction(L, xl_runtime_import);
    lua_setglobal(L, "import");

	lua_pushcfunction(L, xl_xenon_include);
	lua_setglobal(L, "__include");

    luaL_newlib(L, lib);
    lua_setglobal(L, "runtime");

    return 0;
}

extern "C" int xenon_pcall(lua_State* L, int nargs, int nresults) {
	int status = lua_pcall(L, nargs, nresults, 0);

    if (status != LUA_OK) {
        xenon_raise(L, status);
    }

    return status;
}

extern "C" int xenon_loadfile(lua_State* L, const std::string& file_name) {
    std::ifstream file(file_name);
    if (!file.is_open()) {
        lua_pushfstring(L, "Unable to open file: %s", file_name.c_str());
		xenon_raise(L, LUA_ERRERR);
        return LUA_ERRERR;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string processedCode = lua_preprocess_code(buffer.str());

    int status = luaL_loadbuffer(L, processedCode.c_str(), processedCode.size(), file_name.c_str());

    if(status != LUA_OK) {
        xenon_raise(L, status);
	}

    return status;
}

extern "C" int xenon_do_file(lua_State* L, const std::string& file_name) {

    int status = xenon_loadfile(L, file_name);

    if (status != LUA_OK) {
        return status;
    }

    return xenon_pcall(L, 0, LUA_MULTRET);
}

void xenon_register_builtin_module(const std::string& module_name, lua_CFunction func) {
    xenon_modules[module_name] = func;
}