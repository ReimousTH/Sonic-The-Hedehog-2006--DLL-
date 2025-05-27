#include "ZLua.h"


extern "C" int WriteVirtualBytes(lua_State* L){
	int nargs = lua_gettop(L);
	if (nargs < 2) return 0;
	std::stringstream conv;
	unsigned int StartOffset,LastOffset;
	const char* StartOffsetS = lua_tostring(L,1);
	const char* ByteArrayString = lua_tostring(L,2);
	conv << std::hex << StartOffsetS; conv >> StartOffset; conv.clear(); 	conv.str("");
	std::string ByteArrayStringR = std::string(ByteArrayString);
	ByteArrayStringR.erase(std::remove( ByteArrayStringR.begin(),
		ByteArrayStringR.end(),
		' '),
		ByteArrayStringR.end());
	int len  = ByteArrayStringR.length();
	if (len % 2 != 0){ //Add Extra Zero
		ByteArrayStringR.append("0");
	}
	const char* cu = ByteArrayStringR.c_str();
	char* bt = new char[strlen(cu)/2];
	char buff[2] = {0,0};
	for (int i =0 ;i<strlen(cu)/2;i++){
		memcpy((void*)&buff,(void*)((unsigned int)cu + (i*2)),2);
		bt[i] = (unsigned int)(strtoul(buff,NULL,16));

	}	
	DWORD dw;
	VirtualProtectEx(GetModuleHandle(NULL),(void*)(ADDR(StartOffset)),strlen(cu)/2,1,&dw);
	memcpy((void*)ADDR(StartOffset),bt,strlen(cu)/2);
	VirtualProtectEx(GetModuleHandle(NULL),(void*)(ADDR(StartOffset)),strlen(cu)/2,dw,&dw);


	//Free Buffer
	free(bt);




	return 0;
}
extern "C" int WriteVirtualBytesRange(lua_State* L){
	int nargs = lua_gettop(L);
	if (nargs < 2) return 0;
	std::stringstream conv;
	DWORD StartOffset,EndOffset,LastOffset;
	const char* StartOffsetS = lua_tostring(L,1);
	const char* EndOffsetS = lua_tostring(L,2);
	const char* ByteArrayString = lua_tostring(L,3);
	conv << std::hex << StartOffsetS; conv >> StartOffset; conv.clear(); 	conv.str("");
	conv << std::hex << EndOffsetS; conv >> EndOffset; conv.clear(); 	conv.str("");
	std::string ByteArrayStringR = std::string(ByteArrayString);

	ByteArrayStringR.erase(std::remove( ByteArrayStringR.begin(),
		ByteArrayStringR.end(),
		' '),
		ByteArrayStringR.end());


	int len  = ByteArrayStringR.length();
	if (len % 2 != 0){ //Add Extra Zero
		ByteArrayStringR.append("0");
	}
	const char* cu = ByteArrayStringR.c_str();
	byte* bt = new byte[strlen(cu)/2];
	char buff[2] = {0,0};
	for (int i =0 ;i<strlen(cu)/2;i++){
		memcpy((void*)&buff,(void*)((unsigned int)cu + (i*2)),2);
		bt[i] = (unsigned int)(strtoul(buff,NULL,16));

	}	
	DWORD dw;

	for (int i = StartOffset; i<EndOffset;i+=strlen(cu)/2 ){
		VirtualProtectEx(GetModuleHandle(NULL),(void*)(ADDR(StartOffset)),strlen(cu)/2,1,&dw);
		memcpy((void*)ADDR(i),bt,strlen(cu)/2);
		VirtualProtectEx(GetModuleHandle(NULL),(void*)(ADDR(StartOffset)),strlen(cu)/2,dw,&dw);
	}
	free(bt);
}



unsigned int ZL_GetNumber(lua_State* L, int arg) {
	unsigned int return_value = 0;

	if (lua_isnumber(L, arg)) {
		return_value = (unsigned int)lua_tonumber(L, arg); // C
	} else if (lua_isuserdata(L, arg)) {
		return_value = (unsigned int)(size_t)lua_touserdata(L, arg); // Cast to unsigned int
	} else if (lua_isstring(L, arg)) {
		const char* str = lua_tostring(L, arg);
		char* endptr;
		return_value = strtoul(str, &endptr, 16); // Base 16 for hex
		if (*endptr != '\0' && *endptr != ' ') { 
			luaL_error(L, "Invalid hex string: '%s'", str);
			return 0; 
		}
		if (errno == ERANGE)
		{
			luaL_error(L,"Hex string '%s' is out of unsigned integer range.",str);
			return return_value; //Will not reach here
		}
	} else {
		// Handle invalid argument type
		luaL_error(L, "Argument %d must be a number, userdata, or hex string.", arg);
		return return_value;
	}

	return return_value;
}


