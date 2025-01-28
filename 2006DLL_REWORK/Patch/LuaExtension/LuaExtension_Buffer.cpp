#include "LuaExtension_Buffer.h"

namespace DebugLogV2{



	int BufferLIB_GlobalInstall(lua_State* L)
	{
	

		lua_register06(L, "Buffer", Buffer__NEW);
		luaL_newmetatable06(L, "BufferMeta");
		lua_pushstring06(L,"count"); lua_pushnumber(L,0); lua_settable06(L,-3); 
		lua_pushstring06(L,"__index"); lua_pushvalue(L,-2); lua_settable06(L,-3); // __index = BufferMeta

		lua_newtable06(L);lua_pushstring06(L, "__index");luaL_getmetatable06(L, "MemoryMeta");lua_settable06(L, -3); lua_setmetatable06(L, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })
		
		lua_pushstring06(L, "Free"); lua_pushcfunction06(L, Buffer__Free); 	lua_settable06(L, -3);

		

		lua_pop(L,1);

		



		return 0;
	}

	extern "C" Buffer_WatchArgs(lua_State* L,int args){

	}

	extern "C" Buffer__NEW(lua_State* L){
		
		int args = lua_gettop(L);
		std::vector<int> buffer; 
		for (int i = 1; i <= args; i++) {
			if (lua_isnumber(L, i)) {
				buffer.push_back((int)lua_tonumber(L, i));
			}
			else if (lua_istable(L,i)){
				
				lua_getmetatable06(L,i);
				lua_getglobal06(L,"VectorMeta"); // because do_string 
		
				if (lua_rawequal(L,-1,-2)){

					for (int k = 1; k <= 4; k++) {
						lua_rawgeti06(L, i, k); // Get value at index i

						if (lua_isnumber(L, -1)) {
							float value = lua_tonumber(L, -1);
							
							buffer.push_back(*(int*)&value);
						}
						lua_pop(L, 1);
					}
				}

				lua_pop(L,1);

			}
			else if (lua_isuserdata(L, i)) {
				buffer.push_back((int)lua_touserdata(L, i));
			}
		}

		void* fbuffer = (void*)malloc06(buffer.size() * sizeof(int));
		std::memcpy(fbuffer, &buffer[0], buffer.size() * sizeof(int));

		lua_getglobal06(L, "Memory");
		lua_pushlightuserdata(L, fbuffer);
		lua_pcall06(L, 1, 1, 0);


		luaL_getmetatable06(L, "BufferMeta");
		lua_setmetatable06(L, -2);

		lua_pushstring06(L, "count");
		lua_pushnumber(L, args);
		lua_settable06(L, -3);

		return 1;

	}
	extern "C" Buffer__Free(lua_State* L){

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		int ptr =  (int)lua_touserdata(L,-1);
		free06(ptr);

	}



}