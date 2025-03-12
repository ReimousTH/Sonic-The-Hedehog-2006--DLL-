#include "OmegaGauge.h"

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

namespace OmegaGauge{



	class OmegaContextEx : public Sonicteam::Player::State::CommonContext {
	public:

		boost::shared_ptr<void*> OmegaWeapons;
		unsigned int uint0x238;
		unsigned int uint0x23C;
		unsigned int uint0x240;

		char uint0x244;
		char uint0x245;
		char IsLockOn;
		char uint0x247;

		unsigned int uint0x248;
		unsigned int uint0x24C;
		unsigned int uint0x250;
		unsigned int uint0x254;
		unsigned int uint0x258;
		unsigned int uint0x25C;

	
		boost::shared_ptr<Sonicteam::Player::IGauge> c_omega_gauge_module; //0x260
		unsigned int LockOnSoundID; //0x268

		std::vector<REF_TYPE(Sonicteam::SoX::RefCountObject)> LockOnSound; //0x26C
		unsigned int Init; //0x27C


		REF_TYPE(Sonicteam::CsdObject) CSDObject; //0x280
		REF_TYPE(Sonicteam::SoX::RefCountObject) CSDSHADER; //0x284
		REF_TYPE(Sonicteam::SoX::RefCountObject) CSDTechniqueCSD3D; //0x288
		std::vector<REF_TYPE(Sonicteam::SoX::Scenery::Drawable)> CSDObjectDrawable; //0x28C
		int LockOnCount; //0x29C

	};


	void ContextOmegaOnLink(Sonicteam::Player::IDynamicLink* _this,DynContainer(Sonicteam::Player::IPlugIn) plugin){

		OmegaContextEx* _this_2 = (OmegaContextEx*)dynamic_cast<Sonicteam::Player::State::CommonContext*>(_this);

		if (plugin->PluginName == "gauge" && _this_2){
			_this_2->c_omega_gauge_module =  boost::dynamic_pointer_cast<Sonicteam::Player::IGauge>(plugin);
		}
		BranchTo(0x82213440,int,_this,&plugin);
	}

	void* OmegaContextDestroy(OmegaContextEx* _this,int flag){

		_this->c_omega_gauge_module.reset(); //reset_pointer
		_this->LockOnSound.clear();
	

		_this->CSDObject = 0;
		_this->CSDSHADER = 0;
		_this->CSDTechniqueCSD3D = 0;
	
		_this->CSDObjectDrawable.clear();

		return BranchTo(0x822133D8,void*,_this,flag);
	}


