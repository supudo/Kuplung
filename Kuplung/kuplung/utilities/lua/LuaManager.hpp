//
//  LuaManager.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 14/3/17.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef LuaManager_hpp
#define LuaManager_hpp

#include "kuplung/settings/Settings.h"

#ifdef __cplusplus
#include <lua.hpp>
#else
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#endif

namespace KuplungApp { namespace Utilities { namespace Lua {

class LuaManager {
public:
    void initLua();
    void closeLua();
    void checkLuaErrors();

    void execute(const std::string& fileName);
    static int testFromLua(lua_State *state);

private:
    lua_State *luaState;
};

}}}

#endif /* LuaManager_hpp */
