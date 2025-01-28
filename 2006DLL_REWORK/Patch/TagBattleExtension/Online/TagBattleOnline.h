#pragma once
#include <Hook\HookBase.h>
#include <Core\Debug.h>

#include <Player/State/SonicContext.h>
#include <Player/RootFrame.h>
#include <Player/State/BasedObject.h>

#include <Patch\LuaExtension\MessagesUtil.h>
#include <string>


#include <AtgInput.h>

#include <DocMarathonImp.h>
#include <GameImp.h>
#include <vector>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}


#include "Socket.h"


namespace TagBattleMain{

	
	struct PLAYERS_DATA{

		XMVECTOR Rotation;
		XMVECTOR Position;
		int SelectionIndex;
		int LastSelectionIndex;
		int CurrentController;
		int input_data;
		int input_container[2];
		float hold_time;
		float hold_time_Y;
		int selected;

	};






	void GlobalInstall_ONLINE();

}


