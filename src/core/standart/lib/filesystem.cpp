#include "filesystem.h"
#include "stack_helper.h"
#include <cstdlib>
#include <string>

static std::string home_path;

static const char* invalid_path_exception_message = "Invalid path";

std::string lua_get_absolute_path(const std::string& path) {
    char fullpath[4096];

#ifdef _WIN32
    if (_fullpath(fullpath, path.c_str(), sizeof(fullpath)) != NULL)
        return std::string(fullpath);
#else
    if (realpath(path.c_str(), fullpath) != NULL)
        return std::string(fullpath);
#endif

    return path;
}

std::string lua_get_directory_path(const std::string& path) {
    std::string result;
    size_t pos = path.find_last_of("/\\");
    if (pos != std::string::npos){
        result = path.substr(0, pos);
    }
    else{
        result = ".";
    }

    return result;
}

std::string lua_get_home_directory() {
    return home_path;
}

void lua_set_home_path(const std::string& hpath) {
    home_path = hpath;
}

static int l_filesystem_get_home_path(lua_State* L) {
    lua_pushlstring(L, home_path.c_str(), home_path.size());
    return 1;
}

static int l_filesystem_set_home_path(lua_State* L) {

    int top_index = get_function_arg_top_index(L);

    if(!lua_isstring(L, top_index)){
        return luaL_error(L, invalid_path_exception_message);
    }

    std::string path = luaL_checkstring(L, top_index);
    home_path = path;
    return 0;
}

static int l_filesystem_get_absolute_path(lua_State* L) {

    int top_index = get_function_arg_top_index(L);

    if(!lua_isstring(L, top_index)){
        return luaL_error(L, invalid_path_exception_message);
    }

    std::string path = luaL_checkstring(L, top_index);
    std::string full_path = lua_get_absolute_path(path);

    if(full_path.size() <= 0) {
        return luaL_error(L, invalid_path_exception_message);
    }

    lua_pushlstring(L, full_path.c_str(), full_path.size());
    return 1;
}

static int l_filesystem_get_directory_path(lua_State* L) {
    
    int top_index = get_function_arg_top_index(L);

    if(!lua_isstring(L, top_index)){
        return luaL_error(L, invalid_path_exception_message);
    }

    std::string path = luaL_checkstring(L, top_index);
    std::string directory_path = lua_get_directory_path(path);

    if(directory_path.size() <= 0) {
        return luaL_error(L, invalid_path_exception_message);
    }

    lua_pushlstring(L, directory_path.c_str(), directory_path.size());
    return 1;
}

static const luaL_Reg lib[] = {
    {"get_path", l_filesystem_get_absolute_path},
    {"get_dir", l_filesystem_get_directory_path},
    {"get_home_path", l_filesystem_get_home_path},
    {"set_home_path", l_filesystem_set_home_path},
    {NULL, NULL}
};

extern "C" int luaopen_filesystem(lua_State* L) {
    luaL_newlib(L, lib);
    lua_setglobal(L, "file");
    return 0;
}