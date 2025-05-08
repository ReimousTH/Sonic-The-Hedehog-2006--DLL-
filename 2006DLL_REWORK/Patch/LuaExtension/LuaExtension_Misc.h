#ifndef LUAEXTMISC
#define LUAEXTMISC

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <xtl.h>
#include "LuaExtension_Memory.h"


#include <GameImp.h>


namespace Misc {

	Sonicteam::GameImp* GetGameIMP();
	extern "C" size_t GetNumber(lua_State* L,int argument);
	extern "C" XMVECTOR GetVector(lua_State*L, int argument);

}

#endif