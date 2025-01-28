#pragma once
#include <Hook/HookBase.h>
#include "TagBattleMain_InBattle.h"
#include "Online/TagBattleOnline.h"



#include <Patch/LuaExtension/MessagesUtil.h>
#include <string>

#include <DocMarathonImp.h>

#include <vector>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <MainMenuTask.h>

#include <GameImp.h>




namespace TagBattleMain{

	class EmptyXNCPCSD:Sonicteam::SoX::RefCountObject,public Sonicteam::SoX::Engine::Task{


	};

	extern std::map<int, PLAYERS_DATA*> ordered_input;
	extern std::vector<int> origin_index_order;



	void GlobalInstall();

}


