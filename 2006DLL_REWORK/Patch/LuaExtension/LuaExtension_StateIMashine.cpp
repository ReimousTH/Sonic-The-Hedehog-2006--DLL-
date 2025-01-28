#include "LuaExtension_StateIMashine.h"

#define SMLIB_NAME_META "StateIMachineMeta"
#define SMLIB_NAME "StateIMachine"


#include <Player/State/IMachine.h>


namespace DebugLogV2{


	void GlobalInstall_StateIMachine(lua_State* LS)
	{

		
		lua_register06(LS, SMLIB_NAME, StateIMachine__NEW);
		luaL_newmetatable06(LS, SMLIB_NAME_META);

	
//		lua_pushstring06(LS,"ID"); lua_pushnumber(LS,0); lua_settable06(LS,-3); // ID = 0
//		lua_pushstring06(LS,"ActorPTR"); lua_pushlightuserdata(LS,0); lua_settable06(LS,-3); // ActorPTR = 0
//		lua_pushstring06(LS,"AI"); lua_pushboolean(LS,false); lua_settable06(LS,-3); // AI = 0

	
		lua_newtable06(LS);lua_pushstring06(LS, "__index");luaL_getmetatable06(LS, "MemoryMeta");lua_settable06(LS, -3); lua_setmetatable06(LS, -2); //setmetatable(StateMashineMeta, { __index = MemoryMeta })

		lua_pushstring06(LS, "GetStateID");lua_pushcfunction06(LS, StateIMachine__GetStateID);lua_settable06(LS, -3); // Equivalent to table["GetStateID"] = StateIMachine__SetStateID
		lua_pushstring06(LS, "SetStateID");lua_pushcfunction06(LS, StateIMachine__SetStateID);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = StateIMachine__GetStateID
		lua_pushstring06(LS, "GetStateContext");lua_pushcfunction06(LS, StateIMachine__GetContext);lua_settable06(LS, -3); // Equivalent to table["GetStateContext"] = StateIMachine__GetContext

		
		
		lua_pop(LS,1);
		

		return;
	

	}


	extern "C" int StateIMachine__CreateMetatable(lua_State* L,int ptr){
		


		lua_getglobal06(L, "Memory");
		lua_pushvalue(L,1); //LuaFirstArg
		lua_pcall06(L, 1, 1, 0); // Memory(ptr) -> {}::Memory


		// Second table for metatable set
		lua_newtable06(L);
		lua_pushstring06(L, "__index");luaL_getmetatable06(L, SMLIB_NAME_META);lua_settable06(L, -3); // { __index = MemoryMeta }
		lua_setmetatable06(L, -2); // setmetatable(self, { __index = MemoryMeta })


	


		return 1;
	}
	extern "C" int StateIMachine__NEW(lua_State* L)
	{
	
		StateIMachine__CreateMetatable(L,0);
		return 1;
	}

	

	extern "C" int StateIMachine__SetStateID(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		if (Sonicteam::Player::State::IMachine* IMashine = static_cast<Sonicteam::Player::State::IMachine*>(lua_touserdata(L,-1))){

			int StateID = lua_tonumber(L,2);
			IMashine->ChangeMashineState(StateID);
		}

		return 0;
	}

	extern "C" int StateIMachine__GetStateID(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		if (Sonicteam::Player::State::IMachine* IMashine = static_cast<Sonicteam::Player::State::IMachine*>(lua_touserdata(L,-1))){

			lua_pushnumber(L,IMashine->GetCurrentMashineStateID());
		}

		return 1;
	}

	extern "C" int StateIMachine__GetContext(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);

		void* ptr = 0;
		if (Sonicteam::Player::State::IMachine* IMashine = static_cast<Sonicteam::Player::State::IMachine*>(lua_touserdata(L,-1))){
			
			ptr = (void*)IMashine->GetMashineContext().get();
		}

		lua_getglobal06(L, "Memory");
		lua_pushlightuserdata(L,ptr); //LuaFirstArg
		lua_pcall06(L, 1, 1, 0); // Memory(ptr) -> {}::Memory



		return 1;
	}

}
