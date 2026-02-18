#include "xdirectory.h"
#include "xstring.h"
#include "xerror.h"
#include "registry.h"

#include <string>
#include <filesystem>

static const std::string RK_HOME_PATH = "home_path";

std::string xenon_get_absolute_path(const std::string& path) {

	std::filesystem::path p(path);

    if(p.is_absolute()) {
        return path;
	}

    return std::filesystem::absolute(p).string();
}

void xenon_set_home_path(lua_State* L, const std::string& hpath) {
    xenon_registry_set_state_string(L, XENON_STATE, RK_HOME_PATH, hpath);
}

std::string xenon_get_home_directory(lua_State* L) {
    return xenon_registry_get_state_string(L, XENON_STATE, RK_HOME_PATH);
}

std::string xenon_get_parent_path(const std::string& path) {
    std::filesystem::path p(path);
    return p.parent_path().string();
}

bool xenon_is_absolute_path(const std::string& path) {
    std::filesystem::path p(path);
    return p.is_absolute();
}

static int xl_directory_get_home_path(lua_State* L) {
	std::string home_path = xenon_get_home_directory(L);
    xstring_push(L, home_path.c_str(), home_path.size());
    return 1;
}

static int xl_directory_set_home_path(lua_State* L) {

    int top_index = lua_gettop(L);

    if (!is_xstring(L, top_index)) {
        return luaL_error(L, invalid_path_exception_message);
    }

    std::string path = xstring_check(L, top_index)->to_std_string();
	xenon_set_home_path(L, path);
    return 0;
}

static int xl_directory_get_absolute_path(lua_State* L) {

    int top_index = lua_gettop(L);

    if (!is_xstring(L, top_index)) {
        return luaL_error(L, invalid_path_exception_message);
    }

    std::string path = xstring_check(L, top_index)->to_std_string();
    std::string full_path = xenon_get_absolute_path(path);

    if (full_path.size() <= 0) {
        return luaL_error(L, invalid_path_exception_message);
    }

    xstring_push(L, full_path.c_str(), full_path.size());
    return 1;
}

static int xl_directory_get_directory_path(lua_State* L) {

    int top_index = lua_gettop(L);

    if (!is_xstring(L, top_index)) {
        return luaL_error(L, invalid_path_exception_message);
    }

    std::string path = xstring_check(L, top_index)->to_std_string();
    std::string directory_path = xenon_get_parent_path(path);

    if (directory_path.size() <= 0) {
        return luaL_error(L, invalid_path_exception_message);
    }

    xstring_push(L, directory_path.c_str(), directory_path.size());
    return 1;
}

static int xl_directory_get_files(lua_State* L) {
    int top_index = lua_gettop(L);
    if (!is_xstring(L, top_index)) {
        return luaL_error(L, invalid_path_exception_message);
    }
    std::string path = xstring_check(L, top_index)->to_std_string();
    path = xenon_get_absolute_path(path);
    std::filesystem::path p(path);
    if (!std::filesystem::exists(p)) {
        std::string error_message = "Directory does not exist: " + path;
        return luaL_error(L, error_message.c_str());
    }
    if (!std::filesystem::is_directory(p)) {
        std::string error_message = "Path is not a directory: " + path;
        return luaL_error(L, error_message.c_str());
    }
    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(p)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().filename().string());
        }
    }
    lua_newtable(L);
    for (size_t i = 0; i < files.size(); ++i) {
        xstring_push(L, files[i].c_str(), files[i].size());
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}

static int xl_directory_create_directory(lua_State* L) {
    int top_index = lua_gettop(L);
    if (!is_xstring(L, top_index)) {
        return luaL_error(L, invalid_path_exception_message);
    }
    std::string path = xstring_check(L, top_index)->to_std_string();
    path = xenon_get_absolute_path(path);
    std::filesystem::path p(path);
    if (std::filesystem::exists(p)) {
        std::string error_message = "Directory already exists: " + path;
        return luaL_error(L, error_message.c_str());
    }
    try {
        std::filesystem::create_directory(p);
    }
    catch (const std::filesystem::filesystem_error& e) {
        return luaL_error(L, e.what());
    }
    return 0;
}

