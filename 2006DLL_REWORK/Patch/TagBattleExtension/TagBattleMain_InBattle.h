#pragma once


#include <Hook/HookBase.h>
#include <Patch\LuaExtension\MessagesUtil.h>
#include <string>

#include <Player/State/IMachine.h>
#include <DocMarathonImp.h>

#include <vector>

#include <Core/All06Functions.hpp>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}






namespace TagBattleMain{


	extern int GoalCount;
	extern std::vector <int> GoalActors;
	void GlobalInstall_InBattle();

}


