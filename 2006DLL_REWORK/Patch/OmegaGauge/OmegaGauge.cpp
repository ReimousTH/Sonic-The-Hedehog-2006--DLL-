#include "OmegaGauge.h"

#include <Player/IDynamicLink.h>
#include <Player/IPlugIn.h>
#include <boost/shared_ptr.hpp>
#include <Player/State/CommonContext.h>
#include <Player/Gauge/SonicGauge.h>
#include <Player/State/BasedObject.h>

#include <Core/Debug.h>

#include <Hook/HookBase.h>
#include <Hook/HookNew.h>

namespace OmegaGauge{



	class OmegaContextEx : public Sonicteam::Player::State::CommonContext {
	public:

		unsigned int uint0x230;
		unsigned int uint0x234;
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
		REF_TYPE(Sonicteam::SoX::RefCountObject) LockOnSound; //0x26C
		unsigned int Init; //0x270

	};


	void ContextOmegaOnLink(Sonicteam::Player::IDynamicLink* _this,DynContainer(Sonicteam::Player::IPlugIn) plugin){

		OmegaContextEx* _this_2 = (OmegaContextEx*)dynamic_cast<Sonicteam::Player::State::CommonContext*>(_this);

		if (plugin->PluginName == "gauge" && _this_2){
			_this_2->c_omega_gauge_module =  boost::dynamic_pointer_cast<Sonicteam::Player::IGauge>(plugin);
		}
		BranchTo(0x82213440,int,_this,&plugin);
	}

	void* OmegaContextDestroy(OmegaContextEx* ex,int flag){

		ex->c_omega_gauge_module.reset(); //reset_pointer
		ex->LockOnSound = 0;
		return BranchTo(0x822133D8,void*,ex,flag);
	}


	void OmegaContextOnUpdate(OmegaContextEx* _this,float delta){

		Sonicteam::Player::ObjectPlayer* obj =  _this->ScorePlugin->PtrObjectPlayer;
		Sonicteam::Player::RootFrame* frame = obj->RootFrame.param;



		Sonicteam::DocMarathonImp* doc_obj = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		void* AudioModule =  (void*)doc_obj->DocAudioPlayerImp.get();


		if (_this->Init == 0) {
			_this->Init = 1;
			int SoundBankID = BranchTo(0x82585D58,int,AudioModule,"player_omega");
			_this->LockOnSoundID = BranchTo(0x82584C00,int,AudioModule,SoundBankID,"lockon");

		}
	
	



		Sonicteam::Player::State::IMachine*  machine =  obj->PlayerMachine.get();
		int StateID = machine->GetCurrentMashineStateID();

		if (_this->IsLockOn && _this->LockOnSound.param == 0 ){
			_this->LockOnSound = BranchTo(0x82585480 ,REF_TYPE(Sonicteam::SoX::RefCountObject),AudioModule,_this->LockOnSoundID,&frame->RFPosition0xF0,0);
		}
		else if (!_this->IsLockOn && _this->LockOnSound.param != 0){
			_this->LockOnSound = 0;
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
		memset((void*)&_this->LockOnSound,0,0x4);
		_this->Init = 0;
		_this->LockOnSoundID = -1;





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





		INSTALL_HOOKV3EX(OmegaContextConstructor,1,false);
		WRITE_DWORD(0x8200AAD4,ContextOmegaOnLink);
		WRITE_DWORD(0x821B5DD4,0x38600274);
		WRITE_DWORD(0x8200AADC,OmegaContextDestroy);
		WRITE_DWORD(0x8200AAE4,OmegaContextOnUpdate);

	}

}