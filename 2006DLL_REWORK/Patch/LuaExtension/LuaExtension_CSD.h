#ifndef LUAEXT_CSD
#define LUAEXT_CSD



#include <xtl.h>
#include <Patch/LuaExtension/LuaExtension_Misc.h>




namespace DebugLogV2 {


	extern "C" CPROJECT__CREATEMETATABLE(lua_State* L,void* arg1);
	extern "C" CSCENE__CREATEMETATABLE(lua_State* L,void* arg1);


	int CSD_GlobalInstall(lua_State* LS);




	
}
#endif