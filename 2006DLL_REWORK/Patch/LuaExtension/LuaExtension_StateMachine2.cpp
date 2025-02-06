#include "LuaExtension_StateMachine2.h"



#define SM2LIB_NAME_META "StateIMachineMeta"
#define SM2LIB_NAME "StateIMachine"


#include <Player/State/IMachine.h>
#include <Player/State/IContext.h>
#include <boost/weak_ptr.hpp>

#define SM2_OnStateStart 1
#define SM2_OnStateUpdate 2
#define SM2_OnStateEnd 3

#define SM2_OnStateStartP 4
#define SM2_OnStateUpdateP 5
#define SM2_OnStateEndP 6



namespace DebugLogV2{



	void GlobalInstall_StateMachine2(lua_State* LS)
	{
		if (LS == 0){


			return;
		}


		lua_register06(LS, SM2LIB_NAME, StateMachine2__NEW);
		luaL_newmetatable06(LS, SM2LIB_NAME_META);


		lua_newtable06(LS);lua_pushstring06(LS, "__index");luaL_getmetatable06(LS, "MemoryMeta");lua_settable06(LS, -3); lua_setmetatable06(LS, -2); //setmetatable(StateMashineMeta, { __index = MemoryMeta })

		lua_pushstring06(LS, "GetIMachine");lua_pushcfunction06(LS, StateIMachine__GetIMachine);lua_settable06(LS, -3); // Equivalent to table["GetIMachine"] = StateIMachine__SetStateID
		lua_pushstring06(LS, "AddState");lua_pushcfunction06(LS, StateIMachine__AddState);lua_settable06(LS, -3); // Equivalent to table["AddState"] = StateIMachine__GetStateID
		lua_pushstring06(LS, "RemoveStateByID");lua_pushcfunction06(LS, StateIMachine__RemoveStateByID);lua_settable06(LS, -3); // Equivalent to table["RemoveStateByID"] = StateIMachine__GetStateID
		lua_pushstring06(LS, "OnStateConnect");lua_pushcfunction06(LS, StateMachine2__OnStateConnect);lua_settable06(LS, -3); // Equivalent to table["RemoveStateByID"] = StateIMachine__GetStateID


		lua_pop(LS,1);


		lua_register06(LS,"LuaCommonStateConnectRef",LuaCommonStateConnectRefConstructor);



		lua_State* L = LS;

		luaL_newmetatable06(L, "LuaCommonStateConnectRefMeta");
		lua_pushstring06(L,"__index"); lua_pushvalue(L,-2); lua_settable06(L,-3); // __index = MemoryMeta
	//	lua_pushstring06(L,"ptr"); lua_pushlightuserdata(L,(void*)0); lua_settable06(L,-3); // __index = MemoryMeta


		lua_pushstring06(L, "Disconnect"); lua_pushcfunction06(L,LuaCommonStateConnectRef__Disconnect);lua_settable06(L, -3);




		lua_pop(LS,1);



		return;


	}

	extern "C" int LuaCommonStateConnectRefConstructor(lua_State* L){

		lua_newtable06(L); // Create a new table for the instance // local self = {}

		// Second table for metatable set
		//	lua_newtable06(L);
		//	lua_pushstring06(L, "__index");luaL_getmetatable06(L, "MemoryMeta");lua_settable06(L, -3); // { __index = MemoryMeta }
		//	lua_setmetatable06(L, -2); // setmetatable(self, { __index = MemoryMeta })

		luaL_getmetatable06(L, "LuaCommonStateConnectRefMeta");
		lua_setmetatable06(L, -2); // setmetatable(self, { __index = MemoryMeta })


		lua_pushstring06(L, "StateIDType");lua_pushvalue06(L,1);lua_settable06(L, -3); // self.ptr = ptr
		lua_pushstring06(L, "StateID");lua_pushvalue06(L,2);lua_settable06(L, -3); // self.ptr = ptr
		lua_pushstring06(L, "FunctionRef");lua_pushvalue06(L,3);lua_settable06(L, -3); // self.ptr = ptr
		lua_pushstring06(L, "IMachinePTR");lua_pushvalue06(L,4);lua_settable06(L, -3); // self.ptr = ptr




		return 1;

	}
	extern "C" int LuaCommonStateConnectRef__Disconnect(lua_State* L){

		return 1;
	}






	extern "C" int StateMachine2__CreateMetatable(lua_State* L,int ptr){

		return 1;
	}
	extern "C" int StateMachine2__NEW(lua_State* L)
	{
		StateMachine2__CreateMetatable(L,0);
		return 1;
	}





	extern "C" int StateIMachine__GetIMachine(lua_State* L)
	{
		return 1;
	}

	extern "C" int StateIMachine__AddState(lua_State* L)
	{

		return 0;
	}

	extern "C" int StateIMachine__RemoveStateByID(lua_State* L)
	{
		return 0;
	}

	extern "C" int StateMachine2__OnStateConnect(lua_State*L)
	{
	
		return 1;
	}

}
