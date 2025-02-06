#pragma once

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}


#include "MessagesUtil.h"
#include <xtl.h>

#include <iostream>
#include <tchar.h>
#include <map>
#include <string>



#include <Player/Input/IListener.h>
#include <Player/State/IMachine.h>
#include <Player/State/CommonContext.h>
#include <Player/State/FastContext.h>
#include <boost/any.hpp>

#include <Player/State/CommonObject.h>
#include "LuaExtension_Memory.h"




#define _DWORD DWORD
#define _BYTE BYTE


namespace DebugLogV2 {



	
	void GlobalInstall_LuaLCommonObject(lua_State* LS);


	extern "C" int LuaLCommonObject__NEW(lua_State* L);

	extern "C" int LuaLCommonObject__GetIMachine(lua_State*);



	


}