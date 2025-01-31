#pragma once
#include "CompleteGauge_SonicStates.h"
#include "CompleteGauge_SonicGauge.h"
#include <Player/State/CommonObject.h>
#include <Player/State/SonicContext.h>

#include <LuaSystem.h>
#include <Player/INotification.h>

#include <Sox/Scenery/Clump.h>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <boost/cast.hpp>
#include <Player/Load.h>


namespace CompleteGauge{
	

	class SonicSuperStateExtended:public Sonicteam::Player::State::BasedObject<SonicContextExtended>{
	public:
		float time;
		float time_max;
		bool Trigger;
	};


	float ObjPlayerGetCurrentAnimTime(Sonicteam::Player::State::CommonContext* _context){

		Sonicteam::Player::Score* score =  _context->ScorePlugin.get();
		int ObjPlayer = (int)score->PtrObjectPlayer;
		int ModelPlayer = *(int*)(ObjPlayer + 0xD4);
		int ObjectPackageModel = *(int*)(ModelPlayer + 0x30);
		int IntrpoalateAnimationPackageModel = *(int*)(ObjectPackageModel + 0x98); //InterpolateAnimation Mostly
		int AnimationPackageModel;
		if (*(int*)IntrpoalateAnimationPackageModel == 0x8200CA6C){
			AnimationPackageModel = *(int*)(IntrpoalateAnimationPackageModel +0xC);
		}
		else{
			AnimationPackageModel = IntrpoalateAnimationPackageModel;
		}

		int AnimationHierarchyCommon = *(int*)(AnimationPackageModel + 0x4);
		return  *(float*)(AnimationHierarchyCommon +0x18);

	}
	void ObjPlayerSetCurrentAnimTime(Sonicteam::Player::State::CommonContext* _context,float value){

		Sonicteam::Player::Score* score =  _context->ScorePlugin.get();
		int ObjPlayer = (int)score->PtrObjectPlayer;
		int ModelPlayer = *(int*)(ObjPlayer + 0xD4);
		int ObjectPackageModel = *(int*)(ModelPlayer + 0x30);
		int IntrpoalateAnimationPackageModel = *(int*)(ObjectPackageModel + 0x98); //InterpolateAnimation Mostly
		int AnimationPackageModel;
		if (*(int*)IntrpoalateAnimationPackageModel == 0x8200CA6C){
			AnimationPackageModel = *(int*)(IntrpoalateAnimationPackageModel +0xC);
		}
		else{
			AnimationPackageModel = IntrpoalateAnimationPackageModel;
		}

		int AnimationHierarchyCommon = *(int*)(AnimationPackageModel + 0x4);
		*(float*)(AnimationHierarchyCommon +0x18) = value;

	}

	int ObjPlayerGetCurrentAnim(Sonicteam::Player::State::CommonContext* _context){

		Sonicteam::Player::Score* score =  _context->ScorePlugin.get();
		int ObjPlayer = (int)score->PtrObjectPlayer;
		int ModelPlayer = *(int*)(ObjPlayer + 0xD4);
		int ObjectPackageModel = *(int*)(ModelPlayer + 0x30);
	

		return  *(int*)(ObjectPackageModel +0xA0);

	}

	void ObjPlayerSetCurrentAnimation(Sonicteam::Player::State::CommonContext* _context,int value){

		Sonicteam::Player::Score* score =  _context->ScorePlugin.get();
		int ObjPlayer = (int)score->PtrObjectPlayer;
		int ModelPlayer = *(int*)(ObjPlayer + 0xD4);
		int ObjectPackageModel = *(int*)(ModelPlayer + 0x30);
		BranchTo(0x8222F490,int,ObjectPackageModel,value);



	}


