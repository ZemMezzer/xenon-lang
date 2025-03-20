project(xenon-core)

add_subdirectory(Lua)

set(ENGINE_SOURCES
        Engine/LuaState.cpp
        Engine/LuaState.hpp
        Engine/LuaTBoolean.cpp
        Engine/LuaTBoolean.hpp
        Engine/LuaTNil.cpp
        Engine/LuaTNil.hpp
        Engine/LuaTNumber.cpp
        Engine/LuaTNumber.hpp
        Engine/LuaTString.cpp
        Engine/LuaTString.hpp
        Engine/LuaTTable.cpp
        Engine/LuaTTable.hpp
        Engine/LuaTUserData.cpp
        Engine/LuaTUserData.hpp
        Engine/LuaType.cpp
        Engine/LuaType.hpp)

set(REGISTRY_SOURCES
        Registry/LuaCFunction.cpp
        Registry/LuaCFunction.hpp
        Registry/LuaCodeSnippet.cpp
        Registry/LuaCodeSnippet.hpp
        Registry/LuaCompiler.cpp
        Registry/LuaCompiler.hpp
        Registry/LuaLibrary.cpp
        Registry/LuaLibrary.hpp
        Registry/LuaRegistry.cpp
        Registry/LuaRegistry.hpp)

set(CORE_SOURCES
        LuaContext.cpp
        Include/LuaContext.hpp
        Include/LuaCpp.hpp
        LuaMetaObject.cpp
        Include/LuaMetaObject.hpp
        LuaVersion.cpp
        Include/LuaVersion.hpp)

add_library(${PROJECT_NAME} STATIC ${ENGINE_SOURCES} ${REGISTRY_SOURCES} ${CORE_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC Engine)
target_include_directories(${PROJECT_NAME} PUBLIC Lua)
target_include_directories(${PROJECT_NAME} PUBLIC Registry)
target_include_directories(${PROJECT_NAME} PUBLIC Include)

target_link_libraries(${PROJECT_NAME} PUBLIC xenon-lua)