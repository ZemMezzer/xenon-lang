#pragma once
#include "../../Core/LuaContext.hpp"

class Lib
{
public:
    void virtual loadLib(LuaCpp::LuaContext& context) = 0;
};
