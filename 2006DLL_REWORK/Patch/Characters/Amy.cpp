#include "Amy.h"

namespace AmyLOS{





	int __fastcall sub_8223F9F8(int a1, unsigned int a2){


		int result = *(DWORD *)(a1 + 0x34);
		DWORD** v2 = (DWORD **)result;
		int Flags = *(unsigned __int8 *)(result + 0xD4);

		byte a2_f = a2 >> 24;

		int IsShowHammer = a2_f & 0x40;
		int IsHammerAttack = a2_f & 0x80;

		int CIsShowHammer = Flags & 0x40;
		int CIsHammerAttack = Flags & 0x80;


		if (IsShowHammer != CIsShowHammer){

			if ((IsShowHammer & 0x40) != 0){

				*(unsigned __int8 *)(result + 0xD4) |= 0x40;
			}
			if ((IsShowHammer & 0x40) == 0){

				*(unsigned __int8 *)(result + 0xD4) ^= 0x40;
			}



			if (IsShowHammer){
				(*(void (__fastcall **)(DWORD, int))(**(DWORD **)(result + 0x54) + 0x14))(
					*(DWORD *)(result + 0x54),
					result + 0xC4);
			}
			else{
				(*(void (__fastcall **)(DWORD))(**(DWORD **)(result + 0xC4) + 0x20))(*(DWORD *)(result + 0xC4));

			}
		}
		if (IsHammerAttack != CIsHammerAttack){

			if ((IsHammerAttack & 0x40) != 0){

				*(unsigned __int8 *)(result + 0xD4) |= 0x80;
			}
			if ((IsHammerAttack & 0x40) == 0){

				*(unsigned __int8 *)(result + 0xD4) ^= 0x80;
			}



			if (IsHammerAttack){
				(*(void (__fastcall **)(DWORD *, DWORD **))(*v2[0x15] + 0x18))(v2[0x15], v2 + 0x32);
				result = (*(int (__fastcall **)(DWORD *, DWORD **))(*v2[0x30] + 0x3C))(v2[0x30], v2 + 0x14);
			}
			else{
				(*(void (__fastcall **)(DWORD *))(*v2[0x32] + 0x20))(v2[0x32]);
				result = (*(int (__fastcall **)(DWORD *))(*v2[0x30] + 0x40))(v2[0x30]);
			}



		}










		return BranchTo(0x8223F9F8,int,a1,a2);

	}

	void  Context_Amy_CoreX(Sonicteam::Player::State::AmyContext *a1, double a2){


		///((byte*)&a1->UnkownFlagsUnk01)[6] = 0;

		//	a1->IsJumped_PostureVelocityYHandle = 1;
		//a1->VelocityY = 10000;


		BranchTo(0x82206D78,int,a1,a2);
		byte IsAmyAttackX =  (char)((a1->AmyUnkFlags >> 24  ) & 0xFF); // Get First Byte (ALSO it complies as HIBYTE)
		if (IsAmyAttackX != 0){

			//ShowXenonMessage(L"MSG","");
			a1->ExportWeaponRequestFlag |= 0x80000000; //AmyAttack
			a1->ExportWeaponRequestFlag |= 0x40000000; //AmyHammer
		}
		if (a1->CurrentAnimation == 6){
			a1->ExportWeaponRequestFlag |= 0x40000000; //AmyHammer

		}





	}


	void sub_821AB7D0(Sonicteam::Player::State::BasedObject<Sonicteam::Player::State::AmyContext> *a1)
	{


		Sonicteam::Player::State::AmyContext* ptr = (Sonicteam::Player::State::AmyContext*)a1->CObjContext;

		BranchTo(0x821AB7D0,int,a1);


		ptr->base_speed_y =  ptr->c_jump_double_speed;


	}







	void AmyAttackStartNew(AmyAttack *a1){

		(*(byte*)(&a1->BOContext->AmyUnkFlags )) = 1;
		a1->Time = 0;
		a1->Flags0 = 0;

		//hammerjump
		if (a1->CObjContext->CurrentAnimation == 6){
			a1->Flags0 |= AmyAtk01; 
			a1->CObjContext->SetAnimation(0x45);
		}


		if (a1->Flags0 == 0){
			a1->CObjContext->SetAnimation(0x45);


			if ((a1->CObjContext->GroundAirFlags & 1) != 0){
				a1->CObjContext->IsGravityDisabled = 0;
				a1->CObjContext->base_speed_z = 0;
				a1->CObjContext->base_speed_y = 0;
				a1->CObjContext->gimmick_speed_z = 0;
			}

		}


	}


