#pragma once
#include "CompleteGauge_SonicSound.h"
#include <Sox/RefCountObject.h>


namespace CompleteGauge{
	

	void __declspec( naked ) State_Sound_Sonic_ConstructorH(int a1,int a2,int a3,int a4){
		__asm{
			mflr r12

				std       r27, -0x30(r1)
				std       r28, -0x28(r1)
				std       r29, -0x20(r1)
				std       r30, -0x18(r1)
				std       r31, -0x10(r1)
				stw       r12, -0x8(r1)
				stwu      r1, -0x80(r1)
				mr r31,r3
				lis r11,0x8226
				ori r11,r11,0xA488
				mtctr r11
				bctr r11
		}
	}	

	HOOK(int,__fastcall,State_Sound_Sonic_Constructor,0x8226A478,int a1,int a2,int a3,int a4)
	{

		// a4 = char* , sound_element_bank
		State_Sound_Sonic_ConstructorH(a1,a2,a3,a4);

		*(int*)(a1 + 0x1D0) = BranchTo(0x82265810,int,a1,a4,(DWORD)"homing_charge");
		*(int*)(a1 + 0x1D4) = BranchTo(0x82265810,int,a1,a4,(DWORD)"homing_shoot");

		*(int*)(a1 + 0x1D8) = BranchTo(0x82265810,int,a1,"obj_common",(DWORD)"gauge_max");
		*(int*)(a1 + 0x1DC) = BranchTo(0x82265810,int,a1,a4,(DWORD)"levelup");

		*(int*)(a1 + 0x1E0) = BranchTo(0x82265810,int,a1,a4,(DWORD)"super");

		return a1;

	}

	void StateSoundOnUnknownFlags01(int _this,int flags){

		Sonicteam::SoX::RefCountObject* RefSound;
		//LVL-Up
			
		if ((flags & 0x10000000) != 0){
			BranchTo(0x82265898,int,&RefSound, _this - 0x20, *(DWORD *)(_this + 0x1DC-0x20));
			if (RefSound) RefSound->Release();
		}
		//Full Gauge
		if ((flags & 0x20000000) != 0){

	
			BranchTo(0x82265898,int,&RefSound, _this - 0x20, *(DWORD *)(_this + 0x1D8-0x20));
			if (RefSound) RefSound->Release();
		}

		//Homing Smash
		if ((flags & 0x8000000) != 0){


			BranchTo(0x82265898,int,&RefSound, _this - 0x20, *(DWORD *)(_this + 0x1D0-0x20));
			if (RefSound) RefSound->Release();
		}
		//Homing Smash Release
		if ((flags & 0x4000000) != 0){


			BranchTo(0x82265898,int,&RefSound, _this - 0x20, *(DWORD *)(_this + 0x1D4-0x20));
			if (RefSound) RefSound->Release();
		}


		BranchTo(0x82266150,int,_this,flags);

	}


	int __fastcall StateSoundOnAnimationFlag(int result, int a2){
		return 0;
	}


	void GlobalInstall_SonicSound()
	{

		

		WRITE_DWORD(0x8219DF94,POWERPC_ADDI(3,0,0x1E4)); //Allocate More Mem
		WRITE_DWORD(0x8200FD9C,StateSoundOnUnknownFlags01);
	//	WRITE_DWORD(0x8200FD90,StateSoundOnAnimationFlag);
		INSTALL_HOOK(State_Sound_Sonic_Constructor);
		
	}
	
}