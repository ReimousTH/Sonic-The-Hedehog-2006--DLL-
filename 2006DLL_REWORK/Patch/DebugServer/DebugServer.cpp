#include "DebugServer.h"



#include <Sox/Thread.h>

using namespace DebugServer;




class DebugThread:public Sonicteam::SoX::Thread{

public:
	

	DebugThread():Sonicteam::SoX::Thread("Debug",0,false){

	}

	virtual void ThreadStep(double delta)
	{
		_socket.UpdateServer();
	}



};



CUSTOM_STRUCT_MESSAGE(SMDATA_COMMAND, -1, IPPROTO_UDP, 0)
	char command[256]; //message :)
};

CUSTOM_STRUCT_MESSAGE(SMDATA_COMMAND_OUT_PRINT, -1, IPPROTO_UDP, 0)
char command[256]; //message :)
};









//Later
void MessageRec(SocketMessage* message, SOCKET sock){
	
	//
	
	if (message->EqualID<SMDATA_COMMAND>()){
		std::string command = (const char*)&(message->GetDataAs<SMDATA_COMMAND>()->command);
		lua_dostring(L,command.c_str()); //Process Command in lua
	}



}




//network print
static int luaB_print (lua_State *L) {
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	const char* s;
	lua_getglobal(L, "tostring");
	for (i=1; i<=n; i++) {
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);  /* get result */
		if (s == NULL)
			return luaL_error(L, "`tostring' must return a string to `print'");
		if (i>1) fputs("\t", stdout);
		fputs(s, stdout);
		lua_pop(L, 1);  /* pop result */
	}
	fputs("\n", stdout);


	SMDATA_COMMAND_OUT_PRINT jm;
	strcpy((char*)&jm.command,s); //copy out	
	
	SocketMessage msg = SocketMessageFromConst(jm);

	_socket.SendToClientsTUD(&msg);


	return 0;
}




void DebugServer::GlobalInstall()
{
	_socket.Build(true,"127.0.0.1","127.0.0.1");
	_socket.StartUP();
	_socket.CSSS_MESSAGE_RECIEVED = MessageRec;

	L = lua_open();
	luaopen_base(L);
	luaopen_io(L);
	luaopen_math(L);
	luaopen_string(L);

	lua_register(L,"print",luaB_print);


	DebugThread* _thread =  new DebugThread();
	_thread->Resume();






}