	int AmyAttackUpdateNew(AmyAttack *a1,double delta){

		if (BranchTo(0x8220A938,int,a1,delta)){
			return 1;
		}

		if ((a1->CObjContext->GroundAirFlags & 1) != 0  ){

			//Cancel Hammer Jump
			if ( (a1->CObjContext->Input & 0x400u) != 0 &&  IsAttack(AmyAtk01,a1->Flags0)){
				a1->Flags0 ^= AmyAtk01;
			} 
			if ( (a1->CObjContext->AnimationState & 1) != 0 &&  IsAttack(AmyAtk01,a1->Flags0)) {

				a1->CObjContext->IsJumped_PostureVelocityYHandle=1;
				a1->CObjContext->base_speed_y = a1->BOContext->c_jump_double_speed;
				a1->CObjContext->SetAnimation(0xAF);
				a1->Flags0 |= AmyAtk02;
				a1->Time = 0.035f;
			}

			if (a1->CObjContext->base_speed_z > 0.0f){
				a1->CObjContext->base_speed_z -= a1->CObjContext->c_brake_acc *delta * 0.1f;

			}
		}
		//air combos
		else{
			if ((a1->CObjContext->Input & 0x400u) != 0 && !IsAttack(AmyAtk04,a1->Flags0)){

				//a1->CObjContext->IsJumped_PostureVelocityYHandle=1;
				//a1->CObjContext->VelocityY = a1->BOContext->c_jump_double_speed;
				a1->CObjContext->SetAnimation(0xB0);
				a1->Flags0 |= AmyAtk04;
				a1->Time = 0.035f;
			}
			//Hold
			else if ((a1->CObjContext->Input & 0x200u) != 0 && IsAttack(AmyAtk04,a1->Flags0) && !IsAttack(AmyAtk05,a1->Flags0) ){

				a1->CObjContext->SetAnimation(0xAF);
				a1->Flags0 |= AmyAtk05;
				a1->Time = 0.02f;
			}
			if (IsAttack(AmyAtk05,a1->Flags0) && (a1->CObjContext->Input & 0x200u) != 0 ){
				a1->Time = delta + 0.01;
			}


		}

		if ((a1->CObjContext->GroundAirFlags & 1) != 0  )
		{
			if (IsAttack(AmyAtk04,a1->Flags0) || IsAttack (AmyAtk05,a1->Flags0)){

				return BranchTo(0x82209068,int,a1);
			}	

		}


		if ((a1->CObjContext->AnimationState & 1) != 0){
			a1->Time -= delta;


			//Reset
			if (IsAttack(AmyAtk02,a1->Flags0)) a1->CObjContext->IsJumped_PostureVelocityYHandle = 0;
		}

		if ( (a1->CObjContext->AnimationState & 1) != 0 && a1->Time <= 0 ){
			return BranchTo(0x82209068,int,a1);
		}

		//0x45(attack),0xAF(AIR SPIN), 0xB0(SPIN )

	}

	void AmyAttackEndNew(AmyAttack *a1){
		(*(byte*)(&a1->BOContext->AmyUnkFlags )) = 0;

		a1->CObjContext->IsGravityDisabled = 0;



	}

	int AmyJumpUpdate(AmyAttack *a1,double delta){

		if ((a1->CObjContext->Input & 0x400u) != 0){

			//ShowXenonMessage(L"MSG","1");
			a1->ObjectMashine->ChangeMashineState(0x60);

			return 1;

		}

		return BranchTo(0x821AB048,int,a1,delta);

	}






