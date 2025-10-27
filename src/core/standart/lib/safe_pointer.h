#include "type.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

struct SafePointer {
    const Type* type;
    void* value;
    void free_pointer();
};

extern "C" int luaopen_safeptr(lua_State* L);