#include "2006DLL.h"
#include <string>


#include <Patch/Common/XEXALLOCATORS.H>
#include <Patch/Common/XEXALLOCATORS_UNDEF.H>
#include <Patch/FileSystemNew/FileSystemNew.h>
#include <Patch/TailsGauge/TailsGauge.h>
#include <Patch/OmegaGauge/OmegaGauge.h>
#include <Patch/reticle/reticle.h>



#include <Sox/Perfomance.h>
#include <Sox/StepableThread.h>
#include <Sox/Engine/Task.h>
#include <Hook/HookNew.h>
#include <vector>


#include <Sox/ResourceManager.h>
#include <Sox/FileSystemXenon.h>
#include <System/Singleton.h>
#include <System/CreateStatic.h>
#include <Player/IPostureControl.h>
#include <Sox/FileLoaderARC.h>
#include <Sox/FileSystemARC.h>
#include <Sox/ApplicationXenon.h>

#include <Heap.h>
#include <SpanverseHeap.h>
#include <Player/IPostureControl.h>
#include <boost/function.hpp>

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



HOOKV3(0x825E7990,void*,HeapFix,(Sonicteam::Heap*),(_this),Sonicteam::Heap* _this){
	_this->Initialize(0x100000);
	return _this;
}

void __fastcall sub_82200538(Sonicteam::Player::IPostureControl* _this, double a2, int a3){
	

	BranchTo(0x82200538,int,_this,a2);

	XMVECTOR scalingOrigin = XMVectorSet(1,1,1,1);
	XMVECTOR rotationOrigin = XMVectorSet(0,0,0,1);

	// Define the scaling orientation quaternion (identity quaternion for no additional orientation)
	XMVECTOR scalingOrientationQuaternion = XMQuaternionIdentity();
	XMVECTOR scale = XMVectorSet(0.25,0.25,0.25,1);



	// Build the transformation matrix
	XMMATRIX transformationMatrix = XMMatrixTransformation(
		scalingOrigin,                     // Scaling origin
		scalingOrientationQuaternion,      // Scaling orientation quaternion
		scale,                             // Scaling factors
		rotationOrigin,                    // Rotation origin
		_this->RotationFixed,                // Rotation quaternion
		_this->PositionFixed                           // Translation vector (position)
		);


	_this->RootFrame.get()->FrameSetTransformMatrix2(transformationMatrix);


	
}



float lerp(float a, float b, float t)
{
	return a + t * (b - a);
}
#define clamp(val, minVal, maxVal) ((val < minVal) ? minVal : ((val > maxVal) ? maxVal : val))

