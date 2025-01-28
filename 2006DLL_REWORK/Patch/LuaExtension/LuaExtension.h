#pragma once


extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <Hook/HookBase.h>

#include <Player/INotification.h>

#include "MessagesUtil.h"
#include <xtl.h>

#include <iostream>
#include <map>

#include <string>

#include "LuaExtension_Memory.h"
#include "LuaExtension_Buffer.h"
#include "LuaExtension_Uint64.h"
#include "LuaExtension_BIT.h"



#include "LuaExtension_OpenState.h"
#include "LuaExtension_PlayerRework.h"
#include "LuaExtension_Vector.h"
#include "LuaExtension_StateIMashine.h"
#include "LuaExtension_StateMachine2.h"

#include "LuaSystem.h"


#include <boost/any.hpp>


#define _DWORD DWORD
#define _BYTE BYTE


namespace DebugLogV2 {
	extern "C" void DebugLabel_GlobalInstallX(lua_State* LS);


	int DebugLabel_GlobalInstall(lua_State* LS);
	int DebugLabel_new(lua_State* L);
	int DebugLabel_delete(lua_State* L);
	int DebugLabel_SetText(lua_State* L);
	int DebugLabel_SetPosition(lua_State* L);



	
	int VectorLIB_GlobalInstall(lua_State* LS);
	extern "C" Vector_NEW(lua_State* L);
	extern "C" Vector__GC(lua_State* L);
	extern "C"  XMVECTOR Vector__GetVector(lua_State* L,int idx);
	extern "C" Vector__tostring(lua_State* L);
	extern "C" Vector__add(lua_State* L);
	extern "C" Vector__sub(lua_State* L);



	extern "C" int GameLIB_PlayerObjSetActorID(int ObjPlayer,int ID);
	extern "C" int GameLIB_DocGamemodeGetActorManager();
	extern "C" int GameLIB_PlayerObjSetActorPTR(int ObjPlayer,int ID);
	extern "C" int GameLIB_PlayerObjToActorID(int ObjPlayer);
	extern "C" UINT32* GameLIB_PlayerObjGetActorPTR(int ObjPlayer);

	int GameLIB_GlobalInstall(lua_State* LS);

	int PlayerLIB_GlobalInstall(lua_State* LS);

	struct Player_NEWS{
		int* ObjectPlayer;
		int Index;
		bool IsAI; //mostly 
		UINT32* ObjectPlayerActorPTR;
	};




	static int Player___GC(lua_State* L);
	extern "C" Player_RELOAD(lua_State* L);
	extern "C" Player_GetPTR(lua_State* L);
	extern "C" Player_NEW(lua_State* L);
	extern "C" Player__GetCamera(lua_State* L);
	extern "C" Player_SWAP(lua_State* L);
	extern "C" Player__GetName(lua_State* L);

	extern "C" Player__GetPosition(lua_State* L);
	extern "C" Player__SetPosition(lua_State* L);




	extern "C" Player__GetRotation(lua_State* L);
	extern "C" Player__SetRotation(lua_State* L);

	extern "C" Player__GetScoreCount(lua_State* L);
	extern "C" Player__GetLiveCount(lua_State* L);
	extern "C" Player__GetRingsCount(lua_State* L);

	extern "C" Player__SetScoreCount(lua_State* L);
	extern "C" Player__SetLiveCount(lua_State* L);
	extern "C" Player__SetRingsCount(lua_State* L);


	extern "C" int Player__GetCurrentStateID(lua_State* L);
	extern "C" int Player__SetCurrentStateID(lua_State* L);








	extern "C" int Player__GetActorID(lua_State* L);
	extern "C" int Player__SetActorID(lua_State* L);
	extern "C" int Player__SetActorPTR(lua_State* L);



	extern "C" Camera__NEW(lua_State* L);
	extern "C" Camera_GetPTR(lua_State* L);
	extern "C" Camera_GetPosition(lua_State* L);
	extern "C" Camera_GetRotation(lua_State* L);



	extern "C" Player_ReloadContext(lua_State* L);
	extern "C" Player_ReloadSound(lua_State* L);
	extern "C" Player_OpenFrame(lua_State* L);
	extern "C" Player_OpenModel(lua_State* L);
	extern "C" Player_OpenPackage(lua_State* L);
	extern "C" Player_OpenInput(lua_State* L);
	extern "C" Player_OpenPostureControl(lua_State* L);



	void GetCameraManActors(UINT32* pstack);
	void GetPlayerActors(UINT32* pstack,bool AI = false);
	UINT32 GetPlayerPosture(UINT32 ObjectPlayer);
	XMVECTOR* GetPlayerPosition(UINT32 PlayerPosture);
	extern "C" Player_NEW(lua_State* L);
	extern "C" static int GetPlayerRawInput(lua_State* L);
	extern "C" int GetPlayerInput(lua_State* L);
	extern "C" int PrintNext(lua_State* L);
	extern "C" int GetPlayerPosition(lua_State* L);
	extern "C" int SetPlayerPosition(lua_State* L);





	int STRLIB_GlobalInstall(lua_State* LS);



	extern "C" int MessageReceiver__NEW(lua_State* L);
	extern "C" int MessageReceiver_GlobalInstall(lua_State* LS);
	extern "C" int MessageReceiver__SendMessage(lua_State* L);

	extern "C" int MainDisplayTask_GlobalInstall(lua_State* LS);
	extern "C" int MainDisplayTask__NEW(lua_State* L);
	extern "C" int MainDisplayTask__SendMessage(lua_State* L);



	extern "C" int GameImp_GlobalInstall(lua_State* LS);
	extern "C" int GameImp_NEW(lua_State* LS);


	extern "C" int PrintNextFixed(std::string msg);

	template <typename T>
	int PrintNextFixed(T msg);





	template <typename T>
	int PrintNextFixed(T msg)
	{
		std::stringstream ss; ss << std::hex << msg;
		return PrintNextFixed(ss.str());
	}

}