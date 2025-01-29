#pragma once
#include "CompleteGauge_SonicContext.h"



#include "CompleteGauge_SonicGauge.h"




namespace CompleteGauge{







	//Core
	void SonicContextOnStep(SonicContextExtended *_this, double a2){


		bool LVL_UP = false;
		bool FULL_GAUGE = false;
		bool Homing_Smash_Charge = false;
		bool Homing_Smash_Release = false;
		bool SuperSoundFlag = false;
		bool LockGaugeHeal = false;
		bool IsSpeedUPGEM = false;


		IsSpeedUPGEM = (bool)_this->MachAura;

		//std::stringstream sus;
		//sus << std::hex<< _this->Input;

		//DebugLogV2::PrintNextFixed(sus.str());

		//decide 
		Sonicteam::Player::Score* score =  _this->ScorePlugin.get();
		Sonicteam::Player::State::IMachine* Mashine =  *(Sonicteam::Player::State::IMachine**)(score->PtrObjectPlayer + 0xE4);
		int StateID = Mashine->GetCurrentMashineStateID();

		if (StateID == 0x4D || StateID == 0x4A || StateID == 0x4B || _this->isShrink || _this->isSlowTime){

			//LockGaugeHeal = true;
		}

		if ((_this->Input & LN_GAMEPAD_BUTTON_RT_PRESS) != 0){
			Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
			Sonicteam::GameImp* gameimp = *(Sonicteam::GameImp**)(impl->DocCurrentMode + 0x6C);
			Sonicteam::Prop::Scene* mgr =  gameimp->GameProp[0].get();

			std::string Viser;
		

	
			std::map<std::string, int>* _map_ = (std::map<std::string,int>*)&(mgr->ScenePlacament);

			for (std::map<std::string, int>::iterator it =_map_->begin(); it != _map_->end(); ++it) {

					Viser.append(it->first);
					Viser.append(":");
					std::stringstream str; str << std::hex << it->second;
					Viser.append(str.str());
					Viser.append("\n");
				
			}


			PushXenonMessage(L"Test",Viser.c_str());
			
		}




		if ( SonicGaugeExtended* gauge = (SonicGaugeExtended*)_this->GaugePlugin.get())
		{

			if (LockGaugeHeal){
				gauge->IsNotGrounded++;
			}


			//Ground
			if ((_this->GroundAirFlags & 1) != 0){

			}
			//Air
			else{

				gauge->GaugeGroundTime = 0.0f;
			}




			if (gauge->c_current_gauge_maturity_add != 0){

				gauge_storage[_this->CurrentGem] += gauge->GetMaturityToAdd();
				gauge->ResetMaturityAdd();

				if (gauge_storage[_this->CurrentGem] >= 1.0){

					gauge_levels_storage[_this->CurrentGem]++;
					gauge_storage[_this->CurrentGem] = 0;
					LVL_UP = true;
				}
			}
			//Gauge IsFull Controller
			if (gauge->GaugeValue >= gauge->c_gauge_max && !gauge->IsFull){
				gauge->IsFull = true;
			}
			else if (gauge->GaugeValue < gauge->c_gauge_max && gauge->IsFull){
				gauge->IsFull = false;
			}

			//ExportFlagRequest
			if (gauge->IsFull) FULL_GAUGE = true;

			if (gauge->HomingSmash) Homing_Smash_Charge = true;
			if (gauge->HomingSmash_Release) Homing_Smash_Release = true;

			//Always

			gauge->c_current_gauge_maturity = gauge_storage[_this->CurrentGem];
			gauge->c_current_gauge_level = gauge_levels_storage[_this->CurrentGem];

		


		}

		if (_this->IsSuper) {

			_this->HomingInvulnerable = 1; 
			_this->c_super_ring_dec_time += a2;
			if (_this->c_super_ring_dec_time > 1.0){
				_this->c_super_ring_dec_time = 0.0f;
				if (_this->ScorePlugin.get()->RingsCount > 0) _this->ScorePlugin.get()->RingsCount--;


			}


			if (_this->ScorePlugin.get()->RingsCount <= 0 || _this->IsSonicDead){
				_this->HomingInvulnerable = 0;
				_this->IsSuper = false;
				//Switch backB
				Switch(_this,"player/sonic_new.lua","player/sonic_new","player_sonic","sonic");

			}
		}




		BranchTo(0x82219530,int,_this,a2);








		//Complete Output Flag
		_this->UnknownFlags01 |= (LVL_UP ? 0x10000000 : 0) | 
                         (FULL_GAUGE ? 0x20000000 : 0) |
                         (Homing_Smash_Charge ? 0x8000000 : 0) |
                         (Homing_Smash_Release ? 0x4000000 : 0) ;
                        

		_this->ExportWeaponRequestFlag |= (IsSpeedUPGEM ? 0x2000000 : 0);


		if ( CompleteGauge::SonicGaugeExtended* gauge = (SonicGaugeExtended*)_this->GaugePlugin.get()){

			gauge->HomingSmash_Release = false;
		}

	}


