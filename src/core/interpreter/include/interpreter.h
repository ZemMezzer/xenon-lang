#pragma once

extern "C" {
#include "lua.h"
}

#include "arg.h"

class Interpreter {
public:
    Interpreter();
    ~Interpreter();

    int do_arg(const Arg& arg);
    int do_file(const char* file_path);
private:
    lua_State* m_state;
};