	void AmyStealthStartNew(AmyStealth *a1){

		//a1->CObjContext->IsForcedMovement = 1;
		//a1->CObjContext->CurrentSpeed = 0;
		a1->CObjContext->gimmick_speed_z = 0;
		a1->CObjContext->IsInvulnerable2 = 1;
		(*(byte*)(&a1->BOContext->AmyUnkFlags )) = 1;
		a1->Time = 0;
		a1->CObjContext->SetAnimation(0xB0);
	}
	int AmyStealthUpdateNew(AmyStealth *a1,double delta){


		if ( (unsigned __int8)StateCommonGroundFlagsListenerForStates((int *)a1, delta) )
			return 1;
		BranchTo(0x8220A630,int,a1->CObjContext,delta,a1->CObjContext->c_run_acc * 0.25,a1->CObjContext->c_run_speed_max * 0.65,0.0);
		a1->Time += delta;
		if (a1->Time >= ((AmyContextE*)a1->CObjContext)->c_before_dizzy_time)
		{
			a1->ObjectMashine->ChangeMashineState(9);
		}
		if (((a1->CObjContext->Input)  & 0x400 ) != 0 )
		{
			return BranchTo(0x82209068,int,a1);
		}

		return 0;
	}
	void AmyStealthEndNew(AmyStealth *a1){

		a1->CObjContext->IsInvulnerable2 = 0;
		(*(byte*)(&a1->BOContext->AmyUnkFlags )) = 0;
		a1->BOContext->IsAnimationRotationLocked = 0;
		//a1->BOContext->StealthTime = a1->BOContext->c_stealth_limit;
	}


	//MakeStateI(AmyFallingT,AmyFalling);






	void JumpAmyStart(CommonJump* _this){

		//0xD - Bruh
		//0xE - Spin


		//jump double
		//0xB1 - Spin
		//0xB2 - Air (Player Voice)

		//	int table1[] = {0xD,0xE};
		//	int table2[] = {0xB1,0xB2};


		int table1[] = {0xD,0xB2};
		int table2[] = {0xE,0xB1};

		Sonicteam::Player::State::AmyContext* _v = (Sonicteam::Player::State::AmyContext*)(_this->CObjContext);



		_this->AnimNum = table2[_v->AvailableJumpNum % 2];
		_this->AnimNumS = table1[_v->AvailableJumpNum % 2];

		if (_v->AvailableJumpNum <= 0){
			_this->AnimNum = table1[1];
		}





		BranchTo(0x82209208 ,int,_this);
	}


	void AmyNewParams(unsigned int _this,Sonicteam::LuaSystem*& lp){


		BranchTo(0x82207028,int,_this,&lp);
		AmyContextE* t = (AmyContextE*)(_this - 0x20);
		t->c_before_dizzy_time =  lp->GetFloatValue(std::string("c_before_dizzy_time"));


	}

	void GlobalInstall()
	{

		WRITE_DWORD(0x821B5E3C,POWERPC_LI(3,0x264));
		WRITE_DWORD(0x82009648,AmyNewParams); //OnVarible



		//jump
		WRITE_DWORD(0x820049A0,JumpAmyStart);




		WRITE_DWORD(0x8200D5AC,sub_8223F9F8);
		WRITE_DWORD(0x8223FA34,0x60000000); //Disable Origin Hammer Settup

		WRITE_DWORD(0x82009658,Context_Amy_CoreX);


		//	WRITE_DWORD(0x821AB08C,POWERPC_ADDI(4,0,0x5F));



		WRITE_DWORD(0x821AB5B8,POWERPC_LI(4,4));
		WRITE_DWORD(0x821AB520,0x60000000);
		//Jump Double fixed
		WRITE_DWORD(0x821AB598,0x60000000);
		WRITE_DWORD(0x821AB5A0,0x60000000);
		WRITE_DWORD(0x821AB5B0,0x60000000);
		WRITE_DWORD(0x821AB5D4,0x60000000);

		WRITE_DWORD(0x821AB508,0x60000000);
		WRITE_DWORD(0x821AB50C,0x60000000);
		WRITE_DWORD(0x821AB510,0x60000000);
		WRITE_DWORD(0x821AB514,0x60000000);
		WRITE_DWORD(0x821AB520,0x60000000);




		WRITE_DWORD(0x821AB630,POWERPC_ADDI(3,0,0x18));
		WRITE_DWORD(0x82004AC4,AmyAttackStartNew);
		WRITE_DWORD(0x82004AC8,AmyAttackUpdateNew);
		WRITE_DWORD(0x82004ACC,AmyAttackEndNew);



		WRITE_DWORD(0x820049A4,AmyJumpUpdate);
		//WRITE_DWORD(0x82004AF4,sub_821AB7D0);


		WRITE_DWORD(0x821AB288,POWERPC_ADDI(3,0,0x14));
		WRITE_DWORD(0x82004A04,AmyStealthStartNew);
		WRITE_DWORD(0x82004A08,AmyStealthUpdateNew);
		WRITE_DWORD(0x82004A0C,AmyStealthEndNew);


		//	WRITE_DWORD(0x82B169F0 + 0x4,)

	}






}