	void OmegaContextOnUpdate(OmegaContextEx* _this,float delta){


		if (_this->CSDObject.param){
			_this->CSDObject.get()->CsdLink0x8(delta);
		}


		Sonicteam::Player::ObjectPlayer* obj =  _this->ScorePlugin->PtrObjectPlayer;
		Sonicteam::Player::RootFrame* frame = obj->RootFrame.param;

		Sonicteam::Player::OmegaLaser* OmegaLaser;

		if (int Weapons =  (int)_this->OmegaWeapons.get()){
			int Weapons5C = *(int*)(Weapons + 0x5C) ; //OMegaLaser
			int Weapons5C88 = *(int*)(Weapons5C + 0x88);
			OmegaLaser = *(Sonicteam::Player::OmegaLaser**)(Weapons + 0x5C) ;
		}





		Sonicteam::DocMarathonImp* doc_obj = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		void* AudioModule =  (void*)doc_obj->DocAudioPlayerImp.get();


		if (_this->Init == 0) {

		
			Sonicteam::SoX::Scenery::World* GraphicBufferGuess =  (doc_obj->DocGetWorld(7).get());


			_this->Init = 1;
			int SoundBankID = BranchTo(0x82585D58,int,AudioModule,"player_omega");
			_this->LockOnSoundID = BranchTo(0x82584C00,int,AudioModule,SoundBankID,"lockon");

			_this->CSDSHADER = BranchTo(0x82371620,REF_TYPE(Sonicteam::SoX::RefCountObject),&std::string("shader/primitive/csd3domegalockon.fx"));
			_this->CSDTechniqueCSD3D  = BranchTo(0x828B94B8,REF_TYPE(Sonicteam::SoX::RefCountObject),_this->CSDSHADER.get(),"TechniqueCSD3D");
			_this->CSDObject  = BranchTo(0x82617570,REF_TYPE(Sonicteam::CsdObject),&std::string("sprite/reticle_DLL"));

			if (_this->CSDObject.get()){

				_this->CSDObject.get()->FCsdObject0x20 = 0x401 ;
				_this->CSDObject.get()->MarathonPlaySceneLoopAnimation("reticle","omega_reticle");

				if (_this->CSDObject.get()->FCProject){


					for (std::map<const char*,Chao::CSD::RCObject<Chao::CSD::CScene>*,STD_MAP_CONST_CHAR_PTR_COMPARATOR>::iterator it = _this->CSDObject.get()->FCProject->CProjectScene.begin();it != _this->CSDObject.get()->FCProject->CProjectScene.end();it++){

						if (it->second && it->second->get()){
							int v = (int)it->second->get();
							if (*(int*)v == 0x82037090){
								*(float*)(v + 0x84) = 75.0;
							}


						}
					}

				}
	

			
			}
	


		}
	
	



		Sonicteam::Player::State::IMachine*  machine =  obj->PlayerMachine.get();
		int StateID = machine->GetCurrentMashineStateID();





		if (_this->IsLockOn && OmegaLaser){

			//Update Trigger
			if (_this->LockOnCount != OmegaLaser->Entities.size()){

				int Difference =  OmegaLaser->Entities.size() - _this->LockOnCount;
				if (Difference > 0){
					for (int i = 0;i<Difference;i++){
						BranchTo(0x82585480 ,REF_TYPE(Sonicteam::SoX::RefCountObject),AudioModule,_this->LockOnSoundID,&frame->RFPosition0xF0,0); //no need to save them
					}
				}
				else if (Difference < 0){

				}

				_this->LockOnCount = OmegaLaser->Entities.size();
			}
			

			
		


			_this->CSDObjectDrawable.clear();

	
			if (_this->LockOnCount > 0 && _this->CSDObject.get() ){
				Sonicteam::SoX::Scenery::World* GraphicBufferGuess =  (doc_obj->DocGetWorld(7).get());

			
				int cc = 0;
				for (std::deque<EntityContainer>::iterator it = OmegaLaser->Entities.begin(); 
					it != OmegaLaser->Entities.end(); ++it) {

						EntityContainer* havokbody = &(*it);
						Sonicteam::SoX::Physics::Havok::BodyHavok* body = static_cast<Sonicteam::SoX::Physics::Havok::BodyHavok*>(havokbody->Entity);
						if (havokbody->Entity) {
						XMVECTOR pos = body->GetPosition();
				
						REF_TYPE(Sonicteam::CsdObject) CSD_LOCAL = _this->CSDObject;
						Sonicteam::SoX::Scenery::Drawable* CSDObjectDrawable = ((Sonicteam::SoX::Scenery::Drawable* (__fastcall *)(int,int,REF_TYPE(Sonicteam::CsdObject)))0x82616C68)(malloc06(0xA0),doc_obj->DocGetMyGraphicDevice(),CSD_LOCAL);
						((void* (__fastcall *)(Sonicteam::SoX::Scenery::Drawable*,REF_TYPE(Sonicteam::SoX::RefCountObject)))0x82616EB0)(CSDObjectDrawable,_this->CSDTechniqueCSD3D) ;
					   
						

						REF_TYPE(Sonicteam::SoX::Scenery::Drawable) CDW = REF_TYPE(Sonicteam::SoX::Scenery::Drawable)(CSDObjectDrawable) ;
						GraphicBufferGuess->WorldAddDrawable(CDW);
						

				
						int CsdObjectDrawable_INT = (int)CSDObjectDrawable;
						*(char *)(CsdObjectDrawable_INT + 0x74) = 1;
						*(XMVECTOR *)(CsdObjectDrawable_INT + 0x80) = pos;
						*(float *)(CsdObjectDrawable_INT + 0x94) = 1000000.0;
						_this->CSDObjectDrawable.push_back(CDW);

					
						cc++;
						std::stringstream ss;
						ss <<  pos.x  <<  " : " <<  pos.y << " : " << pos.z << std::endl;
						ss <<  havokbody  << std::endl;
		

			
					
					//	PushXenonMessage(L"MSG",ss.str().c_str());
						}
				
				}
			}

		}
		else{
			_this->LockOnCount = 0;
			_this->CSDObjectDrawable.clear();
		}





		if ( Sonicteam::Player::SonicGauge* gauge =  dynamic_cast<Sonicteam::Player::SonicGauge*>(_this->c_omega_gauge_module.get())){


			//Hover
			if (StateID == 0x4E){
				gauge->GaugeValue -= gauge->c_red * delta; //drain per second 

				if (gauge->GaugeValue <= 0){
					gauge->GaugeValue = 0.0;
					machine->ChangeMashineState(3);
				
				}
			}
	


			if ( (_this->GroundAirFlags & CC_GROUND) != 0){
				gauge->IsNotGrounded = 0;
			}
			else{
				gauge->IsNotGrounded = 1;
				gauge->GaugeGroundTime = 0.0f;
			}

			//IsGrounded
			if (!gauge->IsNotGrounded)
				gauge->GaugeGroundTime += delta;
			if (gauge->GaugeGroundTime >=gauge->c_gauge_heal_delay)
				gauge->AddGaugeValue(gauge->c_gauge_heal * delta);
		}

			BranchTo(0x82213018,int,_this,delta);


		
	}