static int xl_directory_directory_exists(lua_State* L) {
    int top_index = lua_gettop(L);
    if (!is_xstring(L, top_index)) {
        return luaL_error(L, invalid_path_exception_message);
    }
    std::string path = xstring_check(L, top_index)->to_std_string();
    path = xenon_get_absolute_path(path);
    std::filesystem::path p(path);
    bool exists = std::filesystem::exists(p) && std::filesystem::is_directory(p);
    lua_pushboolean(L, exists);
    return 1;
}

static int xl_directory_remove_directory(lua_State* L) {
    int top_index = lua_gettop(L);
    if (!is_xstring(L, top_index)) {
        return luaL_error(L, invalid_path_exception_message);
    }
    std::string path = xstring_check(L, top_index)->to_std_string();
    path = xenon_get_absolute_path(path);
    std::filesystem::path p(path);
    if (!std::filesystem::exists(p)) {
        std::string error_message = "Directory does not exist: " + path;
        return luaL_error(L, error_message.c_str());
    }
    if (!std::filesystem::is_directory(p)) {
        std::string error_message = "Path is not a directory: " + path;
        return luaL_error(L, error_message.c_str());
    }
    try {
        std::filesystem::remove_all(p);
    }
    catch (const std::filesystem::filesystem_error& e) {
        return luaL_error(L, e.what());
    }
    return 0;
}

static int xl_directory_is_directory(lua_State* L) {
    int top_index = lua_gettop(L);
    if (!is_xstring(L, top_index)) {
        return luaL_error(L, invalid_path_exception_message);
    }
    std::string path = xstring_check(L, top_index)->to_std_string();
    path = xenon_get_absolute_path(path);
    std::filesystem::path p(path);
    bool is_directory = std::filesystem::exists(p) && std::filesystem::is_directory(p);
    lua_pushboolean(L, is_directory);
    return 1;
}

static int xl_directory_get_parent_directory(lua_State* L) {
    int top_index = lua_gettop(L);
    if (!is_xstring(L, top_index)) {
        return luaL_error(L, invalid_path_exception_message);
    }
    std::string path = xstring_check(L, top_index)->to_std_string();
    std::string directory_path = xenon_get_parent_path(path);
    if (directory_path.size() <= 0) {
        return luaL_error(L, invalid_path_exception_message);
    }
    xstring_push(L, directory_path.c_str(), directory_path.size());
    return 1;
}

static int xl_directory_combine_path(lua_State* L) {
    int top_index = lua_gettop(L);
    if (top_index < 2) {
        return luaL_error(L, "Expected at least 2 arguments");
    }
    std::filesystem::path combined_path;
    for (int i = 1; i <= top_index; ++i) {
        if (!is_xstring(L, i)) {
            return luaL_error(L, invalid_path_exception_message);
        }
        std::string part = xstring_check(L, i)->to_std_string();
        if (combined_path.empty()) {
            combined_path = part;
        } else {
            combined_path = std::filesystem::path(combined_path) / part;
        }
    }
    xstring_push(L, combined_path.string().c_str(), combined_path.string().size());
	return 1;
}

static const luaL_Reg lib[] = {
    {"get_path", xl_directory_get_absolute_path},
    {"get_home_path", xl_directory_get_home_path},
    {"set_home_path", xl_directory_set_home_path},
	{"parent", xl_directory_get_parent_directory},
    {"get_files", xl_directory_get_files},
    {"create", xl_directory_create_directory},
    {"exists", xl_directory_directory_exists},
    {"remove", xl_directory_remove_directory},
    {"is_directory", xl_directory_is_directory},
	{"combine", xl_directory_combine_path},
    {NULL, NULL}
};

extern "C" int xenon_openlib_directory(lua_State* L) {
    luaL_newlib(L, lib);
    lua_setglobal(L, "directory");
    return 0;
}