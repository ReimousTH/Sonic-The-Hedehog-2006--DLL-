#pragma once


#define ALTERNATIVE_UTIL_MSG


extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}


#include <vector>
#include <xtl.h>


#include <Hook/HookBase.h>
#include <DocMarathonImp.h>

//#include "AtgInput.h"
#include <sstream>
#include <string>
#include <AtgInput.h>

#define  _BYTE char

namespace DebugLogV2{

	extern int ThreadLog;


	static std::vector<int> DebugMessagesPlace;
	static std::vector<int> DebugMessages;
	static XMFLOAT4 ScrollPosition;

	void ChangeMessagePosition(UINT32 TextEntity,float x,float y);
	void ChangeMessagePositionY(UINT32 TextEntity,float y);
	void EditMessage(UINT32 TextEntity,const wchar_t* msg);
	void EditMessage(UINT32 TextEntity,const wchar_t* msg,const char* pmsg);
	void EditMessageColor(UINT32 TextEntity,char Alpha,char Red,char Green,char Blue);

	UINT32 SpawnMessageBase(float pos_x,float pos_y,const wchar_t* MSG,const char* PMSG,int FontIndex,int ColorFlag);
	UINT32 SpawnMessage(const wchar_t* msg,float pos_x,float pos_y);
	UINT32 SpawnMessage(const wchar_t* msg,float pos_y);
	UINT32 SpawnMessage(const wchar_t* msg,const char* picturemsg,float pos_x,float pos_y);


	//OriginalLog
	
	extern std::vector<std::string> log;

	void MessageUtilGlobalInstall(lua_State* L);

}

