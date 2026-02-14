#include "xenonstd.h"

#include "pointer.h"
#include "safe_pointer.h"
#include "xstring.h"

#include "console.h"
#include "type.h"
#include "runtime.h"

#include "filesystem.h"

namespace xenon {
    namespace std {

        static void register_modules() {
            lua_register_module("filesystem", luaopen_filesystem);
        }

        void init(lua_State* L){
            luaopen_ptr(L);
            luaopen_safeptr(L);
            luaopen_xstring(L);

            luaopen_console(L);
            luaopen_types(L);
            luaopen_runtime(L);

            register_modules();
        }
    }
}