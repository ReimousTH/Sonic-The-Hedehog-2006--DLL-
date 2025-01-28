#pragma once


extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}


#include <Hook/HookBase.h>
#include "MessagesUtil.h"
#include <xtl.h>

#include <iostream>
#include <tchar.h>
#include <map>
#include <string>





namespace DebugLogV2 {



	int BufferLIB_GlobalInstall(lua_State* LS);



	extern "C" Buffer__NEW(lua_State* L);
	extern "C" Buffer__Free(lua_State* L);


	
}