	template <typename T>
	void ClearObjectByPluignName(std::vector<boost::shared_ptr<T>>* IStepableP1 ,const char* model){
		for (std::vector<boost::shared_ptr<T>>::iterator it = IStepableP1->begin(); it != IStepableP1->end(); it++) {
			boost::shared_ptr<T> pluginPtr = *it;
			if (Sonicteam::Player::IPlugIn* plugin = dynamic_cast<Sonicteam::Player::IPlugIn*>(pluginPtr.get())) {
				if  (plugin->PluginName == model){
				//		ShowXenonMessage(L"MSG","YES");
					IStepableP1->erase(it);
					break;;
				}
			}
		}

	}
	void Switch(Sonicteam::Player::State::CommonContext* _context,const char* lua_name,const char* package_name,const char* sound_name,const char* char_name){

		Sonicteam::Player::Score* score =  _context->ScorePlugin.get();
		int ObjPlayer = (int)score->PtrObjectPlayer;

		if (*(std::string*)(ObjPlayer + 0x1D8) == char_name ){
			return;
		}



		int ModelPlayer = *(int*)(ObjPlayer + 0xD4);
		
		Sonicteam::LuaSystem* p;
		if (!lua_name)
		const char* lua_name = "player/sonic_super.lua";
		if (!package_name)
		const char* package_name = "player/sonic_super";
		if (!sound_name)
		const char* sound_name = "player_sonic";


	    BranchTo(0x821EA260,int,&p,&std::string(lua_name),0x82003380,0x1D);
	


		Sonicteam::Player::Load* package_player =  (Sonicteam::Player::Load*)*(_DWORD *)(ObjPlayer + 0x154);

		

		float Save0x18 = ObjPlayerGetCurrentAnimTime(_context);
		int SavedAnim = ObjPlayerGetCurrentAnim(_context);
	
	

		//Reload (IVarible)) (input,model, gravity, state_context, zock, path, path_gd, path_col, score, physicsbody, automatic_dead, lockon, homing, lockon_homing, path_ld, lockon_lightdash, gauge, sonic_weapons,input vehicle, waterslider, item, ai)
	

		/*
		for (std::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>::iterator  it = IPluginP->begin();it!=IPluginP->end();it++ ){
			
			if ((*it)->PluginName == "model"){
				int ModelCHR = (int)(*it).get();
				Sonicteam::SoX::Scenery::Clump** clump_1 = (Sonicteam::SoX::Scenery::Clump**)(ModelCHR + 0x74);
				Sonicteam::SoX::Scenery::Clump** clump_2 = (Sonicteam::SoX::Scenery::Clump**)(ModelCHR + 0x78);
				Sonicteam::SoX::Scenery::Clump** clump_3 = (Sonicteam::SoX::Scenery::Clump**)(ModelCHR + 0x7C);
				Sonicteam::SoX::Scenery::Clump** clump_4 = (Sonicteam::SoX::Scenery::Clump**)(ModelCHR + 0x80);


				Sonicteam::SoX::Scenery::Clump** clump_5 = (Sonicteam::SoX::Scenery::Clump**)(ModelCHR + 0x130);
				Sonicteam::SoX::Scenery::Clump** clump_6 = (Sonicteam::SoX::Scenery::Clump**)(ModelCHR + 0x134);
				Sonicteam::SoX::Scenery::Clump** clump_7 = (Sonicteam::SoX::Scenery::Clump**)(ModelCHR + 0x138);
				Sonicteam::SoX::Scenery::Clump** clump_8 = (Sonicteam::SoX::Scenery::Clump**)(ModelCHR + 0x13C);

				int* ModelFlag = (int*)(ModelCHR+0x148);

				if (*clump_1) (*clump_1)->SSSD08();
				if (*clump_2) (*clump_2)->SSSD08();
				if (*clump_3) (*clump_3)->SSSD08();
				if (*clump_4) (*clump_4)->SSSD08();
				if (*clump_5) (*clump_5)->SSSD08();
				if (*clump_6) (*clump_6)->SSSD08();
				if (*clump_7) (*clump_7)->SSSD08();
				if (*clump_8) (*clump_8)->SSSD08();
	

	

				(*clump_1)->DestroyObject(1);
				(*clump_2)->DestroyObject(1);
				(*clump_3)->DestroyObject(1);
				(*clump_4)->DestroyObject(1);
				(*clump_5)->DestroyObject(1);
				(*clump_6)->DestroyObject(1);
				(*clump_7)->DestroyObject(1);
				(*clump_8)->DestroyObject(1);
		

			
				*clump_1 = 0;
				*clump_2 = 0;
				*clump_3 = 0;
				*clump_4 = 0;
				*clump_5 = 0;
				*clump_6 = 0;
				*clump_7 = 0;
				*clump_8 = 0;

				*ModelFlag = 0;

				boost::shared_ptr<Sonicteam::SoX::RefCountObject>*  element = (boost::shared_ptr<Sonicteam::SoX::RefCountObject>*)(ModelCHR + 0x30);
				boost::shared_ptr<Sonicteam::SoX::RefCountObject>*  element_1 = (boost::shared_ptr<Sonicteam::SoX::RefCountObject>*)(ModelCHR + 0xD4);
				element->reset();
				element_1->reset();

				(*it)->DestroyObject(0);

			}
		}
		*/
		


		std::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>* IPluginP = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>*>(ObjPlayer + 0x114);


		std::vector<boost::shared_ptr<Sonicteam::Player::IVariable>>* IVariblesP = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IVariable>>*>(ObjPlayer + 0x21C);


		std::vector<boost::shared_ptr<Sonicteam::Player::IDynamicLink>>* IDynamicLinkP = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IDynamicLink>>*>(ObjPlayer + 0x22C);
		std::vector<boost::shared_ptr<Sonicteam::Player::IFlagCommunicator>>* IFlagCommunicatorP = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IFlagCommunicator>>*>(ObjPlayer + 0x23C);

		std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>* IStepableP1 = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>*>(ObjPlayer + 0x24C);  
		std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>* IStepableP2 = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>*>(ObjPlayer + 0x25C);
		std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>* IStepableP3 = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>*>(ObjPlayer + 0x26C);
		std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>* IStepableP4 = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>*>(ObjPlayer + 0x27C);
			
		std::vector<boost::shared_ptr<Sonicteam::Player::IExportExternalFlag>>* IExportExternalFlagP = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IExportExternalFlag>>*>(ObjPlayer + 0x28C);


		std::vector<boost::shared_ptr<Sonicteam::Player::IExportPostureRequestFlag>>* IExportPostureRequestFlagP = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IExportPostureRequestFlag>>*>(ObjPlayer + 0x2AC);
		std::vector<boost::shared_ptr<Sonicteam::Player::INotification>>* INotificationP = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::INotification>>*>(ObjPlayer + 0x2DC);
	



		ClearObjectByPluignName(IVariblesP,"model");
		ClearObjectByPluignName(IPluginP,"model");
		ClearObjectByPluignName(IStepableP1,"model");
		ClearObjectByPluignName(IDynamicLinkP,"model");
		ClearObjectByPluignName(IFlagCommunicatorP,"model");
		ClearObjectByPluignName(IStepableP1,"model");
		ClearObjectByPluignName(IStepableP2,"model");
		ClearObjectByPluignName(IStepableP3,"model");
		ClearObjectByPluignName(IExportExternalFlagP,"model");
		ClearObjectByPluignName(IExportPostureRequestFlagP,"model");
		ClearObjectByPluignName(INotificationP,"model");




		Sonicteam::SoX::Object* obj = (Sonicteam::SoX::Object*)ObjPlayer;
		p->LuaProceedArgFunction(obj,std::string("SetupModule"));



		for (std::vector<boost::shared_ptr<Sonicteam::Player::IVariable>>::iterator  it = IVariblesP->begin();it!=IVariblesP->end();it++ ){
			(*it)->OnVarible(&p);
		}

		for (std::vector<boost::shared_ptr<Sonicteam::Player::IDynamicLink>>::iterator  it = IDynamicLinkP->begin();it!=IDynamicLinkP->end();it++ ){

			for (std::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>::iterator  it_2 = IPluginP->begin();it_2!=IPluginP->end();it_2++ ){
				(*it)->OnLink(*it_2);
			}
		}


		


/*
		ClearObjectByPluignName(IPluginP,"effect");
		ClearObjectByPluignName(IStepableP1,"effect");
		ClearObjectByPluignName(IDynamicLinkP,"effect");
		ClearObjectByPluignName(IFlagCommunicatorP,"effect");
		ClearObjectByPluignName(IStepableP1,"effect");
		ClearObjectByPluignName(IStepableP2,"effect");
		ClearObjectByPluignName(IStepableP3,"effect");
		ClearObjectByPluignName(IExportExternalFlagP,"effect");
		ClearObjectByPluignName(IExportPostureRequestFlagP,"effect");
		ClearObjectByPluignName(INotificationP,"effect");

*/

	
		*(std::string*)(ObjPlayer + 0x1D8) = p->LuaNodeUnknown0x0F("c_player_name");
	
	
		int ModelPlug =   (int)*(_DWORD *)(ObjPlayer + 0xD4);
		Sonicteam::Player::Load* ModelLoad = *(Sonicteam::Player::Load**)(((int)ModelPlug) + 0x98);
		



	//	Sonicteam::Player::IVariable* ModelPlugIVariable =  (Sonicteam::Player::IVariable*)(*(_DWORD *)(ObjPlayer + 0xD4)   + 0x20);
	//	ModelPlugIVariable->OnVarible(&p);

	
	//	ModelLoad->ProcessClump02(	(Sonicteam::SoX::RefCountObject**)((PackageModel->get()) + 0x40));
	//	ModelLoad->ProcessClump02_01((Sonicteam::SoX::RefCountObject**)(ModelPlug + 0x74));

			

		//LOAD,statetable,package,mode,face,frameGP
	//	Sonicteam::SoX::Scenery::Clump* ModelClumpBody = (Sonicteam::SoX::Scenery::Clump* )*(int*)(((int)PackageModel) + 0x40);
	//	Sonicteam::SoX::Scenery::Clump* ModelClumpHead = (Sonicteam::SoX::Scenery::Clump* )*(int*)(((int)PackageModel) + 0x4C);


	//	ModelClumpBody->SSSD08();
	//	ModelClumpHead->SSSD08();
	
	//	boost::shared_ptr<int>* ptr = (boost::shared_ptr<int>*)ObjPlayer + 0xD4;
	//	ptr->reset();
	


	

		//Sonicteam::SoX::Object* player = (Sonicteam::SoX::Object*)ObjPlayer;
		//Sonicteam::LuaSystem* lua_file = (Sonicteam::LuaSystem*)(p);
		//lua_file->LuaProceedArgFunction(player,std::string("SetupModule"));

	
	

	

	//	BranchTo(0x821962E8,int,ObjPlayer,&p); //This instead 
	//	BranchTo(0x82195ED0,int,ObjPlayer); //I Dont know what this does but it fixing upgrades
	



	    ObjPlayerSetCurrentAnimation(_context,SavedAnim);
		ObjPlayerSetCurrentAnimTime(_context,Save0x18);
	




	
	//	BranchTo(0x821966E0,int,ObjPlayer);
	//	BranchTo(0x82196CF8,int,ObjPlayer);
	//	BranchTo(0x82196768,int,ObjPlayer);
		

	
	
		//Reload Sound

		/*
		std::vector<boost::shared_ptr<Sonicteam::Player::INotification>>* Plugins = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::INotification>>*>(ObjPlayer + 0x2DC);
		for (std::vector<boost::shared_ptr<Sonicteam::Player::INotification>>::iterator it = Plugins->begin(); it != Plugins->end(); it++) {
			boost::shared_ptr<Sonicteam::Player::INotification> pluginPtr = *it;

			if (Sonicteam::Player::IPlugIn* plugin = dynamic_cast<Sonicteam::Player::IPlugIn*>(pluginPtr.get())) {
			

				if (plugin->PluginName == "sound"){

					int a1 = (int)plugin;

					int *v6; // r27
					int v7 = 0x8200EB78;
					int v8; // r11
					int v9; // r3
					char *v10; // r28
					int v11; // r3
					int v12; // r3
					int result; // r3


					v6 = (int *)(a1 + 0x5C);

					int ModelPlayer = *(int*)(ObjPlayer + 0xD4);

					v9 = *(_DWORD *)(a1 + 0x2C);
					Sonicteam::SoX::RefCountObject* CommonSound;
					Sonicteam::SoX::RefCountObject* SpecificSound;
					BranchTo(0x82618068,int,&CommonSound,ObjPlayer + 0xD0,"common");
					BranchTo(0x82618068,int,&SpecificSound,ObjPlayer + 0xD0,"specific");

					(*(void (__fastcall **)(int, int))(*(_DWORD *)v9 + 4))(v9, (int)&CommonSound);
					(*(void (__fastcall **)(int, int))(*(_DWORD *)v9 + 4))(v9, (int)&SpecificSound);



					do
					{
						v8 = *(_DWORD *)v7;
						if ( !*(_DWORD *)v7 )
							v8 = (int)sound_name;
						v9 = *(_DWORD *)(a1 + 0x2C);
						v10 = *(char **)(v7 + 4);
						if ( v9  )
						{
							v11 = (*(int (__fastcall **)(int, int))(*(_DWORD *)v9 + 0x14))(v9, v8);
							v12 = (*(int (__fastcall **)(_DWORD, int, char *))(**(_DWORD **)(a1 + 0x2C) + 0x1C))(
								*(_DWORD *)(a1 + 0x2C),
								v11,
								v10);
						}
						else
						{
							v12 = 0;
						}
						v7 += 8;
						*v6++ = v12;
					}
					while (v7 != 0x8200ED20 );
					//Do More Later


					BranchTo(0x822668A8,int,a1 + 0x20,&ModelPlayer);



					break;
				}

			}
		}



*/
	

	

	









		p->Release();


	}







	
	
