#include "xfile.h"
#include "xdirectory.h"
#include "xerror.h"

#include "stack_helper.h"
#include "xstring.h"
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>

static const char* empty_content_exception_message = "Empty content";

bool xenon_file_exists(const std::string& path) {
	std::string absolute_path = xenon_get_absolute_path(path);
    std::filesystem::path p(absolute_path);
	return std::filesystem::exists(p) && std::filesystem::is_regular_file(p);
}

static std::pair<std::string, bool> xenon_file_move_file(std::string source_path, std::string distanation_path, bool copy) {
    std::string source = xenon_get_absolute_path(source_path);
    std::string destination = xenon_get_absolute_path(distanation_path);

    std::filesystem::path src_path(source);

    if (!std::filesystem::exists(src_path)) {
        std::string error_message = "File does not exist: " + source;
		return { error_message, false };
    }

    if (!std::filesystem::is_regular_file(src_path)) {
        std::string error_message = "Path is not a file: " + source;
		return { error_message, false };
    }

    std::filesystem::path dst_path(destination);

    if (copy) {
        std::filesystem::copy_file(
            src_path,
            dst_path,
            std::filesystem::copy_options::overwrite_existing
        );
    }
    else {
        try {
            std::filesystem::rename(src_path, dst_path);
        }
        catch (const std::filesystem::filesystem_error&) {
            std::filesystem::copy_file(
                src_path,
                dst_path,
                std::filesystem::copy_options::overwrite_existing
            );
            std::filesystem::remove(src_path);
        }
    }

    return { dst_path.string(), true };
}

static int xl_file_exists(lua_State* L) {
    int top_index = lua_gettop(L);
    if (!is_xstring(L, top_index)) {
        return luaL_error(L, invalid_path_exception_message);
    }
    std::string path = xstring_check(L, top_index)->to_std_string();
    path = xenon_get_absolute_path(path);

    std::filesystem::path p(path);

    bool exists = std::filesystem::exists(p);
    lua_pushboolean(L, exists);
    return 1;
}

