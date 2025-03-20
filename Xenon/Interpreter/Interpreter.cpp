#include "Interpreter.h"

#include <Lib.h>

Interpreter::Interpreter()
{
    context = LuaCpp::LuaContext();
}

void Interpreter::loadLib(Lib* lib)
{
    lib->loadLib(context);
}

void Interpreter::loadScript(const char* script)
{
    context.CompileStringAndRun(script);
}

