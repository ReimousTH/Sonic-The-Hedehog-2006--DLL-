#pragma once
#include "CompleteGauge_SonicEffect.h"


namespace CompleteGauge{
	


	void __declspec( naked ) EffectModuleSonicHookH(int a1,int a2){
		__asm{
			mflr r12
				std       r25, -0x40(r1)
				std       r26, -0x38(r1)
				std       r27, -0x30(r1)
				std       r28, -0x28(r1)
				std       r29, -0x20(r1)
				std       r30, -0x18(r1)
				std       r31, -0x10(r1)
				stw       r12, -0x8(r1)
				stwu      r1, -0xD0(r1)
				mr r31,r3
				lis r11,0x8222
				ori r11,r11,0x82D0
				mtctr r11
				bctr r11
		}
	}	

	HOOK(int,__fastcall,EffectModuleSonicHook,0x822282C0,int a1,int a2)
	{
		EffectModuleSonicHookH(a1,a2);




		memset((void*)(a1 +0x680),0,0x1C * 3);
		BranchTo(0x82225A18,int,a1 + 0x680,a1 +0x2C,&std::string("superchange"));
		BranchTo(0x82225A18,int,a1 + 0x69C,a1 +0x2C,&std::string("homing_smash"));
		BranchTo(0x82225A18,int,a1 + 0x6B8,a1 +0x2C,&std::string("homing_00"));





		return a1;

	}

	void Sonic_Effect_OnAnimationChange(int _this,int flag,int flag2){

	
		DWORD _fake_boost_container[2] = {0};

		//Super-Sonic Transformation
		if (flag == 0xCE){

			BranchTo(0x82225BD8,int,
				&_fake_boost_container, //r3
				_this + 0xC, // (GE1,GE2,GE3 and some like this, buffers) r4
				*(UINT64*)(_this + 0x680 - 0x20), //r5
				*(UINT64*)(_this + 0x680 - 0x20 + 8), //r6
				*(UINT64*)(_this + 0x680 - 0x20 + 0x10), //r7
				(UINT64)*(unsigned int*)(_this + 0x680 - 0x2C + 0x18), //r8
				(_this + 0x488) //Effect Group (GE1,GE2,GE3) maybe
				); //LoadEffectFunc

			BranchTo(0x8221F3C8,int,_this + 0x644,&_fake_boost_container);
			if (_fake_boost_container[1])
				BranchTo(0x821601B8,int,_fake_boost_container[1]);


		}
		//HELP (my effects)
		else if (flag != 0x47)
		{

			BranchTo(0x82227E70,int,_this,flag,flag2);
		}

	}
	void Sonic_Effect_OnUnknownFlags01(int _this,int flags){

		
		DWORD _fake_boost_container[2] = {0};
		

		//Homing Smash
		if ((flags & 0x8000000) != 0){
	
			BranchTo(0x82225BD8,int,
				&_fake_boost_container, //r3
				_this + 0xC, // (GE1,GE2,GE3 and some like this, buffers) r4
				*(UINT64*)(_this + 0x69C - 0x20), //r5
				*(UINT64*)(_this + 0x69C - 0x20 + 8), //r6
				*(UINT64*)(_this + 0x69C - 0x20 + 0x10), //r7
				(UINT64)*(unsigned int*)(_this + 0x69C	 - 0x2C + 0x18), //r8
				(_this + 0x488) //Effect Group (GE1,GE2,GE3) maybe
				); //LoadEffectFunc

			BranchTo(0x8221F3C8,int,_this + 0x48C,&_fake_boost_container);
			if (_fake_boost_container[1])
				BranchTo(0x821601B8,int,_fake_boost_container[1]);


		}
		//Homing Smash Release
		else if ((flags & 0x4000000) != 0){
			BranchTo(0x82225BD8,int,
				&_fake_boost_container, //r3
				_this + 0xC, // (GE1,GE2,GE3 and some like this, buffers) r4
				*(UINT64*)(_this + 0x6B8 - 0x20), //r5
				*(UINT64*)(_this + 0x6B8 - 0x20 + 8), //r6
				*(UINT64*)(_this + 0x6B8 - 0x20 + 0x10), //r7
				(UINT64)*(unsigned int*)(_this + 0x6B8 - 0x2C + 0x18), //r8
				(_this + 0x488) //Effect Group (GE1,GE2,GE3) maybe
				); //LoadEffectFunc

			BranchTo(0x8221F3C8,int,_this + 0x48C,&_fake_boost_container);
			if (_fake_boost_container[1])
				BranchTo(0x821601B8,int,_fake_boost_container[1]);

		
		}
		else
		{

		BranchTo(0x82228DF0,int,_this,flags);
		}


	}


	void GlobalInstall_SonicEffect()
	{
		WRITE_DWORD(0x8219DB40,POWERPC_ADDI(3,0,0x680 + (3*0x1C)));

		INSTALL_HOOK(EffectModuleSonicHook);
		WRITE_DWORD(0x8200C410,Sonic_Effect_OnAnimationChange);
		WRITE_DWORD(0x8200C424,Sonic_Effect_OnUnknownFlags01);
		
	}
	
}