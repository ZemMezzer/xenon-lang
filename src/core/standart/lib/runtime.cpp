#include "runtime.h"
#include "stack_helper.h"
#include "lua.h"
#include "preprocessor.h"
#include "filesystem.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

extern "C" {
#include "lauxlib.h"
}

static const char* invalid_arg_exception_message = "Invalid argument";
static std::unordered_map<std::string, lua_CFunction> xenon_modules;

static int l_runtime_import(lua_State* L) {
    if(!lua_isstring(L, 1)){
        return luaL_error(L, invalid_arg_exception_message);
    }

    std::string module_name = luaL_checkstring(L, 1);

    auto it = xenon_modules.find(module_name);

    if(it == xenon_modules.end()){
        std::string error_message = "Can't find module named '" + module_name + "'";
        return luaL_error(L, error_message.c_str());
    }

    return it->second(L);
}

static int l_runtime_do_file(lua_State* L) {
    auto top_index = get_function_arg_top_index(L);

    if(!lua_isstring(L, top_index)){
        return luaL_error(L, invalid_arg_exception_message);
    }

    std::string file_name = luaL_checkstring(L, top_index);

    auto top_before = lua_gettop(L);
    lua_do_file(L, lua_get_home_directory() + "/" + file_name);
    return lua_gettop(L) - top_before;
}

static const luaL_Reg lib[] = {
    {"run", l_runtime_do_file},
    {NULL, NULL}
};

extern "C" int luaopen_runtime(lua_State* L) {
    
    lua_pushcfunction(L, l_runtime_import);
    lua_setglobal(L, "import");

    luaL_newlib(L, lib);
    lua_setglobal(L, "runtime");

    return 0;
}

extern "C" int lua_do_file(lua_State* L, const std::string& file_name) {

    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Unable to open file " << file_name << std::endl;
        return -1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string processedCode = lua_preprocess_code(buffer.str());

    if (luaL_loadbuffer(L, processedCode.c_str(), processedCode.size(), file_name.c_str()) || lua_pcall(L, 0, LUA_MULTRET, 0)) {
        const char* err = lua_tostring(L, -1);

        std::cerr 
        << std::endl 
        << "Xenon Exception" 
        << std::endl 
        << "  Uncaught Exception:" 
        << std::endl 
        << std::endl 
        << (err ? err : "unknown") 
        << std::endl;

        lua_pop(L, 1);
        return -1;
    }

    return 0;
}

void lua_register_module(const std::string& module_name, lua_CFunction func) {
    xenon_modules[module_name] = func;
}