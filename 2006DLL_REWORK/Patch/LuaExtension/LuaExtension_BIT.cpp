#include "LuaExtension_BIT.h"

namespace DebugLogV2{

	static const struct luaL_reg BitLIBX [] = {
		{"AND", BIT_ANDEXY},
		{"OR", BIT_OREXY},
		{"SR", BIT_ShiftRightEXY},
		{"SL", BIT_ShiftLeftEXY},
		{NULL,NULL}
	};


	int BitLibGlobalInstall(lua_State*L)
	{
		luaL_openlib06(L,"bit",BitLIBX,0);
		return 1;

	}

	int BitBaseEXY(lua_State*L,int cs)
	{

		int v1 = lua_tonumber(L,1);
		int v2 = lua_tonumber(L,2);
		int result = 0;
		switch (cs){

			case 0:
				result = v1 << v2;
				break;
			case 1:
				result = v1 >> v2;
				break;
			case 2:
				result = v1 & v2;
				break;
			case 3:
				result = v1 | v2;
				break; 

			default:
				lua_pushnil(L);
				return 1;
		}
		lua_pushnumber(L,result);
		return 1;
	}

	int BIT_ShiftLeftEXY(lua_State* L)
	{
		return BitBaseEXY(L,0);
	}

	int BIT_ShiftRightEXY(lua_State* L)
	{
		return BitBaseEXY(L,1);
	}

	int BIT_ANDEXY(lua_State* L)
	{
		return BitBaseEXY(L,2);
	}

	int BIT_OREXY(lua_State* L)
	{
		return BitBaseEXY(L,3);
	}












}