#include "Interpreter.h"
#include "src/libs/StandartLib.h"

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

