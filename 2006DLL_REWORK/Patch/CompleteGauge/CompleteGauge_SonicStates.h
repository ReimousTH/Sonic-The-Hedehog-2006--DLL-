#pragma once
#include <Hook/HookBase.h>
#include "CompleteGauge_SonicContext.h"
#include <Player/State/BasedObject.h>
#include <Player/State/SonicContext.h>


namespace CompleteGauge{
	
	class SonicHomingSmash:public Sonicteam::Player::State::BasedObject<Sonicteam::Player::State::SonicContext>{

	public:
		DWORD Flag1;
		DWORD Flag2;
		float charge_time;
	};
	
	void Switch(Sonicteam::Player::State::CommonContext* _context,const char* lua_name,const char* package_name,const char* sound_name,const char* char_name);
	void GlobalInstall_SonicStates();
	
}