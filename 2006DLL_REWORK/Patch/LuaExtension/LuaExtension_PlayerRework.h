#pragma once

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <Hook/HookBase.h>
#include "MessagesUtil.h"
#include <xtl.h>

#include <iostream>
#include <tchar.h>
#include <map>
#include <string>



#include <GameImp.h>
#include <Player/Input/IListener.h>
#include <Player/State/IMachine.h>
#include <Player/State/CommonContext.h>
#include <Player/State/FastContext.h>


#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <Player/ObjectPlayer.h>

#include "LuaExtension_Memory.h"


#include <algorithm>

#define _DWORD DWORD
#define _BYTE BYTE


namespace DebugLogV2 {

	static const char* FixedPlayerFunctionsTables[] = {
		"SetupModule",
		"SetupModule",
		"SetupModuleDebug"

	};

	void GlobalInstall_PlayerRework(lua_State* LS);

	void GetPlayerActorsR(UINT32* pstack,bool AI);
	extern "C" int Player__NEW(lua_State* L);
	extern "C" int PlayerR__GetName(lua_State* L);
	extern "C" int PlayerR__SetName(lua_State* L);
	extern "C" int PlayerR__Reload(lua_State*L);
	extern "C" int PlayerR__SWAP(lua_State*L);

	extern "C" int Vector__CallLUAConstructor(lua_State* L,XMVECTOR* vector);
	extern "C"  XMVECTOR Vector__GetVectorTable(lua_State* L,int idx);

	UINT32 GetPlayerPostureR(UINT32 ObjectPlayer);
	XMVECTOR* GetPlayerPositionR(UINT32 PlayerPosture);

	extern "C" int PlayerR__GetPosition(lua_State* L);
	extern "C" int PlayerR__SetPosition(lua_State* L);
	extern "C" int PlayerR__SetScale(lua_State* L);


	extern "C" int PlayerR__GetActorID(lua_State* L);
	extern "C" int PlayerR__GetActorPTR(lua_State* L);
	extern "C" int PlayerR__SetActorPTR(lua_State* L);

	extern "C" int PlayerR__GetStateID(lua_State* L);
	extern "C" int PlayerR__SetStateID(lua_State* L);
	extern "C" int PlayerR__SetStateIDForce(lua_State* L);


	extern "C" int PlayerR__GetMachine2(lua_State* L);
	extern "C" int PlayerR__GetIPluginByName(lua_State* L);

	extern "C" int PlayerR__OpenPackage(lua_State* L);
	extern "C" int PlayerR__OpenFrame(lua_State* L);
	extern "C" int PlayerR__OpenInput(lua_State* L);

	extern "C" int PlayerR__OpenModel(lua_State* L);
	extern "C" int PlayerR__OpenPostureControl(lua_State* L);
	extern "C" int PlayerR__OpenGravity(lua_State* L);
	extern "C" int PlayerR__OpenState(lua_State* L);
	extern "C" int PlayerR__OpenZock(lua_State* L);
	extern "C" int PlayerR__OpenPerformance(lua_State* L);
	extern "C" int PlayerR__OpenImpulse(lua_State* L);
	extern "C" int PlayerR__OpenPath(lua_State* L);
	extern "C" int PlayerR__OpenLightDash(lua_State* L);
	extern "C" int PlayerR__OpenScore(lua_State* L);
	extern "C" int PlayerR__OpenBodyList(lua_State* L);
	extern "C" int PlayerR__OpenLockOn(lua_State* L);
	extern "C" int PlayerR__OpenGameMaster(lua_State* L);
	extern "C" int PlayerR__OpenEventer(lua_State* L);


	extern "C" int PlayerR__OpenSound(lua_State* L);
	extern "C" int PlayerR__OpenBody(lua_State* L);
	extern "C" int PlayerR__OpenAutomaticDead(lua_State* L);

	extern "C" int PlayerR__OpenWeapons(lua_State* L);
	extern "C" int PlayerR__OpenOther(lua_State* L);
	extern "C" int PlayerR__OpenGauge(lua_State* L);
	extern "C" int PlayerR__OpenOtherParameter(lua_State* L);
	extern "C" int PlayerR__OpenEffect(lua_State* L);


	extern "C" int PlayerR__IDynamicLink(lua_State* L);
	extern "C" int PlayerR__IVarible(lua_State* L);
	extern "C" int PlayerR__IDynamicLink2(lua_State* L);
	extern "C" int PlayerR__IEventerListener(lua_State* L);
	extern "C" int PlayerR__RemovePlugin(lua_State* L);

	


}