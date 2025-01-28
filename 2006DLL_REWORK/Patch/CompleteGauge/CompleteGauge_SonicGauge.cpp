#pragma once
#include "CompleteGauge_SonicGauge.h"



float clamp(float value, float minVal, float maxVal) {
	if (value < minVal) {
		return minVal;
	} else if (value > maxVal) {
		return maxVal;
	} else {
		return value;
	}
}
namespace CompleteGauge{
	

	


	int SonicGaugeGetGaugeLevel(SonicGaugeExtended* _this){
		return _this->c_current_gauge_level;

	}

	float SonicGaugeGetGaugeMaturity(SonicGaugeExtended* _this){
		return _this->c_current_gauge_maturity;
	}

	void SonicGaugeAddGaugeMaturity(SonicGaugeExtended* _this){
		_this->c_current_gauge_maturity_add += 0.1f;
	}



	void __declspec( naked ) SonicGaugeConstructorH(SonicGaugeExtended* _this){
		__asm{
			mflr r12
				stw       r12, -0x8(r1)
				std		  r31, -0x10(r1)
				stwu      r1, -0x60(r1)
				lis r11,0x8223
				ori r11,r11,0xF218
				mtctr r11
				bctr r11
		}
	}	

	

	HOOK(void,__fastcall,SonicGaugeConstructor,0x8223F208,SonicGaugeExtended* _this){



		 SonicGaugeConstructorH(_this);
	
		 _this->c_current_gauge_maturity_add = 0;
		 _this->c_current_gauge_maturity = 0;
		 _this->c_current_gauge_level = 0;
		 _this->c_current_gauge_fatigue = 100;
		 _this->HomingSmash = false;
		 _this->HomingSmash_Release = false;
		
	}

	//_this->IsNotGrounded fatigue now
	void GaugeModuleStep(int _thisu, double delta){

		SonicGaugeExtended* _this = (SonicGaugeExtended*)(_thisu -0x24);

		
		_this->c_current_gauge_fatigue = clamp(_this->c_current_gauge_fatigue,1,100);
	
		_this->GaugeGroundTime +=delta;
		if (_this->GaugeGroundTime > _this->c_gauge_heal_delay){
			
			_this->c_current_gauge_fatigue+=1.0f * delta;// weak_fatigue
			
			float fdelta = _this->c_current_gauge_fatigue / 100.0f;
			_this->AddGaugeValue((_this->c_gauge_heal * fdelta ) * delta );
		}

	
	}


	void __fastcall SonicGaugeIVarible(Sonicteam::Player::IVariable* _this, Sonicteam::LuaSystem** lua){
		BranchTo(0x8223F360,int,_this,lua);
		CompleteGauge::SonicGaugeExtended* _this_fx = (CompleteGauge::SonicGaugeExtended*)dynamic_cast<Sonicteam::Player::SonicGauge*>(_this);

		_this_fx->c_gauge_fatigue_green =  (*lua)->GetFloatValue(std::string("c_fatigue_green"));
		_this_fx->c_gauge_fatigue_red =    (*lua)->GetFloatValue(std::string("c_fatigue_red"));
		_this_fx->c_gauge_fatigue_blue =   (*lua)->GetFloatValue(std::string("c_fatigue_blue"));
		_this_fx->c_gauge_fatigue_white =  (*lua)->GetFloatValue(std::string("c_fatigue_white"));
		_this_fx->c_gauge_fatigue_sky =    (*lua)->GetFloatValue(std::string("c_fatigue_sky"));
		_this_fx->c_gauge_fatigue_yellow = (*lua)->GetFloatValue(std::string("c_fatigue_yellow"));
		_this_fx->c_gauge_fatigue_purple = (*lua)->GetFloatValue(std::string("c_fatigue_purple"));
		_this_fx->c_gauge_fatigue_super =  (*lua)->GetFloatValue(std::string("c_fatigue_super"));




	}


	HOOK(int,__fastcall,ObjectEventsGaugeAddMaturityGaugeValue,0x82198C08,int a1, int a2, char Maturity){

		boost::shared_ptr<Sonicteam::Player::IGauge> *v4; // [sp+50h] [-20h] BYREF
		int v5; // [sp+54h] [-1Ch]


		*(BYTE *)(a2 + 4) = 1;
		*(DWORD *)(a1 + 0x214) |= 4u;


		v4 =  (boost::shared_ptr<Sonicteam::Player::IGauge> *)(a1 + 0x104);
			if ( v4  )
			{
				if (dynamic_cast<Sonicteam::Player::SonicGauge*>(v4->get())){
					v4->get()->AddMaturityValue();// 8200D4F0,8219F2E8
				}
				else if (dynamic_cast<Sonicteam::Player::IGauge*>(v4->get())){
					if ( Maturity )
						v4->get()->AddMaturityValue();// 8200D4F0,8219F2E8
					else
						v4->get()->AddGaugeValue(1.0);// 8223F328
				}
			}
				
			
	
			return 1;	


	}
	void GlobalInstall_SonicGauge()
	{


		//move to commonGuage
		INSTALL_HOOK(ObjectEventsGaugeAddMaturityGaugeValue);
	


		//Later, Move To CompleteGauge_CommonGauge
		//WRITE_DWORD(0x82198C70,0x409A0018);//Fix Recieve

		//Allocate more memory
		WRITE_DWORD(0x8219F038,POWERPC_ADDI(3,0,sizeof(SonicGaugeExtended)));
		INSTALL_HOOK(SonicGaugeConstructor);

		//WRITE_DWORD(0x8223EF64,0x419A0010); //Allow flag to heal


		WRITE_DWORD(0x8200D4E0,SonicGaugeGetGaugeLevel);
		WRITE_DWORD(0x8200D4E4,SonicGaugeGetGaugeMaturity);
		WRITE_DWORD(0x8200D4F0,SonicGaugeAddGaugeMaturity);

		WRITE_DWORD(0X8200D4C8,GaugeModuleStep);
		WRITE_DWORD(0x8200D4D0,SonicGaugeIVarible);


	}
	
}