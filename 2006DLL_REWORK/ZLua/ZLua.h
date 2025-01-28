#ifndef DLL_ZLUA
#define DLL_ZLUA


extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


#include <Core/Debug.h>
#include <Hook/HookBase.h>
#include <xtl.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>   


class ZLua
{
public:
	std::string FilePath;
	lua_State* L;
	bool executed;
	bool locked;


	void OpenLua();
	void UseBaseLibs();
	void DoFile(bool);
	void CallFunction();

	const char* ZLua::GetGlobalString(const char* string);
	bool GetGlobalBool(const char* string);
	int ZLua::GetGlobalInt(const char* string);

	static void UseBaseLibsEx(lua_State* L);


	ZLua(void);
	ZLua(const char*);
	~ZLua(void);
};



#endif

