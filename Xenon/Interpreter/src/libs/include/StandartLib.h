#pragma once
#include "Lib.h"

class StandartLib : public Lib
{
public:
    void loadLib(LuaCpp::LuaContext& context) override;
};