HOOKV3(0x821F1E30,void*,PostureCommon,(Sonicteam::Player::IPostureControl*,double),(_this,delta), Sonicteam::Player::IPostureControl* _this, double delta){
	
	
	if (Sonicteam::Player::State::ICommonContextIF* context =  _this->IContextIF.get()){
		_this->ImpulseZX = context->GetTotalSpeedZ();
		_this->ImpulseY = context->GetTotalSpeedY();
	}
	if ((_this->IPostureControlFlag0x114.PostureRequestFlag & 0x8000) != 0){
		_this->IPostureControlImport(_this->PlayerGetTransformData(),1.0f);
	}
	else{
		Sonicteam::Player::Input::IListener* AmigoAI = _this->AmigoListener.get();
		Sonicteam::Player::Input::IListener* InputListener = _this->InputListener.get();
		if (AmigoAI && AmigoAI->IsListenerEnabled()){ 
			_this->IPostureControlImport(*AmigoAI->ListenerGetStickVector4(delta,0),AmigoAI->ListenerGetStickPower()); //weird
		}
		else if (InputListener && InputListener->IsListenerEnabled()){
				_this->IPostureControlImport(*InputListener->ListenerGetStickVector4(delta,0),InputListener->ListenerGetStickPower()); //weird
	
		}
	}

	
	if (Sonicteam::Player::Gravity* gravity = _this->Gravity.get()){

		_this->GravityDirection = *(XMVECTOR*)gravity->GetGravityDirection();
		_this->GravityDownForce = gravity->GetGravityDownForce();
		_this->NormalizedSurface = CreateRotationFromUpToDirection(*gravity->GetGravityDirection());
	}

	Sonicteam::Player::ObjectPlayer* plr = (Sonicteam::Player::ObjectPlayer*)_this->PostureTask;
	Sonicteam::Player::State::IContext* icontext =  plr->PlayerMachine.get()->GetMashineContext().get();
	Sonicteam::Player::State::CommonContext* cc  = reinterpret_cast<Sonicteam::Player::State::CommonContext*>(icontext);




	int a1 = (int)_this;
	float c_slope_rad = *(float*)(a1 + 0x320); // in radians 
    float slope_dot = (XMVector3Dot(_this->NormalizedSurface, XMVectorSet(0,1,0,1))).x;
	float current_angle = acosf(clamp(slope_dot, -1.0f, 1.0f));


	if ( (_this->ContextIFFlag & (0x40 | 0x8 | 0x100 | 0x20)  ) != 0){
		float lerp_acc = _this->GravityDownForce / 1024.0 * delta;

		XMVECTOR interpolate_direction = XMVectorSet(0,1,0,1);


		if ( _this->GravityDownForce > cc->c_jump_run){
			lerp_acc *= 4;
		}
		if (current_angle > (c_slope_rad)) {
			interpolate_direction = _this->ImpulseVectorUP; // do not change them
		}
		


		if ((_this->ContextIFFlag & 0x8) != 0  && (_this->ContextIFFlag & 0x100) == 0 ){
			lerp_acc = 0.2;
			interpolate_direction = XMVectorSet(0,1,0,1);
		}
		else if ((_this->ContextIFFlag & 0x20) != 0){ //Tails Fly
			lerp_acc = 0.1;
			interpolate_direction = XMVectorSet(0,1,0,1);
		}

	


		_this->ImpulseVectorUP = XMVectorLerp(_this->ImpulseVectorUP,interpolate_direction,lerp_acc);
	
	}

	else if ((_this->ContextIFFlag & 0x400) != 0){


		struct Spline
		{
			XMVECTOR VPosition; // 
			XMVECTOR Up;   // 
			XMVECTOR Tangent; //probably
		};

		if (*(int*)(a1 + 0x340)){
			int PlayerPath = *(int*)(a1 + 0x340) - 0x20; // - IPosturePlugIn
			int PathAnimationController= *(int*)(PlayerPath + 0x44); // - IPosturePlugIn
			if (PathAnimationController && *(int*)(PathAnimationController) == 0x8204D3E0){
				Spline* _spline = (Spline*)(PathAnimationController + 0x20);
				_this->NormalizedSurface = _spline->Tangent;
				_this->ImpulseVectorUP = _spline->Up;
			}
		}
		


	}



	return 0;
}



HOOKV3(0x82582648,REF_TYPE(Sonicteam::SoX::IResource),sub_82582380,(REF_TYPE(Sonicteam::SoX::IResource)&,Sonicteam::SoX::IResourceMgr*,std::string&,char*,int,int),
	   (ret,mgr,str,out,flag1,flag2),
	   REF_TYPE(Sonicteam::SoX::IResource)& ret,Sonicteam::SoX::IResourceMgr* mgr, std::string& str,char* out,int flag1,int flag2){
		   
		   if (*(int*)0x82D3B264 = 0){
				*(int*)0x82D3B264 = BranchTo(0x82581F00,int);
		   }

		   Sonicteam::SoX::ResourceManager* rmgr =  &SSINGLETON(Sonicteam::SoX::ResourceManager)::getInstance();

		   //create
		   if (mgr->MgrType == 0 && rmgr->ManagerResouceMgr.size() == 0){
			   Sonicteam::SoX::HoldMGR holdmgr;
			   holdmgr.Unk0 = 0;
			   holdmgr.Unk4 = 0;
			   holdmgr.Mgr = mgr;
			   holdmgr.Flag01 = 1;
			   holdmgr.Flag02 = 0;
			   rmgr->ManagerResouceMgr[rmgr->ManagerResouceMgr.size()] = holdmgr;

		   }
		   else if ( rmgr->ManagerResouceMgr.find(mgr->MgrType) == rmgr->ManagerResouceMgr.end()){
			   Sonicteam::SoX::HoldMGR holdmgr;
			   holdmgr.Unk0 = 0;
			   holdmgr.Unk4 = 0;
			   holdmgr.Mgr = mgr;
			   holdmgr.Flag01 = 1;
			   holdmgr.Flag02 = 0;
			   rmgr->ManagerResouceMgr[rmgr->ManagerResouceMgr.size()] = holdmgr;
		   }





	   return ret;
}