	void SonicSuperOnStart(SonicSuperStateExtended* _this){
		BranchTo(0x821A3318,int,_this);
		_this->time = 0;
		_this->time_max = 1.5f;
		_this->Trigger = false;

	}
	//Complete REDO
	void SonicSuperOnUpdate(SonicSuperStateExtended* _this,float delta){
		_this->time+=delta;
		if (_this->time > _this->time_max && _this->Trigger == false){
			_this->Trigger = true;


			Switch(_this->CObjContext,"player/sonic_super.lua","player/sonic_super","player_sonic","supersonic");
			
		}
		
		BranchTo(0x821A3328,int,_this,delta);



	}
	void SonicSuperOnStateEnd(SonicSuperStateExtended* _this){
		_this->BOContext->IsSuper = true;
		_this->BOContext->c_super_ring_dec_time = 1.0f;
	}

	
	
		
	
	int __fastcall State_Sonic_HomingSmash_Action_Mid(SonicHomingSmash *a1, double a2)
	{
	
		if (a1->Flag1 == 2){

			if ( CompleteGauge::SonicGaugeExtended* gauge = (SonicGaugeExtended*)a1->BOContext->GaugePlugin.get()){

				gauge->HomingSmash_Release = true;
			}


		}

		return BranchTo(0x822177B0,int,a1,a2);

			

	}

