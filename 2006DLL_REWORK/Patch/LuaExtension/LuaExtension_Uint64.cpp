#include "LuaExtension_Uint64.h"

namespace DebugLogV2{

	


	int Uint64LIB_GlobalInstall(lua_State* L)
	{

		luaL_newmetatable06(L, "Uint64Meta");
		lua_pushstring06(L,"__index"); lua_pushvalue(L,-2); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pushstring06(L,"part1"); lua_pushlightuserdata(L,(void*)0); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pushstring06(L,"part2"); lua_pushlightuserdata(L,(void*)0); lua_settable06(L,-3); // __index = MemoryMeta


		//lua_pushstring06(L, "Move"); lua_pushcfunction06(L, Memory__Move); 	lua_settable06(L, -3);


		lua_pop(L,1);
		lua_register06(L,"Uint64",Uint64__NEW);


		return 0;
	}


	extern "C" Uint64__CreateMetatable(lua_State* L,int value,int move){


		lua_newtable06(L); 
		luaL_getmetatable06(L, "Uint64Meta"); 
		lua_setmetatable06(L, -2);

	
		lua_pushstring06(L, "part1"); // 
		lua_pushlightuserdata(L, (void*)value);  
		lua_settable06(L, -3);        // Set table[-3]["part1"] = value

		lua_pushstring06(L, "part2"); //
		lua_pushlightuserdata(L, (void*)move);  
		lua_settable06(L, -3);        // Set table[-3]["part2"] = move

		return 1; // Return the new table (the instance)

	}


	extern "C" int LuaArgToVoid(lua_State*L,int arg_num){

		int value = 0;
		if (lua_isuserdata(L,arg_num)){
			value = (int)lua_touserdata(L,arg_num);
		}
		else if (lua_isnumber(L,arg_num)){
			value = lua_tonumber(L,arg_num);
		}
		else if (lua_isstring(L,arg_num)){

			const char* s = lua_tostring(L,arg_num);
			const TCHAR* hexString = _T(s); // Hex string to convert
			value = _tcstoul(hexString, NULL, 16); // Convert hex string to unsigned long integer
		}
		else{
			value = lua_tonumber(L,arg_num);
		}
		return value;
	}

	extern "C" Uint64__NEW(lua_State* L){
		int arg_1 = LuaArgToVoid(L,1);
		int arg_2 = LuaArgToVoid(L,2);
		Uint64__CreateMetatable(L,arg_1,arg_2);
		return 1;
	}



}