/*
HOOKV3(0x825B1870,void*,PostureCommon,(Sonicteam::Player::IPostureControl*,double),(_this,delta), Sonicteam::Player::IPostureControl* _this, double delta){


}
*/


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





	HookNew::IsEmulated();
	HookV2::IsNotEmulatedHardWare =  HookV2::CheckIsNotEmulatedHardWare();


	/*
	WRITE_DWORD(0x82537B04,0x60000000);
	WRITE_DWORD(0x82537B0C,0x60000000);
	WRITE_DWORD(0x82537B10,0x60000000);
	WRITE_DWORD(0x82537B18,0x60000000);
	WRITE_DWORD(0x82537B1C,0x60000000);
	WRITE_DWORD(0x82537B30,0x60000000);
	WRITE_DWORD(0x82537B34,0x60000000);
	WRITE_DWORD(0x82537B3C,0x60000000);
	WRITE_DWORD(0x82537B28,0x60000000);
	*/
	

	//_onexitend   _onexitbegin
	*(unsigned int*)0x82D5E890 = 0xFFFFFFFF;
	*(unsigned int*)0x82D5E894 = 0xFFFFFFFF;
	BranchTo(0x825383D8,int); //INIT HEAP Early  XapiInitProcess
	int rout = BranchTo(0x82538230 ,int,1); //XapiCallThreadNotifyRoutines
	int XapiPAL50Incompatible = BranchTo(0x82537910,int,rout); //XapiPAL50Incompatible
	BranchTo(0x826E2B38,int); // _mtinit
	BranchTo(0x825381B8,int); // _rtinit
	BranchTo(0x825380D8,int,1); // _cinit




	//Declare :} Force easy use in-game singleton from DLL (next just SSINGLETON_INSTANCE pretty easy :))


	


	//Sonicteam::SoX::FileLoaderARC* ARC = &Sonicteam::SoX::FileLoaderARC::getInstance();
	//boost::function<void*(void)> test;

	//ARC->LoadFile(std::string("asdg"),test);





	SSINGLETON(Sonicteam::SoX::FileSystemArc)::getInstance((void**)0X82D3B284,(void*)0x825828F8);
	SSINGLETON(Sonicteam::SoX::FileLoaderARC)::getInstance((void**)0x82D3C184,(void*)0x8262A3E8); //FileLoaderARC
	SSINGLETON(Sonicteam::SoX::ArcHandleMgr)::getInstance((void**)0x82D36710,(void*)0x82163D20); //
	SSINGLETON(Sonicteam::SoX::ResourceManager)::getInstance((void**)0x82D3B264,(void*)0x82581F00); //ResourceManager


	SSINGLETON(Sonicteam::SoX::PerformanceFrequency)::getInstance((void**)0x82D3B210,(void*)0x82581C88);

	SSINGLETON(Sonicteam::SoX::FileSystemXenon)::getInstance((void*)0x82D37088); 
	SSINGLETON(Sonicteam::SpanverseHeap)::getInstance((void*)0x82D3C620); 
	HookNew::SaveBuffer = new std::map<void*, std::vector<HookNew*>>();
	

	//new Sonicteam::SoX::ApplicationXenon();



	//WRITE_DWORD(0x82009050,sub_82200538);
	


	FileSystemNew::GlobalInstall();
	FileSystemNew::AddArc("Resources.arc",2,0);
	
	
	

	//INSTALL_HOOKV3EX(HeapFix,1,false);
	//WRITE_DWORD(0x82050978,0x825E7B30);
	//WRITE_DWORD(0x8205097C,0x825E7C10);

//	reticle::GlobalInstall();
//	INSTALL_HOOKV3EX(PostureCommon,-1,1,11);




	ZLua lua_file = ZLua("game:\\common\\DLL.lua");
	std::stringstream log;
	std::wstringstream wlog;


	lua_file.DoFile(true);
	//DebugLogV2::ThreadLog = lua_file.GetGlobalBool("ThreadLog");
	DebugLogV2::ThreadLog = false; //temp


	for (int i = 0;i<sizeof(functions)/sizeof(FPair);i++){
		if (lua_file.GetGlobalBool(functions[i].Name) == true){
			functions[i].Function();
			log << functions[i].Name << "\n";
		}
	}
	



	
	if (HookNew::IsEmulated()){ //  because i cant show Messages To Xbox360 , why tho?
		wlog << "<<Core.exe>> [" << __DATE__ << " ; " << __TIME__ << "]" << " Patch List ";
		XOVERLAPPED overlap;
		overlap.hEvent = CreateEvent(0,false,false,0);
		PushXenonMessage(wlog.str().c_str(),log.str().c_str(),&overlap);
		WaitForSingleObject(overlap.hEvent,INFINITE);
		CloseHandle(overlap.hEvent);
	}
	
	


	//PushXenonMessage(L"MSG","Test");
}
