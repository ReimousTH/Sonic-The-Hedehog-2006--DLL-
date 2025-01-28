#pragma once

#include <Core/All06Functions.hpp>
#include <Hook/HookBase.h>
#include <Player/State/BasedObject.h>
#include <Player/State/AmyContext.h>
#include <string>


#define AmyAtk01 0x1
#define AmyAtk02 0x2
#define AmyAtk03 0x4
#define AmyAtk04 0x8
#define AmyAtk05 0x10
#define AmyAtk06 0x20
#define AmyAtk07 0x40
#define AmyAtk08 0x80
#define AmyAtk09 0x100
#define IsAttack(atk,flags) ((atk & flags) != 0)



namespace AmyLOS{


	class AmyContextE:public Sonicteam::Player::State::AmyContext{

	public:
		float c_before_dizzy_time;
	};

	class AmyStealth:public Sonicteam::Player::State::BasedObject<Sonicteam::Player::State::AmyContext>{
	public:
		float Time;
	};

	class AmyFalling:public Sonicteam::Player::State::BasedObject<Sonicteam::Player::State::AmyContext>{
	public:
		float Time;
	};

	class  CommonFall:public Sonicteam::Player::State::CommonObject
	{
	public:
		float UnkTime;
		float FallStateFlags;
	};



	class CommonJump : public CommonFall
	{
	public:
		float UnkFloat01;
		int AnimNumS;
		int AnimNum;
		int UnkUint02;
	};

	class AmyAttack:public Sonicteam::Player::State::BasedObject<Sonicteam::Player::State::AmyContext>{
	public:int Flags0;
		   float Time;
	};




	void GlobalInstall();

}
