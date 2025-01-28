#include "2006DLL.h"
#include "Patch/LuaExtension/DebugLog.h"
void STH2006DLLMain()
{

	HookV2::IsNotEmulatedHardWare =  HookV2::CheckIsNotEmulatedHardWare();
	ZLua lua_file = ZLua("game:\\common\\DLL.lua");
	lua_file.DoFile(true);
	
	DebugLogV2::GlobalInstall();
	//PushXenonMessage(L"MSG","Test");
}
