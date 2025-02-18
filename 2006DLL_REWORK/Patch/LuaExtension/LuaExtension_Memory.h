#pragma once


extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <Core/Debug.h>

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


#define _DWORD DWORD
#define _BYTE BYTE


namespace DebugLogV2 {



	int MemoryLIB_GlobalInstall(lua_State* LS);



	extern "C" Memory__NEW(lua_State* L);


	extern "C" int Memory__FMove(lua_State* L);
	extern "C" Memory__Move(lua_State* L);
	extern "C" Memory__eq(lua_State* L);
	extern "C" Memory__neq(lua_State* L);

	extern "C" int Memory__OR(lua_State* L);
	extern "C" int Memory__AND(lua_State* L);
	extern "C" int Memory__add(lua_State* L);
	extern "C" int Memory__SBL(lua_State* L);
	extern "C" int Memory__SBR(lua_State* L);



	extern "C" Memory__SetDWORD(lua_State* L);
	extern "C" Memory__SetFLOAT(lua_State* L);
	extern "C" Memory__SetBYTE(lua_State* L);
	extern "C" Memory__SetPointer(lua_State* L);
	extern "C" Memory__SetVector(lua_State* L);
	extern "C" Memory__SetString(lua_State* L);

	extern "C" int Memory__GetDWORD(lua_State* L);
	extern "C" int Memory__GetFLOAT(lua_State* L);
	extern "C" int Memory__GetBYTE(lua_State* L);
	extern "C" int Memory__GetPointer(lua_State* L);
	extern "C" int Memory__GetVector(lua_State* L);

	extern "C" int Memory__GetPTR(lua_State* L);

	extern "C" int Memory__GetClassName(lua_State* L);

	extern "C" int Memory__IsValidPTR(lua_State* L);
	extern "C" Memory__CreateMetatableFields(lua_State* L,int value,int move);
	extern "C" Memory__CreateMetatable(lua_State* L,int value,int move);


	extern "C" Memory_GetPointer(lua_State* L);
	extern "C" Memory_ADD(lua_State* L);
	extern "C" Memory_SetPointerValue(lua_State* L);
	extern "C" Memory_GetDWORD(lua_State* L);
	extern "C" Memory_SetDWORD(lua_State* L);
	extern "C" Memory_GetFLOAT(lua_State* L);
	extern "C" Memory_SetFLOAT(lua_State* L);
	extern "C" Memory_GetBYTE(lua_State* L);
	extern "C" Memory_SetBYTE(lua_State* L);


	extern "C" Memory_QueryProtect(lua_State* L);

	

	extern "C" Memory_CallFuncF(lua_State* L);
	extern "C" Memory_CallFunc(lua_State* L);
	extern "C" Memory_AsInt(lua_State* L);
	extern "C" Memory_AsFloat(lua_State* L);


	
}