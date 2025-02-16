#include "2006DLL.h"
#include <string>


#include <Patch/Common/XEXALLOCATORS.H>
#include <Patch/Common/XEXALLOCATORS_UNDEF.H>
#include <Patch/FileSystemNew/FileSystemNew.h>
#include <Patch/TailsGauge/TailsGauge.h>
#include <Patch/OmegaGauge/OmegaGauge.h>



#include <Sox/Perfomance.h>
#include <Sox/StepableThread.h>
#include <Sox/Engine/Task.h>
#include <Hook/HookNew.h>
#include <vector>


#include <Sox/ResourceManager.h>
#include <Sox/ExFileSystem.h>
#include <System/Singleton.h>
#include <System/CreateStatic.h>


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
	//{"FileSystemNew", FileSystemNew::GlobalInstall},
	{"TailsGauge", TailsGauge::GlobalInstall},
	{"OmegaHoverGauge",OmegaGauge::GlobalInstall}
};


void* myFunc1(double value) {

	
	if (ATG::Input::GetMergedInput()->wPressedButtons & XINPUT_GAMEPAD_Y){
	
	}
	return 0;
}


void* myFunc2() {

	
	
	return 0;
}


HOOKV3(0x828CBDC0, int, TestFunc01, (int,int,int),(arg1,arg2,arg3),int arg1,int arg2,int arg3) {
	
	PushXenonMessage(L"MSG","T1");
	return 0;
}

HOOKV3(0x828CBDC0, int, TestFunc02, (int,int,int),(arg1,arg2,arg3),int arg1,int arg2,int arg3) {

	PushXenonMessage(L"MSG","T2");
	return 0;
}

HOOKV3(0x828CBFD8, int, TestFunc03, (void*,unsigned int,short*),(_this,index,buffer),void* _this,unsigned int index,short* buffer) {


	buffer[0] = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
	buffer[1] = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
	buffer[2] = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	buffer[3] = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

	return 1;
}


HOOKV3(0x825BAAA0, Sonicteam::SoX::Component*,ComponentNewConstructor,(Sonicteam::SoX::Component*,Sonicteam::SoX::Component*),(_this,component),Sonicteam::SoX::Component* _this, Sonicteam::SoX::Component* component){
	
	return new (_this) Sonicteam::SoX::Component(component);
}

HOOKV3(0x82581500,Sonicteam::SoX::Engine::Task*,TaskNewConstructor,(Sonicteam::SoX::Engine::Task*,Sonicteam::SoX::Engine::Task*),(_this,task), Sonicteam::SoX::Engine::Task* _this,Sonicteam::SoX::Engine::Task* task){
	return new (_this) Sonicteam::SoX::Engine::Task(task);
}

HOOKV3(0x82581470,Sonicteam::SoX::Engine::Task*,TaskNewConstructor2,(Sonicteam::SoX::Engine::Task*,Sonicteam::SoX::Engine::Doc*),(_this,doc), Sonicteam::SoX::Engine::Task* _this,Sonicteam::SoX::Engine::Doc* doc){
	return new (_this) Sonicteam::SoX::Engine::Task(doc);
}


HOOKV3(0x825D4B50,void*,DefaultDebugListenerFunc01,(void*,const char*),(_this,str), void* _this, const char* str){
	PushXenonMessage(L"MSG","test");
	DebugLogV2::log.push_back(std::string(str));
	return (void*)0;
}

void DefaultDebugListenerFunc01Test(void* _this, const char* str){
	PushXenonMessage(L"MSG","test");
	DebugLogV2::log.push_back(std::string(str));
}






void STH2006DLLMain()
{
	
	

 //	Sonicteam::SoX::ExFileSystem& instance = Sonicteam::System::Singleton<Sonicteam::SoX::ExFileSystem, Sonicteam::System::CreateStatic<Sonicteam::SoX::ExFileSystem>>::getInstance();
//	instance.FileSystemGetFullPath(std::string("test"),1);

	


	//	auto* system = SINGLETON(Sonicteam::SoX::ExFileSystem,Sonicteam::System::CreateStatic).getInstance(0x82000000);




	

	//INSTALL_HOOKV3(TestFunc01);
	//INSTALL_HOOKV3(TestFunc02);
//	INSTALL_HOOKV3EX(TestFunc03,1,false);
//	INSTALL_HOOKV3EX(ComponentNewConstructor,-1,true);
//	INSTALL_HOOKV3EX(TaskNewConstructor,-1,true);
//	INSTALL_HOOKV3EX(TaskNewConstructor2,-1,true);
//	INSTALL_HOOKV3EX(DefaultDebugListenerFunc01,-1,true);
	//WRITE_DWORD(0X82048BC0,DefaultDebugListenerFunc01Test);
//	INSTALL_HOOKV3EX(ComponentNewDestructor,1,false);

//	boost::function<void*(double)> func1 = myFunc1;
//	boost::function<void*(void)> func2 = myFunc2;

	//Sonicteam::SoX::StepableThread* Thread = new Sonicteam::SoX::StepableThread("Test",func1,func2,1);
	//Thread->Resume();
	//SetEvent(	Thread->StartEvent);






	SSINGLETON(Sonicteam::SoX::ResourceManager)::getInstance((void*)0x82D3B224); //ResourceManager
	SSINGLETON(Sonicteam::SoX::PerformanceFrequency)::getInstance((void*)0x82D3B209);
	SSINGLETON(Sonicteam::SoX::ExFileSystem)::getInstance((void*)0x82D37088); 



	BranchTo(0x825383D8,int); //INIT HEAP Early
	HookNew::SaveBuffer = new std::map<void*, std::vector<HookNew*>>();
	HookNew::IsEmulated();
	HookV2::IsNotEmulatedHardWare =  HookV2::CheckIsNotEmulatedHardWare();



	FileSystemNew::GlobalInstall();
	FileSystemNew::AddArc("Resources.arc",2,0);





	ZLua lua_file = ZLua("game:\\common\\DLL.lua");
	std::stringstream log;
	std::wstringstream wlog;


	lua_file.DoFile(true);
	DebugLogV2::ThreadLog = lua_file.GetGlobalBool("ThreadLog");
	for (int i = 0;i<sizeof(functions)/sizeof(FPair);i++){
		if (lua_file.GetGlobalBool(functions[i].Name) == true){
			functions[i].Function();
			log << functions[i].Name << "\n";
		}
	}
	



	
	if (HookNew::IsEmulated()){ //  because i cant show Messages To Xbox360 , why tho?
		wlog << "DLL [" << __DATE__ << " ; " << __TIME__ << "]" << " Patch List ";
		XOVERLAPPED overlap;
		overlap.hEvent = CreateEvent(0,false,false,0);
		PushXenonMessage(wlog.str().c_str(),log.str().c_str(),&overlap);
		WaitForSingleObject(overlap.hEvent,INFINITE);
		CloseHandle(overlap.hEvent);
	}
	
	


	//PushXenonMessage(L"MSG","Test");
}
