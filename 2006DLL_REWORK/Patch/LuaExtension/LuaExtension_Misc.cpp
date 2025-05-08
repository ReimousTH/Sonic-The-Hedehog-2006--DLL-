#include "LuaExtension_Misc.h"

using namespace Misc;


extern "C" XMVECTOR Misc::GetVector(lua_State*L, int argument){
	XMVECTOR vector1 = {0,0,0,1};
	if (lua_istable(L,argument))
		for (int i = 1; i <= 4; ++i) {
			lua_rawgeti06(L, argument, i); // Get value at index i

			if (lua_isnumber(L, -1)) {
				float value = lua_tonumber(L, -1);
				vector1.v[i-1] = value;
			}

			// Pop the value from the stack
			lua_pop(L, 1);
		}
		return vector1;
}


Sonicteam::GameImp* Misc::GetGameIMP()
{
	Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
	return *(Sonicteam::GameImp**)(impl->DocCurrentMode + 0x6C);
}

extern "C" size_t Misc::GetNumber(lua_State* L,int argument)
{

	size_t return_value = 0;

	switch (lua_type(L,argument)){
		case LUA_TNUMBER:
			return_value = (size_t)lua_tonumber(L,argument);
			break;
		case LUA_TBOOLEAN:
			return_value = (size_t)lua_toboolean(L,argument);
			break;
		case LUA_TLIGHTUSERDATA:
		case LUA_TUSERDATA:
			return_value = (size_t)lua_touserdata(L,argument);
			break;
		case LUA_TTABLE:
			{
				lua_getmetatable(L,argument);
				lua_getglobal06(L,"MemoryMeta");
				if (lua_rawequal(L,-1,-2)){

					lua_pushstring(L,"ptr");
					lua_gettable(L,argument);
					return_value = (size_t)lua_touserdata(L,-1);
					lua_pop(L,1);
				}
				lua_pop(L,1); //MemoryMeta
				//other


				lua_pop(L,1); //lua_getmetatable

			}
			break;
		default:
			break;
	}


	return return_value;
}
