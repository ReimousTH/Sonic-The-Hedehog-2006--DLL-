#pragma once


extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}


#include "MessagesUtil.h"
#include <xtl.h>

#include <iostream>
#include <map>

#include <string>

#include "LuaExtension_Memory.h"

#include <Player/Input/IListener.h>
#include <Player/State/IMachine.h>
#include <Player/State/CommonContext.h>
#include <Player/State/FastContext.h>



#include <boost/any.hpp>


#define _DWORD DWORD
#define _BYTE BYTE


namespace DebugLogV2 {
	extern "C" void DebugLabel_GlobalInstallX(lua_State* LS);

	int BitLibGlobalInstall(lua_State* L);
	int BitBaseEXY(lua_State* L, int cs);
	int BIT_ShiftLeftEXY(lua_State* L);
	int BIT_ShiftRightEXY(lua_State* L);
	int BIT_ANDEXY(lua_State* L);
	int BIT_OREXY(lua_State* L);

	


	
}