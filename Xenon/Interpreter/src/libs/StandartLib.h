#pragma once
#include "Lib.h"
#include "../../Core/LuaContext.hpp"

class StandartLib : public Lib
{
public:
    void loadLib(LuaCpp::LuaContext& context) override;
};

