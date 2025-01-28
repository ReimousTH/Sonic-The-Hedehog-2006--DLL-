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
#include <Player/Input/IListener.h>
#include <Player/State/IMachine.h>
#include <Player/State/CommonContext.h>
#include <Player/State/FastContext.h>
#include <boost/any.hpp>

#include "LuaExtension_LuaLCommonObject.h"

#include "LuaExtension_Memory.h"




#define _DWORD DWORD
#define _BYTE BYTE


namespace DebugLogV2 {


	void GlobalInstall_StateMachine2(lua_State* LS);


	extern "C" int StateMachine2__NEW(lua_State* L);

	extern "C" int StateIMachine__GetIMachine(lua_State*);
	extern "C" int StateIMachine__AddState(lua_State*);
	extern "C" int StateIMachine__RemoveStateByID(lua_State*);

	extern "C" int StateMachine2__OnStateConnect(lua_State*L);


	extern "C" int LuaCommonStateConnectRefConstructor(lua_State* L);
	extern "C" int LuaCommonStateConnectRef__Disconnect(lua_State* L);

	


	


}