	void __declspec( naked ) SonicContextConstructorH(SonicContextExtended* _this){
		__asm{
			mflr r12
				stw       r12, -0x8(r1)
				std		  r31, -0x10(r1)
				stwu      r1, -0x60(r1)
				lis r11,0x8221
				ori r11,r11,0x9330
				mtctr r11
				bctr r11
		}
	}	

	HOOK(void,__fastcall,SonicContextConstructor,0x82219320,SonicContextExtended* _this){
		SonicContextConstructorH(_this);
		_this->IsSuper = false;
	}



	void __fastcall ScaleVisibleSwitchAuraController(int a1, int a2, double a3){

	}
	INT __fastcall ModelController_Sonic_Effects(INT result, unsigned int a2){

		return 0;
	}

	HOOK(bool,__fastcall,ContextIsCanDrainGauge,0x82217FC0,SonicContextExtended* _this,int index,float delta) {

		int index_table[9] = {0,3,2,1,7,5,4,6,8};
		index = index_table[index];

		if ( SonicGaugeExtended* gauge = (SonicGaugeExtended*)_this->GaugePlugin.get())
		{
			float drain_value = 10.0f;
			switch ( index )
			{
			case 1:
				drain_value = gauge->c_green;
				break;
			case 2:
				drain_value = gauge->c_red * delta;
			
				break;
			case 3:
				drain_value = gauge->c_blue;
				break;
			case 4:
				drain_value = gauge->c_white;
				break;
			case 5:
				drain_value = gauge->c_sky;
				break;
			case 6:
				drain_value = gauge->c_yellow;
				break;
			case 7:
				drain_value = gauge->c_purple * delta;
				break;
			case 8:
				drain_value = gauge->c_super;
				break;

				

			}

			if (gauge->GaugeValue >= drain_value) return true;
			else if (index == 2 || index == 7){

				if (   (_this->Input & LN_GAMEPAD_BUTTON_RT_HOLD) != 0)
					
					gauge->GaugeValue = 0.0f;
				return false;
			}

			return false;

		}
		return false;
	}


	HOOK(void,__fastcall,ContextIsDrainGauge,0x82218068,SonicContextExtended* _this,int index,double delta) {

		int index_table[9] = {0,3,2,1,7,5,4,6,8};
		index = index_table[index];

		if ( SonicGaugeExtended* gauge = (SonicGaugeExtended*)_this->GaugePlugin.get())
		{
			float drain_value = 10.0f;
			float fatigue_flag = 0;
			switch ( index )
			{
			case 1:
				drain_value = gauge->c_green;
				fatigue_flag = gauge->c_gauge_fatigue_green;
				break;
			case 2:
				drain_value = gauge->c_red * delta;
				fatigue_flag = gauge->c_gauge_fatigue_red * delta;
				break;
			case 3:
				drain_value = gauge->c_blue;
				fatigue_flag = gauge->c_gauge_fatigue_blue;
				break;
			case 4:
				drain_value = gauge->c_white;
				fatigue_flag = gauge->c_gauge_fatigue_white;
				break;
			case 5:
				drain_value = gauge->c_sky;
				fatigue_flag= gauge->c_gauge_fatigue_sky;
				break;
			case 6:
				drain_value = gauge->c_yellow;
				fatigue_flag= gauge->c_gauge_fatigue_yellow;
				break;
			case 7:
				drain_value = gauge->c_purple * delta;
				fatigue_flag = gauge->c_gauge_fatigue_purple;
				break;
			case 8:
				drain_value = gauge->c_super;
				fatigue_flag = gauge->c_gauge_fatigue_super;
				break;

	
			
		


			}
			gauge->GaugeValue -= drain_value;
			gauge->GaugeGroundTime = 0.0f;
			gauge->c_current_gauge_fatigue -= fatigue_flag;
			if (gauge->GaugeValue <= 0) gauge->GaugeValue = 0;
		}

	}


	void GlobalInstall_SonicContext(){



	
		WRITE_DWORD(0x821B5BCC,POWERPC_ADDI(3,0,sizeof(SonicContextExtended)));
		INSTALL_HOOK(SonicContextConstructor);
		WRITE_DWORD(0x8200B564,SonicContextOnStep);
		//	WRITE_DWORD(0x8200CFF0,ScaleVisibleSwitchAuraController);
		//	WRITE_DWORD(0x8200CFB8,ModelController_Sonic_Effects);

		WRITE_DWORD(0x822196CC,0x48000020); //Disable Original Context Handler

		INSTALL_HOOK(ContextIsCanDrainGauge);
		INSTALL_HOOK(ContextIsDrainGauge);





	}

}