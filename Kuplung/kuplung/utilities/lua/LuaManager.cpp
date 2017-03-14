//
//  LuaManager.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 14/3/17.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "LuaManager.hpp"

namespace KuplungApp { namespace Utilities { namespace Lua {

void LuaManager::initLua() {
    this->luaState = luaL_newstate();
    luaL_openlibs(this->luaState);
}

void LuaManager::execute(const std::string& fileName) {
    int lua_result;
    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[LuaManager] Evaluating file: %s", fileName.c_str()));
    lua_register(
                this->luaState,
                "testFromLua",
                KuplungApp::Utilities::Lua::LuaManager::testFromLua
                );
    lua_result = luaL_loadfile(this->luaState, fileName.c_str());
    if (lua_result != LUA_OK) {
        this->checkLuaErrors();
        return;
    }
    lua_result = lua_pcall(this->luaState, 0, LUA_MULTRET, 0);
    if (lua_result != LUA_OK) {
        this->checkLuaErrors();
        return;
    }
}

#ifdef __cplusplus
extern "C"
#endif
int LuaManager::testFromLua(lua_State* state) {
    // The number of function arguments will be on top of the stack.
    int args = lua_gettop(state);

    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[LuaManager] This was called from Lua with %d arguments!", args));

    for (int n=1; n <= args; ++n) {
        Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[LuaManager]  Argument %d: '%s'", n, lua_tostring(state, n)));
    }

    // Push the return value on top of the stack. NOTE: We haven't popped the
    // input arguments to our function. To be honest, I haven't checked if we
    // must, but at least in stack machines like the JVM, the stack will be
    // cleaned between each function call.
    lua_pushnumber(state, 123);

    // Let Lua know how many return values we've passed
    return 1;
}

void LuaManager::checkLuaErrors() {
    const char* message = lua_tostring(this->luaState, -1);
    if (message != NULL)
        Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[LuaManager] Error occured: %s!\n", message));
    lua_pop(this->luaState, 1);
}

void LuaManager::closeLua() {
    lua_close(this->luaState);
}

}}}
