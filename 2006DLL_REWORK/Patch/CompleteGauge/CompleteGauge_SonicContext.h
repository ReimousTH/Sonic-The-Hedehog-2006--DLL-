#pragma once
#include <Core/Debug.h>
#include <Player/Input/ListenerNormal.h>
#include <Player/State/SonicContext.h>
#include <Hook/HookBase.h>
#include "CompleteGauge_SonicStates.h"
#include <Patch/LuaExtension/DebugLog.h>
namespace CompleteGauge{
	


	class SonicContextExtended:public  Sonicteam::Player::State::SonicContext{

	public:
		bool IsSuper;
		float c_super_ring_dec_time;

	};
	//temp story values
	static float gauge_storage[8] = {0};
	static int gauge_levels_storage[8] = {0};

	
	void GlobalInstall_SonicContext();
	
}