#include "runtime.h"
#include "stack_helper.h"
#include "xstring.h"
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

static constexpr const char* XENON_CUR_FILE_STACK = "xenon.current_file_stack";
static constexpr const char* XENON_LOADED = "xenon.loaded";
static constexpr const char* XENON_LOADING = "xenon.loading";

static const char* invalid_arg_exception_message = "Invalid argument";
static std::unordered_map<std::string, lua_CFunction> xenon_modules;

static bool xenon_file_exists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

static void xenon_get_or_create_reg_table(lua_State* L, const char* key) {
    lua_getfield(L, LUA_REGISTRYINDEX, key);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setfield(L, LUA_REGISTRYINDEX, key);
    }
}

static bool xenon_reg_table_get_bool(lua_State* L, const char* key, const std::string& k) {
    xenon_get_or_create_reg_table(L, key);
    lua_pushlstring(L, k.c_str(), k.size());
    lua_gettable(L, -2);
    bool r = lua_toboolean(L, -1) != 0;
    lua_pop(L, 2);
    return r;
}

static void xenon_reg_table_set_bool(lua_State* L, const char* key, const std::string& k, bool v) {
    xenon_get_or_create_reg_table(L, key);
    lua_pushlstring(L, k.c_str(), k.size());
    lua_pushboolean(L, v ? 1 : 0);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

static void xenon_reg_table_set_nil(lua_State* L, const char* key, const std::string& k) {
    xenon_get_or_create_reg_table(L, key);
    lua_pushlstring(L, k.c_str(), k.size());
    lua_pushnil(L);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

static void xenon_push_current_file(lua_State* L, const std::string& f) {
    xenon_get_or_create_reg_table(L, XENON_CUR_FILE_STACK);
    lua_Integer n = (lua_Integer)lua_rawlen(L, -1);
    lua_pushlstring(L, f.c_str(), f.size());
    lua_rawseti(L, -2, n + 1);
    lua_pop(L, 1);
}

static void xenon_pop_current_file(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, XENON_CUR_FILE_STACK);
    if (lua_istable(L, -1)) {
        lua_Integer n = (lua_Integer)lua_rawlen(L, -1);
        if (n > 0) {
            lua_pushnil(L);
            lua_rawseti(L, -2, n);
        }
    }
    lua_pop(L, 1);
}

static bool xenon_is_abs_path_posix(const std::string& p) {
    return !p.empty() && p[0] == '/';
}

static std::string xenon_dirname_posix(const std::string& path) {
    // "/a/b/c.xn" -> "/a/b"
    // "c.xn" -> ""
    // "/c.xn" -> "/"
    auto pos = path.find_last_of('/');
    if (pos == std::string::npos) return "";
    if (pos == 0) return "/";
    return path.substr(0, pos);
}

static std::string xenon_join_posix(const std::string& base, const std::string& rel) {
    if (base.empty() || base == ".") return rel;
    if (base == "/") return "/" + rel;
    if (!base.empty() && base.back() == '/') return base + rel;
    return base + "/" + rel;
}

static std::string xenon_normalize_posix(const std::string& path) {
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

static std::string xenon_get_current_file(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, XENON_CUR_FILE_STACK);
    if (!lua_istable(L, -1)) { lua_pop(L, 1); return {}; }
    lua_Integer n = (lua_Integer)lua_rawlen(L, -1);
    if (n <= 0) { lua_pop(L, 1); return {}; }

    lua_rawgeti(L, -1, n);
    const char* s = lua_tostring(L, -1);
    std::string r = s ? s : "";
    lua_pop(L, 2);
    return r;
}

static std::string xenon_resolve_path(lua_State* L, const std::string& file) {
    if (!file.empty() && file[0] == '/') {
        return file;
    }

    std::string cur = xenon_get_current_file(L);
    if (!cur.empty()) {
        std::string base = xenon_dirname_posix(cur);
        std::string candidate = xenon_normalize_posix(
            xenon_join_posix(base, file)
        );

        if (xenon_file_exists(candidate)) {
            return candidate;
        }
    }

    std::string home = lua_get_home_directory();
    std::string homeCandidate = xenon_normalize_posix(
        xenon_join_posix(home, file)
    );

    if (xenon_file_exists(homeCandidate)) {
        return homeCandidate;
    }

    return homeCandidate;
}

static bool xenon_is_loaded(lua_State* L, const std::string& resolvedPath) {
    return xenon_reg_table_get_bool(L, XENON_LOADED, resolvedPath);
}

static bool xenon_is_loading(lua_State* L, const std::string& resolvedPath) {
    return xenon_reg_table_get_bool(L, XENON_LOADING, resolvedPath);
}

static void xenon_mark_loading(lua_State* L, const std::string& resolvedPath) {
    xenon_reg_table_set_bool(L, XENON_LOADING, resolvedPath, true);
}

static void xenon_mark_loaded(lua_State* L, const std::string& resolvedPath, bool ok) {
    xenon_reg_table_set_nil(L, XENON_LOADING, resolvedPath);

    if (ok) {
        xenon_reg_table_set_bool(L, XENON_LOADED, resolvedPath, true);
    }
    else {
        xenon_reg_table_set_nil(L, XENON_LOADED, resolvedPath);
    }
}

static int l_runtime_import(lua_State* L) {
    std::string module_name = xstring_to_std_string(L, 1);

    auto it = xenon_modules.find(module_name);

    if(it == xenon_modules.end()){
        std::string error_message = "Can't find module named '" + module_name + "'";
        return luaL_error(L, error_message.c_str());
    }

    return it->second(L);
}

static int l_xenon_include(lua_State* L) {
    std::string file = xstring_to_std_string(L, 1);

    std::string resolved = xenon_resolve_path(L, file);

    if (xenon_is_loading(L, resolved)) {
        return luaL_error(L, "Circular include detected: %s", resolved.c_str());
    }

    if (xenon_is_loaded(L, resolved)) {
        return 0;
    }

    xenon_mark_loading(L, resolved);
    int rc = lua_do_file(L, resolved);
    xenon_mark_loaded(L, resolved, rc == 0);

    if (rc != 0) {
        return luaL_error(L, "Include failed: %s", resolved.c_str());
    }

    return 0;
}

static int l_runtime_do_file(lua_State* L) {
    auto top_index = get_function_arg_top_index(L);

    std::string file_name = xstring_to_std_string(L, top_index);
    std::string full_path = lua_get_home_directory() + "/" + file_name;

    auto top_before = lua_gettop(L);
    int rc = lua_do_file(L, full_path);

    if (rc != 0) {
        return luaL_error(L, "Unable to process file");
    }

    return lua_gettop(L) - top_before;
}

static const luaL_Reg lib[] = {
    {"run", l_runtime_do_file},
    {NULL, NULL}
};

extern "C" int luaopen_runtime(lua_State* L) {
    
    lua_pushcfunction(L, l_runtime_import);
    lua_setglobal(L, "import");

	lua_pushcfunction(L, l_xenon_include);
	lua_setglobal(L, "__include");

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