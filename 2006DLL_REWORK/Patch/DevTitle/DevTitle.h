#pragma once


#include <Hook/HookBase.h>
#include <Patch/LuaExtension/MessagesUtil.h>
#include <string>

#include <DocMarathonImp.h>

#include <vector>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}


namespace DevTitleV2{

	static UINT32 Label1;
	static UINT32 Label2;
	static UINT32 Label3;
	static UINT32 Label4;
	void GlobalInstall();

}


