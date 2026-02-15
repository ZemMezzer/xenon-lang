#include "filesystem.h"
#include "stack_helper.h"
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>

static std::string home_path;

static const char* invalid_path_exception_message = "Invalid path";

std::string xenon_get_absolute_path(const std::string& path) {
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

std::string xenon_get_directory_path(const std::string& path) {
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

bool xenon_is_abs_path_posix(const std::string& path) {
    return !path.empty() && path[0] == '/';
}

std::string xenon_dirname_posix(const std::string& path) {
    // "/a/b/c.xn" -> "/a/b"
    // "c.xn" -> ""
    // "/c.xn" -> "/"
    auto pos = path.find_last_of('/');
    if (pos == std::string::npos) return "";
    if (pos == 0) return "/";
    return path.substr(0, pos);
}

std::string xenon_join_posix(const std::string& base, const std::string& rel) {
    if (base.empty() || base == ".") return rel;
    if (base == "/") return "/" + rel;
    if (!base.empty() && base.back() == '/') return base + rel;
    return base + "/" + rel;
}

std::string xenon_normalize_posix(const std::string& path) {
    if (path.empty()) return path;

    bool abs = xenon_is_abs_path_posix(path);
    std::vector<std::string> parts;
    parts.reserve(16);

    size_t i = 0;
    while (i < path.size()) {
        // skip repeated '/'
        while (i < path.size() && path[i] == '/') i++;
        if (i >= path.size()) break;

        size_t j = i;
        while (j < path.size() && path[j] != '/') j++;

        std::string token = path.substr(i, j - i);
        i = j;

        if (token == "." || token.empty()) continue;

        if (token == "..") {
            if (!parts.empty() && parts.back() != "..") {
                parts.pop_back();
            }
            else {
                if (!abs) parts.push_back("..");
            }
            continue;
        }

        parts.push_back(std::move(token));
    }

    std::string out;
    if (abs) out = "/";

    for (size_t k = 0; k < parts.size(); k++) {
        if (!out.empty() && out.back() != '/') out.push_back('/');
        out += parts[k];
    }

    if (out.empty()) return abs ? "/" : ".";
    return out;
}

bool xenon_file_exists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

std::string xenon_get_home_directory() {
    return home_path;
}

void xenon_set_home_path(const std::string& hpath) {
    home_path = hpath;
}

static int xl_filesystem_get_home_path(lua_State* L) {
    lua_pushlstring(L, home_path.c_str(), home_path.size());
    return 1;
}

static int xl_filesystem_set_home_path(lua_State* L) {

    int top_index = get_function_arg_top_index(L);

    if(!lua_isstring(L, top_index)){
        return luaL_error(L, invalid_path_exception_message);
    }

    std::string path = luaL_checkstring(L, top_index);
    home_path = path;
    return 0;
}

static int xl_filesystem_get_absolute_path(lua_State* L) {

    int top_index = get_function_arg_top_index(L);

    if(!lua_isstring(L, top_index)){
        return luaL_error(L, invalid_path_exception_message);
    }

    std::string path = luaL_checkstring(L, top_index);
    std::string full_path = xenon_get_absolute_path(path);

    if(full_path.size() <= 0) {
        return luaL_error(L, invalid_path_exception_message);
    }

    lua_pushlstring(L, full_path.c_str(), full_path.size());
    return 1;
}

static int xl_filesystem_get_directory_path(lua_State* L) {
    
    int top_index = get_function_arg_top_index(L);

    if(!lua_isstring(L, top_index)){
        return luaL_error(L, invalid_path_exception_message);
    }

    std::string path = luaL_checkstring(L, top_index);
    std::string directory_path = xenon_get_directory_path(path);

    if(directory_path.size() <= 0) {
        return luaL_error(L, invalid_path_exception_message);
    }

    lua_pushlstring(L, directory_path.c_str(), directory_path.size());
    return 1;
}

static const luaL_Reg lib[] = {
    {"get_path", xl_filesystem_get_absolute_path},
    {"get_dir", xl_filesystem_get_directory_path},
    {"get_home_path", xl_filesystem_get_home_path},
    {"set_home_path", xl_filesystem_set_home_path},
    {NULL, NULL}
};

extern "C" int xenon_openlib_filesystem(lua_State* L) {
    luaL_newlib(L, lib);
    lua_setglobal(L, "file");
    return 0;
}