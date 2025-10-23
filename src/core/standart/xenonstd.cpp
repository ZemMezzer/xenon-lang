#include "xenonstd.h"

#include "lua.h"
#include "pointer.h"
#include "console.h"

namespace xenon {
    namespace std {
        void init(lua_State* L){
            luaopen_ptr(L);
            luaopen_console(L);
        }
    }
}