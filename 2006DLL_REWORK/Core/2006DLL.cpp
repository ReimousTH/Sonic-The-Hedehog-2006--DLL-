#include "2006DLL.h"
#include <string>


#include <Patch/Common/XEXALLOCATORS.H>
#include <Patch/Common/XEXALLOCATORS_UNDEF.H>


struct FPair{
	const char* Name;
	void(*Function)();
};


FPair functions[] = {
	{"LuaExtension", DebugLogV2::GlobalInstall},
	{"DevTitleV2", DevTitleV2::GlobalInstall},
	{"NoArcMode", ArcRemover::GlobalInstall},
	{"TimeSystemRestore", TimeSystem::GlobalInstall},
	{"CompleteGauge", CompleteGauge::GlobalInstall},
	{"AmyLOS", AmyLOS::GlobalInstall},
	{"TagBattleExtension", TagBattleMain::GlobalInstall},
};



void STH2006DLLMain()
{

	HookV2::IsNotEmulatedHardWare =  HookV2::CheckIsNotEmulatedHardWare();

	ZLua lua_file = ZLua("game:\\common\\DLL.lua");
	std::stringstream log;
	std::wstringstream wlog;
	lua_file.DoFile(true);

	for (int i = 0;i<sizeof(functions)/sizeof(FPair);i++){
		if (lua_file.GetGlobalBool(functions[i].Name) == true){
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
