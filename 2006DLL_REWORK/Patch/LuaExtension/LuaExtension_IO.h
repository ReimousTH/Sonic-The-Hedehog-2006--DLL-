#pragma once


extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <xtl.h>

#include "LuaExtension_Memory.h"



namespace DebugLogV2 {




	int IO_GlobalInstall(lua_State* L);


	extern "C" int IO__CreateMetatable(lua_State* L);
	extern "C" int IO__NEW(lua_State* L);
	extern "C" int IO__GC(lua_State* L);
	extern "C" int IO_ReadFile(lua_State* L);
	extern "C" int IO_WriteFile(lua_State* L);
	extern "C" int IO_GetSize(lua_State* L);
	extern "C" int IO_Move(lua_State* L);
	extern "C" int Move(lua_State* L);

	extern "C" int IO_CloseFile(lua_State* L);
	extern "C" int IO_GetErrorCode(lua_State* L);
	extern "C" int IO__FREE(lua_State* L);
	extern "C" int IO__MALLOC(lua_State* L);






	
}