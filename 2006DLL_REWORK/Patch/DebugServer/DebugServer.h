#pragma once

#include "Socket.h"



extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}





namespace DebugServer
{
	extern Socket _socket;
	extern lua_State* L;

	void GlobalInstall();
};