	void __fastcall State_Sonic_HomingSmash_Action_Start(SonicHomingSmash *a1, double a2)
	{

		BranchTo(0x82217210 ,int,a1,a2);
		Sonicteam::Player::State::SonicContext *v2  = a1->BOContext;
		v2->SetAnimation(0x47);

		if ( CompleteGauge::SonicGaugeExtended* gauge = (SonicGaugeExtended*)a1->BOContext->GaugePlugin.get()){

			gauge->HomingSmash = true;
		}





	}

	int __fastcall State_Sonic_HomingSmash_Action_End(SonicHomingSmash* result)
	{

		if ( CompleteGauge::SonicGaugeExtended* gauge = (SonicGaugeExtended*)result->BOContext->GaugePlugin.get()){

			gauge->HomingSmash = false;
		}

		result->BOContext->IsGravityDisabled = 0;
		result->BOContext->HomingLockOn = 0;
		result->BOContext->HomingInvulnerable = 0;
		result->BOContext->IsJumped_PostureVelocityYHandle = 0;
		result->BOContext->IsForceTypeA_DashpanelsJumpRunHoming = 0;
		result->BOContext->byte23F = 0;


		return 0;;
	}

	void GlobalInstall_SonicStates()
	{
	

		WRITE_DWORD(0x8200B3F8,State_Sonic_HomingSmash_Action_Start);
		WRITE_DWORD(0x8200B3FC,State_Sonic_HomingSmash_Action_Mid);
		WRITE_DWORD(0x8200B400 ,State_Sonic_HomingSmash_Action_End	);

		WRITE_DWORD(0x82003B5C,SonicSuperOnStart);
		WRITE_DWORD(0x82003B60,SonicSuperOnUpdate);
		WRITE_DWORD(0x82003B64,SonicSuperOnStateEnd);
		WRITE_DWORD(0x821A32B8,POWERPC_ADDI(3,0,sizeof(SonicSuperStateExtended)));
	}
	
}