#include "xenonstd.h"

#include "xstring.h"

#include "console.h"
#include "runtime.h"

#include "filesystem.h"

namespace xenon {
    namespace std {

        static void register_modules() {
            lua_register_module("filesystem", luaopen_filesystem);
        }

        void init(lua_State* L){
            luaopen_xstring(L);

            luaopen_console(L);
            luaopen_runtime(L);

            register_modules();
        }
    }
}