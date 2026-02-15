#include "xenonstd.h"

#include "xstring.h"

#include "console.h"
#include "runtime.h"

#include "filesystem.h"

namespace xenon {
    namespace std {

        static void register_modules() {
            xenon_register_builtin_module("filesystem", xenon_openlib_filesystem);
        }

        void init(lua_State* L){
            xenon_openlib_xstring(L);

            xenon_openlib_console(L);
            xenon_openlib_runtime(L);

            register_modules();
        }
    }
}