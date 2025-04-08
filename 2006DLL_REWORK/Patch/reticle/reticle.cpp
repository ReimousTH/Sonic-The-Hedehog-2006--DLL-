#include "reticle.h"

#include <Player/IDynamicLink.h>
#include <Player/IPlugIn.h>
#include <boost/shared_ptr.hpp>
#include <Player/State/CommonContext.h>
#include <Player/Gauge/SonicGauge.h>
#include <Player/State/BasedObject.h>

#include <Player/OmegaLaser.h>

#include <Core/Debug.h>

#include <Hook/HookBase.h>
#include <Hook/HookNew.h>

#include <Sox/RefCountObject.h>
#include <CsdObject.h>


#include <iostream>
#include <sstream>
#include <string>

#include <Player/LockOn.h>

#include <Player/Homing.h>
namespace reticle{


	class HomingEx:public Sonicteam::Player::Homing{
	public:
		unsigned int LockOnSoundID; //0x64
		std::vector<REF_TYPE(Sonicteam::SoX::RefCountObject)> LockOnSound;

		REF_TYPE(Sonicteam::CsdObject) CSDObject; 
		REF_TYPE(Sonicteam::SoX::RefCountObject) CSDSHADER; 
		REF_TYPE(Sonicteam::SoX::RefCountObject) CSDTechniqueCSD3D; 
		REF_TYPE(Sonicteam::SoX::Scenery::Drawable) CSDObjectDrawable; 
		bool init;


	};



	HOOKV3(0x821F8DE0,void*,HomingExReplace,(HomingEx*,void*,void*,void*,void*),(_this,arg2,arg3,arg4,arg5),HomingEx* _this,void* arg2,void* arg3,void* arg4,void* arg5){

		memset(&_this->LockOnSoundID,0,4);
		memset(&_this->LockOnSound,0,0xC);

		memset(&_this->CSDObject,0,4);
		memset(&_this->CSDSHADER,0,4);
		memset(&_this->CSDTechniqueCSD3D,0,4);
		memset(&_this->CSDObjectDrawable,0,4);





		_this->CSDSHADER = BranchTo(0x82371620,REF_TYPE(Sonicteam::SoX::RefCountObject),&std::string("shader/primitive/csd3D.fx"));
		_this->CSDTechniqueCSD3D  = BranchTo(0x828B94B8,REF_TYPE(Sonicteam::SoX::RefCountObject),_this->CSDSHADER.get(),"TechniqueCSD3D");
		_this->CSDObject  = BranchTo(0x82617570,REF_TYPE(Sonicteam::CsdObject),&std::string("sprite/reticle_DLL"));

		if (_this->CSDObject.get()){

			_this->CSDObject.get()->FCsdObject0x20 = 0x401 ;
			_this->CSDObject.get()->MarathonPlaySceneLoopAnimation("reticle","sonic_reticle");

			if (_this->CSDObject.get()->FCProject){


				/*
				for (std::map<const char*,Chao::CSD::RCObject<Chao::CSD::CScene>*,STD_MAP_CONST_CHAR_PTR_COMPARATOR>::iterator it = _this->CSDObject.get()->FCProject->CProjectScene.begin();it != _this->CSDObject.get()->FCProject->CProjectScene.end();it++){

					if (it->second && it->second->get()){
						int v = (int)it->second->get();
						if (*(int*)v == 0x82037090){
							*(float*)(v + 0x84) = 75.0;
						}


					}
				}
				*/

			}
		}





		return _this;
	}

	void LockOnFlag(Sonicteam::Player::IFlagCommunicator* _step,unsigned int flag){
		HomingEx* _this = static_cast<HomingEx*>(_step);

		int pre = _this->HomingStatePre;
		int after = _this->HomingStateAfter;

		BranchTo(0x821F7CB8,int,_step,0x10000000);


		//rollback-values
		if ((flag & 0x10000000) == 0){
			_this->HomingStatePre = pre;
			_this->HomingStateAfter = after;
			BranchTo(0x821F7CB8,int,_step,flag);
		}


		Sonicteam::DocMarathonImp* doc_obj = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		void* AudioModule =  (void*)doc_obj->DocAudioPlayerImp.get();
		Sonicteam::Player::RootFrame* frame = _this->RootFrame.get();
		Sonicteam::SoX::Scenery::World* GraphicBufferGuess =  (doc_obj->DocGetWorld(7).get());


		if (Sonicteam::SoX::Physics::Entity* entity =  _this->LockOnEntityAfter.Entity){
			//BranchTo(0x82585480 ,REF_TYPE(Sonicteam::SoX::Scenery::Drawable),AudioModule,_this->LockOnSoundID,&frame->RFPosition0xF0,0); //no need to save them
			Sonicteam::SoX::Physics::Havok::BodyHavok* body = static_cast<Sonicteam::SoX::Physics::Havok::BodyHavok*>(entity);
			XMVECTOR pos = body->GetPosition();
			REF_TYPE(Sonicteam::CsdObject) CSD_LOCAL = _this->CSDObject;
			Sonicteam::SoX::Scenery::Drawable* CSDObjectDrawable = ((Sonicteam::SoX::Scenery::Drawable* (__fastcall *)(int,int,REF_TYPE(Sonicteam::CsdObject)))0x82616C68)(malloc06(0xA0),doc_obj->DocGetMyGraphicDevice(),CSD_LOCAL);
			((void* (__fastcall *)(Sonicteam::SoX::Scenery::Drawable*,REF_TYPE(Sonicteam::SoX::RefCountObject)))0x82616EB0)(CSDObjectDrawable,_this->CSDTechniqueCSD3D) ;
			_this->CSDObjectDrawable =   REF_TYPE(Sonicteam::SoX::Scenery::Drawable)(CSDObjectDrawable) ;
			GraphicBufferGuess->WorldAddDrawable(_this->CSDObjectDrawable);
			int CsdObjectDrawable_INT = (int)CSDObjectDrawable;
			*(char *)(CsdObjectDrawable_INT + 0x74) = 1;
			*(XMVECTOR *)(CsdObjectDrawable_INT + 0x80) = pos;
			*(float *)(CsdObjectDrawable_INT + 0x94) = 1000000.0;
		}
		else{
			_this->CSDObjectDrawable = 0;
		}
		



	


	}

	void LockOnStep(Sonicteam::Player::IStepable* _step,double delta){
		HomingEx* _this = static_cast<HomingEx*>(_step);
		if (_this->CSDObject.param){
			_this->CSDObject.get()->CsdLink0x8(delta);
		}

		if (!_this->init){
		//	Sonicteam::DocMarathonImp* doc_obj = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		//	void* AudioModule =  (void*)doc_obj->DocAudioPlayerImp.get();
		//	int SoundBankID = BranchTo(0x82585D58,int,AudioModule,"player_sonic");
		//	_this->LockOnSoundID = BranchTo(0x82584C00,int,AudioModule,SoundBankID,"guard_on");
			_this->init = true;
		}



		//still int workk tho :(
	//	EntityContainer container_2 = EntityContainer();
	//	EntityContainer container =  _this->LockOn->GetLockEntity1(&container_2);




	//	container.Reset();
	//	container_2.Reset();
	



	



		BranchTo(0x821F83D8,int,_step,delta);
	}



	
	void GlobalInstall()
	{
		WRITE_DWORD(0x8219EAAC,0x38600250);
	
		INSTALL_HOOKV3EX(HomingExReplace,1,false,11);
		WRITE_DWORD(0x82008BA8 ,LockOnStep);
		WRITE_DWORD(0x82008BB4 ,LockOnFlag);
	}

}