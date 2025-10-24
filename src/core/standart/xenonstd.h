#pragma once

extern "C" {
#include "lua.h"
}

namespace xenon {
    namespace std {
        void init(lua_State* L);
    }
}