#pragma once


extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}




namespace DebugLogV2 {

	void GlobalInstall__LoadReAttachArc(lua_State* L);
}