//just shell
HOOKV3EX(0,void*,LuaHookReturnVoidZ,(void*,void*,void*,void*,void*,void*,void*,double,double,double,double,double,double),(r3z,r4z,r5z,r6z,r7z,r8z,r9z,fp1z,fp2z,fp3z,fp4z,fp5z,fp6z),void* r3z,void* r4z,void* r5z,void* r6z,void* r7z,void* r8z,void* r9z,double fp1z,double fp2z,double fp3z,double fp4z,double fp5z,double fp6z){
	return (void*)(0);
}





extern "C" int InitHOOK(lua_State* L){

	void* addr_to = (void*)ZL_GetNumber(L,1);
	int reg = (int)ZL_GetNumber(L,2); 
	HookNew* hook =  HookNew::CreateHook((void*)LuaHookReturnVoidZ,(void*)LuaHookReturnVoidZMAP,addr_to,1,0,0,-1,false,reg);
	return 0;
}


ZLua::ZLua(void)
{
	OpenLua();
	UseBaseLibs();
	executed = false;
}

ZLua::~ZLua(void)
{
	lua_close(L);
}
ZLua::ZLua(const char* a1){
	FilePath = std::string(a1);
	OpenLua();
	UseBaseLibs();
	executed = false;
	
	
}
void ZLua::UseBaseLibsEx(lua_State* L){
	

	
	lua_register(L,"WriteVirtualBytes",WriteVirtualBytes);
	lua_register(L,"WriteVirtualBytesRange",WriteVirtualBytesRange);
	lua_register(L,"WriteVirtialBytes",WriteVirtualBytes);
	lua_register(L,"InitHOOK",InitHOOK);


	luaopen_base(L);
	luaopen_debug(L);
	luaopen_string(L);


	DebugLogV2::MessageUtilGlobalInstall(L);
	DebugLogV2::DebugLabel_GlobalInstall(L);
	DebugLogV2::BitLibGlobalInstall(L);
	DebugLogV2::PlayerLIB_GlobalInstall(L);
	DebugLogV2::STRLIB_GlobalInstall(L);
	DebugLogV2::MemoryLIB_GlobalInstall(L);
	DebugLogV2::BufferLIB_GlobalInstall(L);
	DebugLogV2::Uint64LIB_GlobalInstall(L);
	DebugLogV2::VectorRLIB_GlobalInstall(L);
	DebugLogV2::XMMATRIX_GlobalInstall(L);
	DebugLogV2::MainDisplayTask_GlobalInstall(L);
	DebugLogV2::GameImp_GlobalInstall(L);
	DebugLogV2::MessageReceiver_GlobalInstall(L);
	DebugLogV2::GlobalInstall_LuaLCommonObject(L);
	DebugLogV2::GlobalInstall_StateIMachine(L);
	DebugLogV2::GlobalInstall_StateMachine2(L);
	DebugLogV2::GlobalInstall_PlayerRework(L);
	DebugLogV2::OpenState_GlobalInstall(L);
	DebugLogV2::GlobalInstall__LoadReAttachArc(L);
	DebugLogV2::IO_GlobalInstall(L);
	
}

void ZLua::UseBaseLibs(){

	UseBaseLibsEx(L);

}
void ZLua::OpenLua(){
	L = lua_open();


}

const char* ZLua::GetGlobalString(const char* string){

	lua_getglobal(this->L,string);
	return lua_tostring(this->L,-1);
	
}


extern "C" bool ZLua::GetGlobalBool(const char* string){


	lua_getglobal(this->L, string);

	
	if (lua_isboolean(this->L, -1)) {
		return lua_toboolean(this->L, -1);	
	}


	lua_pop(this->L, 1);


	
}


int ZLua::GetGlobalInt(const char* string, int defaultt)
{
	int _return = 0;
	lua_getglobal(this->L, string);
	if (lua_isnumber(this->L, -1)) {
		_return = lua_tonumber(this->L, -1);  // Added missing '=' here
	}
	else {
		_return = defaultt;
	}
	lua_pop(this->L, 1);
	return _return;
}
void ZLua::DoFile(bool ignore){

	locked = true;
	if (ignore){
		executed =false;

	}

	if (!executed) {

		int re = luaL_loadfile(L, FilePath.c_str());
		if ( re== LUA_ERRSYNTAX || re == LUA_ERRFILE ){
			PushXenonMessage(L"ERROR",lua_tostring(L,-1));
			lua_close(L);
			Sleep(10000);
			exit(0);
			return;
		}
		else{
			lua_pcall(L, 0,0,0);
		}

			//   ShowXenonMessage(L"DUMBY",luaL_loadfile(L, FilePath.c_str()),0);

	
		executed = true;

	}
	locked = false;
	
}
//testing
void ZLua::CallFunction(){

}


