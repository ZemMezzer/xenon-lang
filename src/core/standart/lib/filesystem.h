#pragma once

#include <string>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

bool xenon_file_exists(const std::string& path);
bool xenon_is_abs_path_posix(const std::string& path);
std::string xenon_dirname_posix(const std::string& path);
std::string xenon_join_posix(const std::string& base, const std::string& rel);
std::string xenon_normalize_posix(const std::string& path);

std::string xenon_get_absolute_path(const std::string& relative_path);
std::string xenon_get_directory_path(const std::string& path);
std::string xenon_get_home_directory();

void xenon_set_home_path(const std::string& home_path);
extern "C" int xenon_openlib_filesystem(lua_State* L);