#pragma once


extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}


#include "MessagesUtil.h"
#include <xtl.h>

#include <iostream>
#include <tchar.h>
#include <map>
#include <string>

#include <Hook/HookBase.h>
#include <Player/State/Machine2.h>




#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>



namespace DebugLogV2 {



	

	int Pause_GlobalInstall(lua_State* LS);







	
}