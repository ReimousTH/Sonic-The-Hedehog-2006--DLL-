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


	lua_getglobal(this->L,string);
	if (lua_isboolean(this->L,-1)){
		return lua_toboolean(this->L,-1);	
	}
	lua_pop(this->L,1);
	return false;
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