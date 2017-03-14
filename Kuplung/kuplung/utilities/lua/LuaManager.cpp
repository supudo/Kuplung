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

void LuaManager::evalLuaFile(const std::string& fileName) {
    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[LuaManager] Evaluating file: %s\n", fileName.c_str()));
    luaL_dofile(this->luaState, fileName.c_str());
    this->checkLuaErrors();
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
