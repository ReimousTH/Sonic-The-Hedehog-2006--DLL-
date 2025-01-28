#pragma once


extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}


#include "MessagesUtil.h"
#include <xtl.h>

#include <iostream>
#include <tchar.h>
#include <map>
#include <string>

#include <Hook/HookBase.h>
#include <Player/State/Machine2.h>




#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>



namespace DebugLogV2 {



	extern int OpenStateExEnabled;

	struct LEOS_Store{
		int L;
		int L_Ref;
	};

	extern std::map<int,std::map<int, std::map<int, std::map<int, std::vector<LEOS_Store>>>>> LEOS_CACHE_STATE;
	extern bool LEOS_CACHE_STATE_LOCK;

	int OpenState_GlobalInstall(lua_State* LS);

	extern "C" OpenStateRework(lua_State* L);
	void OnStateChangeRework(Sonicteam::Player::State::Machine2* _this,boost::shared_ptr<Sonicteam::SoX::AI::State<Sonicteam::Player::State::IContext>> NextState,Sonicteam::Player::State::IContext* context);
	extern "C" Clear(lua_State* L);
	extern "C" Connect(lua_State* L);


	extern "C" int OpenState__CreateMetatable(lua_State* L);

	extern "C" int OpenStateConnect__CreateMetatable(lua_State* L,int user_data,int c_module_state,int StateID, int StateWHEN,int StateFuncFromLua);



	extern "C" int DisconnectLeosState(int user_data,int c_module_state,int StateID,int StateWHEN,int StateFuncFromLua );
	extern "C" int OpenStateConnectDisconnect(lua_State* L);
	extern "C" int OpenStateConnect__NEW(lua_State* L);
	extern "C" int OpenState__NEW(lua_State* L);

	extern "C" int OpenState__GC(lua_State* L);






	
}