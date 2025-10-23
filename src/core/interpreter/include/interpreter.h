#pragma once

extern "C" {
#include "lua.h"
}

#include "arg.h"

class Interpreter {
public:
    Interpreter(lua_State* L);

    int doArg(const Arg& arg);
    int doFile(const char* file_path);
private:
    lua_State* m_state;
};