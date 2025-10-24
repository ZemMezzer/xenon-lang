#include "xenonstd.h"

#include "pointer.h"
#include "console.h"
#include "type.h"

namespace xenon {
    namespace std {
        void init(lua_State* L){
            luaopen_ptr(L);
            luaopen_console(L);
            luaopen_types(L);
        }
    }
}