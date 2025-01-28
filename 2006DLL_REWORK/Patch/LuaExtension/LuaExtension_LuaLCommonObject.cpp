#include "LuaExtension_LuaLCommonObject.h"

#define LO22LIB_NAME "LObject2"
#define LO22LIB_NAME_META "LObject2Meta"


#include <Player/State/IMachine.h>


namespace DebugLogV2{


#define TRIPLE_QUOTES(...) #__VA_ARGS__
	







	void GlobalInstall_LuaLCommonObject(lua_State* LS)
	{



		
		lua_register06(LS, LO22LIB_NAME, LuaLCommonObject__NEW);
		luaL_newmetatable06(LS, LO22LIB_NAME_META);

	
//		lua_pushstring06(LS,"ID"); lua_pushnumber(LS,0); lua_settable06(LS,-3); // ID = 0
//		lua_pushstring06(LS,"ActorPTR"); lua_pushlightuserdata(LS,0); lua_settable06(LS,-3); // ActorPTR = 0
//		lua_pushstring06(LS,"AI"); lua_pushboolean(LS,false); lua_settable06(LS,-3); // AI = 0

	
		//lua_newtable06(LS);lua_pushstring06(LS, "__index");luaL_getmetatable06(LS, "MemoryMeta");lua_settable06(LS, -3); lua_setmetatable06(LS, -2); //setmetatable(StateMashineMeta, { __index = MemoryMeta })

	//	lua_pushstring06(LS, "GetIMachine");lua_pushcfunction06(LS, StateIMachine__SetStateID);lua_settable06(LS, -3); // Equivalent to table["GetIMachine"] = StateIMachine__SetStateID
	//	lua_pushstring06(LS, "AddState");lua_pushcfunction06(LS, StateIMachine__GetStateID);lua_settable06(LS, -3); // Equivalent to table["AddState"] = StateIMachine__GetStateID
	//	lua_pushstring06(LS, "RemoveStateByID");lua_pushcfunction06(LS, StateIMachine__GetStateID);lua_settable06(LS, -3); // Equivalent to table["RemoveStateByID"] = StateIMachine__GetStateID
		

		
		lua_pop(LS,1);
		

		lua_dostring06(LS,"LObject2Meta = {OnStart=function(self) end,OnUpdate=function(self,delta) end,OnEnd=function(self) end,PreUpdateCmn=function(self,delta) end,PostUpdateCmn = function(self,delta) end}");



		lua_dostring06(LS,TRIPLE_QUOTES(
			function CommonObjectRef(arg0,arg1)
			local self = Memory(arg0)
			self.context = arg1
			end
			));
		lua_dostring06(LS,"""function CommonObjectRef(arg0,arg1,arg2,arg3)\
			local self = Memory(arg0)\
			self.IMachine = StateIMachine(arg1)\
			self.Context = Memory(arg2)\
			self.Fields = Memory(arg3)\
			return self\
			end""");

	

		return;
	

	}


	extern "C" int LuaLCommonObject__CreateMetatable(lua_State* L,int ptr){
		
		
		lua_newtable06(L); // Create a new table for the instance // local self = {}

		// Second table for metatable set
		lua_newtable06(L);
		lua_pushstring06(L, "__index");luaL_getmetatable06(L, LO22LIB_NAME_META);lua_settable06(L, -3); // { __index = LO22LIB_NAME_META }
		lua_setmetatable06(L, -2); // setmetatable(self, { __index = MemoryMeta })


		lua_pushstring06(L,"OnStart");lua_gettable(L,1); if (lua_isfunction(L,-1)) {lua_pushstring06(L,"OnStart");lua_pushvalue(L,-2); lua_settable06(L,-4);}
		lua_pushstring06(L,"OnUpdate");lua_gettable(L,1); if (lua_isfunction(L,-1)) {lua_pushstring06(L,"OnUpdate");lua_pushvalue(L,-2); lua_settable06(L,-4);}
		lua_pushstring06(L,"OnEnd");lua_gettable(L,1); if (lua_isfunction(L,-1)) {lua_pushstring06(L,"OnEnd");lua_pushvalue(L,-2); lua_settable06(L,-4);}
		
//		lua_pushstring06(L, "ptr");lua_pushlightuserdata(L, (void*)value);lua_settable06(L, -3); // self.ptr = ptr
//		lua_pushstring06(L, "move");lua_pushlightuserdata(L, (void*)move);lua_settable06(L, -3); // self.move = move


	


		return 1;
	}
	extern "C" int LuaLCommonObject__NEW(lua_State* L)
	{
		LuaLCommonObject__CreateMetatable(L,0);
		return 1;
	}

	




	

}
