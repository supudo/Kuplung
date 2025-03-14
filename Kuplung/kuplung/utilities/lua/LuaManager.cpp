//
//  LuaManager.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 14/3/17.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "LuaManager.hpp"

namespace KuplungApp { namespace Utilities { namespace Lua {

LuaManager::LuaManager() {
	// this->luaState = luaL_newstate();
}

void LuaManager::initLua() {
  // luaL_openlibs(this->luaState);
}

void LuaManager::execute(const std::string& fileName) {
//     int lua_result;
//     Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[LuaManager] Evaluating file: ", fileName.c_str()));
//     lua_register(
//                 this->luaState,
//                 "testFromLua",
//                 KuplungApp::Utilities::Lua::LuaManager::testFromLua
//                 );
//     lua_result = luaL_loadfile(this->luaState, fileName.c_str());
//     if (lua_result != 0) {
//         this->checkLuaErrors();
//         return;
//     }
//     lua_result = lua_pcall(this->luaState, 0, LUA_MULTRET, 0);
//     if (lua_result != 0) {
//         this->checkLuaErrors();
//         return;
//     }
// }

// #ifdef __cplusplus
// extern "C"
// #endif
// int LuaManager::testFromLua(lua_State* state) {
//     int args = lua_gettop(state);
//     Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[LuaManager] This was called from Lua with ", args, " arguments!"));
//     for (int n=1; n <= args; ++n) {
//         Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[LuaManager]  Argument ", n, ": '", lua_tostring(state, n), "'"));
//     }
//     lua_pushnumber(state, 123);
//     return 1;
}

void LuaManager::checkLuaErrors() {
  // const char* message = lua_tostring(this->luaState, -1);
  // if (message != NULL)
  //     Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[LuaManager] Error occured: ", message, "!\n"));
  // lua_pop(this->luaState, 1);
}

void LuaManager::closeLua() {
  // lua_close(this->luaState);
}

}}}