static int xl_file_read_all_text(lua_State* L) {
    int top_index = lua_gettop(L);
    if(!is_xstring(L, top_index)){
        return luaL_error(L, invalid_path_exception_message);
    }
    std::string path = xstring_check(L, top_index)->to_std_string();
    path = xenon_get_absolute_path(path);
    std::ifstream file(path);
    if (!file.is_open()) {
		std::string error_message = "Failed to open file: " + path;
        return luaL_error(L, error_message.c_str());
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    xstring_push(L, content.c_str(), content.size());
	return 1;
}

static int xl_file_write_all_text(lua_State* L) {
	int top_index = lua_gettop(L);
    if(!is_xstring(L, top_index - 1)){
        return luaL_error(L, invalid_path_exception_message);
    }

    if(!is_xstring(L, top_index)) {
        return luaL_error(L, empty_content_exception_message);
	}

    std::string path = xstring_check(L, top_index - 1)->to_std_string();
    std::string content = xstring_check(L, top_index)->to_std_string();
    path = xenon_get_absolute_path(path);
    std::ofstream file(path);

    if(!file.good()) {
		return luaL_error(L, "Failed to create file for writing");
	}

    if (!file.is_open()) {
        return luaL_error(L, "Failed to open file for writing");
    }
    file << content;
    file.close();
	return 0;
}

static int xl_file_remove_file(lua_State* L) {
    int top_index = lua_gettop(L);
    if(!is_xstring(L, top_index)){
        return luaL_error(L, invalid_path_exception_message);
    }
    std::string path = xstring_check(L, top_index)->to_std_string();
    path = xenon_get_absolute_path(path);
    std::filesystem::path p(path);
    if (!std::filesystem::exists(p)) {

		std::string error_message = "File does not exist: " + path;
        return luaL_error(L, error_message.c_str());
    }
    if (!std::filesystem::is_regular_file(p)) {
		std::string error_message = "Path is not a file: " + path;
        return luaL_error(L, error_message.c_str());
    }
    try {
        std::filesystem::remove(p);
    } catch (const std::filesystem::filesystem_error& e) {
        return luaL_error(L, e.what());
    }
    return 0;
}

static int xl_file_get_file_name(lua_State* L) {
    int top_index = lua_gettop(L);
    if(!is_xstring(L, top_index)){
        return luaL_error(L, invalid_path_exception_message);
    }
    std::string path = xstring_check(L, top_index)->to_std_string();
    path = xenon_get_absolute_path(path);
    std::filesystem::path p(path);
    if (!std::filesystem::exists(p)) {
        std::string error_message = "File does not exist: " + path;
        return luaL_error(L, error_message.c_str());
    }
    if (!std::filesystem::is_regular_file(p)) {
        std::string error_message = "Path is not a file: " + path;
        return luaL_error(L, error_message.c_str());
    }
    std::string file_name = p.filename().string();
    xstring_push(L, file_name.c_str(), file_name.size());
	return 1;
}

static int xl_file_is_file(lua_State* L) {
    int top_index = lua_gettop(L);
    if(!is_xstring(L, top_index)){
        return luaL_error(L, invalid_path_exception_message);
    }
    std::string path = xstring_check(L, top_index)->to_std_string();
    path = xenon_get_absolute_path(path);
    std::filesystem::path p(path);
    bool is_file = std::filesystem::exists(p) && std::filesystem::is_regular_file(p);
    lua_pushboolean(L, is_file);
	return 1;
}

static int xl_file_get_extension(lua_State* L) {
    int top_index = lua_gettop(L);
    if(!is_xstring(L, top_index)){
        return luaL_error(L, invalid_path_exception_message);
    }
    std::string path = xstring_check(L, top_index)->to_std_string();
    path = xenon_get_absolute_path(path);
    std::filesystem::path p(path);
    if (!std::filesystem::exists(p)) {
        std::string error_message = "File does not exist: " + path;
        return luaL_error(L, error_message.c_str());
    }
    if (!std::filesystem::is_regular_file(p)) {
        std::string error_message = "Path is not a file: " + path;
        return luaL_error(L, error_message.c_str());
    }
    std::string extension = p.extension().string();
    xstring_push(L, extension.c_str(), extension.size());
	return 1;
}

static int xl_file_set_extension(lua_State* L) {
    int top_index = lua_gettop(L);
    if(!is_xstring(L, top_index - 1)){
        return luaL_error(L, invalid_path_exception_message);
    }
    if(!is_xstring(L, top_index)) {
        return luaL_error(L, "Invalid extension");
    }
    std::string path = xstring_check(L, top_index - 1)->to_std_string();
    std::string new_extension = xstring_check(L, top_index)->to_std_string();
    path = xenon_get_absolute_path(path);
    std::filesystem::path p(path);
    if (!std::filesystem::exists(p)) {
        std::string error_message = "File does not exist: " + path;
        return luaL_error(L, error_message.c_str());
    }
    if (!std::filesystem::is_regular_file(p)) {
        std::string error_message = "Path is not a file: " + path;
        return luaL_error(L, error_message.c_str());
    }
    p.replace_extension(new_extension);

    std::string new_path = p.string();
    xstring_push(L, new_path.c_str(), new_path.size());
	return 1;
}

static int xl_file_rename_file(lua_State* L) {
    int top_index = lua_gettop(L);
    if(!is_xstring(L, top_index - 1)){
        return luaL_error(L, invalid_path_exception_message);
    }
    if(!is_xstring(L, top_index)) {
        return luaL_error(L, "Invalid new name");
    }
    std::string path = xstring_check(L, top_index - 1)->to_std_string();
    std::string new_name = xstring_check(L, top_index)->to_std_string();
    path = xenon_get_absolute_path(path);
    std::filesystem::path p(path);
    if (!std::filesystem::exists(p)) {
        std::string error_message = "File does not exist: " + path;
        return luaL_error(L, error_message.c_str());
    }
    if (!std::filesystem::is_regular_file(p)) {
        std::string error_message = "Path is not a file: " + path;
        return luaL_error(L, error_message.c_str());
    }
    std::filesystem::path new_path = p.parent_path() / new_name;
    try {
        std::filesystem::rename(p, new_path);
    } catch (const std::filesystem::filesystem_error& e) {
        return luaL_error(L, e.what());
    }

    xstring_push(L, new_path.string().c_str(), new_path.string().size());
	return 1;
}

static int xl_file_move_file(lua_State* L) {
    int top_index = lua_gettop(L);

    if (!is_xstring(L, top_index - 1)) {
        return luaL_error(L, invalid_path_exception_message);
    }

    if (!is_xstring(L, top_index)) {
        return luaL_error(L, "Invalid destination path");
    }

    std::string source = xstring_check(L, top_index - 1)->to_std_string();
    std::string destination = xstring_check(L, top_index)->to_std_string();

	auto [result, success] = xenon_file_move_file(source, destination, false);

    if(success) {
        xstring_push(L, result.c_str(), result.size());
        return 1;
    } else {
        return luaL_error(L, result.c_str());
	}
}

static int xl_file_copy_file(lua_State* L) {
    int top_index = lua_gettop(L);
    if (!is_xstring(L, top_index - 1)) {
        return luaL_error(L, invalid_path_exception_message);
    }
    if (!is_xstring(L, top_index)) {
        return luaL_error(L, "Invalid destination path");
    }
    std::string source = xstring_check(L, top_index - 1)->to_std_string();
    std::string destination = xstring_check(L, top_index)->to_std_string();

    auto [result, success] = xenon_file_move_file(source, destination, true);
    if(success) {
        xstring_push(L, result.c_str(), result.size());
        return 1;
    } else {
        return luaL_error(L, result.c_str());
	}
}

static const luaL_Reg lib[] = {
	{"exists", xl_file_exists},
	{"read_all_text", xl_file_read_all_text},
	{"write_all_text", xl_file_write_all_text},
	{"remove_file", xl_file_remove_file},
	{"get_file_name", xl_file_get_file_name},
	{"is_file", xl_file_is_file},
	{"get_extension", xl_file_get_extension},
	{"set_extension", xl_file_set_extension},
	{"rename", xl_file_rename_file},
    {"move", xl_file_move_file},
	{"copy", xl_file_copy_file},
    {NULL, NULL}
};

extern "C" int xenon_openlib_file(lua_State* L) {
    luaL_newlib(L, lib);
    lua_setglobal(L, "file");
    return 0;
}