#include "LuaExtension_Misc.h"

using namespace Misc;

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