	HOOKV3(0x822132E8,void*,OmegaContextConstructor,(OmegaContextEx*),(_this),OmegaContextEx* _this){
		memset((void*)&_this->c_omega_gauge_module,0,0x8);
		_this->Init = 0;
		_this->LockOnSoundID = -1;
		_this->LockOnCount = 0;


		memset((void*)&_this->CSDObject,0,0x4);
		memset((void*)&_this->CSDSHADER,0,0x4);
		memset((void*)&_this->CSDTechniqueCSD3D,0,0x4);
		memset((void*)&_this->CSDObjectDrawable,0,0xC);
		memset((void*)&_this->LockOnSound,0,0xC);


		return _this;
	}



	int State_Omega_Fall_Action_Mid(Sonicteam::Player::State::BasedObject<OmegaContextEx>* _this, double delta){

		OmegaContextEx* context = (OmegaContextEx*)_this->CObjContext;
		int input =  context->LockInputTime > 0 ? 0 : context->Input;

		bool gauge_pass = true;
		if ( Sonicteam::Player::SonicGauge* gauge =  dynamic_cast<Sonicteam::Player::SonicGauge*>(context->c_omega_gauge_module.get())){
			if (gauge->GaugeValue <= 0) gauge_pass = false;
		}

		if ((input >> 1  & 1) != 0 && gauge_pass){
			_this->ObjectMashine->ChangeMashineState(0x4E);
			return 1;
		}



		return BranchTo(0x821A9BC0,int,_this,delta);
	}

	BOOL __fastcall State_Omega_Jump_Action_Mid(Sonicteam::Player::State::BasedObject<OmegaContextEx>* _this, double delta){



		char  v4 = *(unsigned __int8 *)((int)(_this) + 0x2C);
		OmegaContextEx* context = (OmegaContextEx*)_this->CObjContext;
		int input =  context->LockInputTime > 0 ? 0 : context->Input;

		bool gauge_pass = true;
		if ( Sonicteam::Player::SonicGauge* gauge =  dynamic_cast<Sonicteam::Player::SonicGauge*>(context->c_omega_gauge_module.get())){
			if (gauge->GaugeValue <= 0) gauge_pass = false;
		}

		if ((input >> 1  & 1) != 0 && gauge_pass && v4){
			_this->ObjectMashine->ChangeMashineState(0x4E);
			return 1;
		}
		return BranchTo(0x821A9E38,int,_this,delta);

	}




	
	
	void GlobalInstall()
	{
		//State
		WRITE_DWORD(0x820047B0,State_Omega_Fall_Action_Mid);
		WRITE_DWORD(0x820047E4,State_Omega_Jump_Action_Mid);
		WRITE_DWORD(0x821A9BEC,0x48000030); //becaue i dont want to rewrite eve
		WRITE_DWORD(0x821A9E60,0x48000040); //becaue i dont want to rewrite eve


		//Sound





		INSTALL_HOOKV3EX(OmegaContextConstructor,1,false,12);
		WRITE_DWORD(0x8200AAD4,ContextOmegaOnLink);
		WRITE_DWORD(0x821B5DD4,0x386002A0);
		WRITE_DWORD(0x8200AADC,OmegaContextDestroy);
		WRITE_DWORD(0x8200AAE4,OmegaContextOnUpdate);

	}

}