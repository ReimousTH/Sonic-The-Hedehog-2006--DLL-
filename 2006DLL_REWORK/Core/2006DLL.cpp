#include "2006DLL.h"
#include <string>

#include "Patch/LuaExtension/DebugLog.h"


struct FPair{
	const char* Name;
	void(*Function)();
};


FPair functions[] = {
	{"LuaExtension", DebugLogV2::GlobalInstall},
};



void STH2006DLLMain()
{

	HookV2::IsNotEmulatedHardWare =  HookV2::CheckIsNotEmulatedHardWare();

	ZLua lua_file = ZLua("game:\\common\\DLL.lua");
	std::stringstream log;
	std::wstringstream wlog;
	lua_file.DoFile(true);

	for (int i = 0;i<sizeof(functions)/sizeof(FPair);i++){
		if (lua_file.GetGlobalBool(functions[0].Name)){
			functions[i].Function();
			log << functions[i].Name << "\n";
		}
	}
	wlog << "DLL [" << __DATE__ << " ; " << __TIME__ << "]" << " Patch List ";

	XOVERLAPPED overlap;
	overlap.hEvent = CreateEvent(0,false,false,0);
	PushXenonMessage(wlog.str().c_str(),log.str().c_str(),&overlap);
	WaitForSingleObject(overlap.hEvent,INFINITE);
	CloseHandle(overlap.hEvent);

	//PushXenonMessage(L"MSG","Test");
}
