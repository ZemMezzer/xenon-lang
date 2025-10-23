#include "xenonstd.h"
#include "pointer.h"

namespace xenon {
    namespace std {
        void init(lua_State* L){
            luaopen_ptr(L);
            lua_setglobal(L, "ptr");
        }
    }
}