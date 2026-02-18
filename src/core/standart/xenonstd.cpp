#include "xenonstd.h"

#include "xstring.h"

#include "console.h"
#include "runtime.h"

#include "xfile.h"
#include "xdirectory.h"

namespace xenon {
    namespace std {

        static void register_modules() {
            xenon_register_builtin_module("file", xenon_openlib_file);
			xenon_register_builtin_module("directory", xenon_openlib_directory);
        }

        void init(lua_State* L){
            xenon_openlib_xstring(L);

            xenon_openlib_console(L);
            xenon_openlib_runtime(L);

            register_modules();
        }
    }
}