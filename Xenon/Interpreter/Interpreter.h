#pragma once

#include "src/libs/Lib.h"

class Interpreter
{
public:
    Interpreter();
    void loadLib(Lib* lib);
    
    void loadScript(const char* script);

private:
    LuaCpp::LuaContext context;
};

