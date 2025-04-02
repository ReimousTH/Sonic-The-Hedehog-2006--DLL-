#pragma once


extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <xtl.h>

#define _DWORD DWORD
#define _BYTE BYTE


namespace DebugLogV2 {



	int XMMATRIX_GlobalInstall(lua_State* LS);

	extern "C" XMMATRIX_CREATEMETATABLE(lua_State* L,XMMATRIX mat);
	extern "C" XMMATRIX_NEW(lua_State* L);

	XMMATRIX XM_GetMatrix(lua_State* L, int index);
	extern "C" int XMMATRIX_Scaling(lua_State* L);
	extern "C" int XMMATRIX_Transformation(lua_State* L);

	extern "C" int index_handler(lua_State* L) ;
	extern "C" int newindex_handler(lua_State* L);



	
}