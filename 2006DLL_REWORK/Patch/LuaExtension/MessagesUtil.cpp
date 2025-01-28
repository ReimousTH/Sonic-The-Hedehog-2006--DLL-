#include "MessagesUtil.h"



namespace DebugLogV2{


	UINT32 SpawnMessageBase(float pos_x,float pos_y,const wchar_t* MSG,const char* PMSG,int FontIndex,int ColorFlag){
		Sonicteam::DocMarathonImp* doc = *(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		UINT32 Resouce = doc->DocGetUnkGameResources();
		// 8 * (index + 3)
		//2 bold (little bigger 0)
		//1 bold, but bigger
		//0 --common
		//Font
	
		UINT32 RequiredResouce = (8*(FontIndex+3)) + Resouce;

		//wchar_t or i
		//i dunno text-size???// 
		//Z-Inddex ???
		//MSG
		//MSG-Picture 
		//picture(button_a),picture(button_b)
		//picture(button_b) 
		UINT32 TextEntity = BranchTo(0x8262DC60,UINT32,malloc06(0x110),RequiredResouce,MSG,PMSG,128);
		BranchTo(0x8262AF00,int,TextEntity,20.0); //unk
		XMVECTOR* pos = (XMVECTOR*)(TextEntity + 0x30);
		pos->x = pos_x;
		pos->y = pos_y;
		*(_BYTE *)(TextEntity + 0xDD) = 1; //reset
		//ARGB(Alpha,Red,Green,Blue)
		BranchTo(0x8262B288,int,TextEntity,&ColorFlag);
		BranchTo(0x8262B008,int,TextEntity);
		return TextEntity;
	}

}

int DebugLogV2::ThreadLog = false;

void DebugLogV2::ChangeMessagePosition(UINT32 TextEntity,float x,float y)
{

	XMVECTOR* pos = (XMVECTOR*)(TextEntity + 0x30);
	pos->x = x;
	pos->y = y;
	*(_BYTE *)(TextEntity + 0xDD) = 1;
	BranchTo(0x8262AF00,int,TextEntity,30.0);
	BranchTo(0x8262B008,int,TextEntity);


}

void DebugLogV2::ChangeMessagePositionY(UINT32 TextEntity,float y)
{
	XMVECTOR* pos = (XMVECTOR*)(TextEntity + 0x30);
	pos->x = 0.0;
	pos->y = 480 + y;
	*(_BYTE *)(TextEntity + 0xDD) = 1;
	BranchTo(0x8262AF00,int,TextEntity,30.0);
	BranchTo(0x8262B008,int,TextEntity);
}

void DebugLogV2::EditMessage(UINT32 TextEntity,const wchar_t* msg)
{
	*(_BYTE *)(TextEntity + 0xDD) = 1;
	BranchTo(0x8262DB90,int,TextEntity,msg,0); //New Text
	BranchTo(0x8262B008,int,TextEntity);
}

void DebugLogV2::EditMessageColor(UINT32 TextEntity,char Alpha,char Red,char Green,char Blue)
{

	UINT flag = ((Alpha << 24) |(Red << 16) | (Green << 8) |  Blue );
	BranchTo(0x8262B288,int,TextEntity,&flag);
	BranchTo(0x8262B008,int,TextEntity);
}

void DebugLogV2::EditMessage(UINT32 TextEntity,const wchar_t* msg,const char* pmsg)
{
	*(_BYTE *)(TextEntity + 0xDD) = 1;
	BranchTo(0x8262DB90,int,TextEntity,msg,pmsg); //New Text
	BranchTo(0x8262B008,int,TextEntity);
}





UINT32 DebugLogV2::SpawnMessage(const wchar_t* msg,float pos_x,float pos_y)
{
	return  SpawnMessageBase(pos_x,pos_y,msg,0,0,0xFFFFFFFF); 
}






UINT32 DebugLogV2::SpawnMessage(const wchar_t* msg,const char* picturemsg,float pos_x,float pos_y)
{

	return  SpawnMessageBase(pos_x,pos_y,msg,picturemsg,0,0xFFFFFFFF);
}
UINT32 DebugLogV2::SpawnMessage(const wchar_t* msg,float pos_y)
{
	return SpawnMessageBase(0,480.0+pos_y,msg,0,0,0xFFFFFFFF);
}















namespace DebugLogV2{
wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString=new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}


// Assuming DebugLog is your std::vector<std::string>
std::wstring ConcatenateStrings(const std::vector<std::string>& debugLog)
{
	std::wstringstream ss;

	for (std::vector<std::string>::const_iterator it = debugLog.begin(); it != debugLog.end(); ++it)
	{
		ss << std::wstring(it->begin(), it->end());
		ss << "\r\n";
	}

	return ss.str();
}

DWORD WINAPI ThreadProc( LPVOID lpParameter )
{
	LPCWSTR g_pwstrButtonsXx[1] = { L"------------OK----------------" };
	DWORD dwThreadNumber = (DWORD) lpParameter;



	while (true){

		/*
		ATG::GAMEPAD* gc = ATG::Input::GetMergedInput(0);


		if (gc->wPressedButtons & XINPUT_GAMEPAD_DPAD_UP){
		
			MESSAGEBOX_RESULT result;
			XOVERLAPPED m_Overlapped; 
			XShowMessageBoxUI(0,L"DebugLog V2.0",DebugLogV2::ConcatenateStrings(log).c_str(),1,g_pwstrButtonsXx,1,XMB_ALERTICON,&result,&m_Overlapped);


		//	while (result.dwButtonPressed != 0){
		//	}
			Sleep(1000);




		}
		if (gc->wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN){

			DebugLogV2::log.clear();

		}
		*/


	}


	return 0;
}



static int luaB_print (lua_State *L) {

	int n = lua_gettop(L);  /* number of arguments */
	int i;
	lua_getglobal06(L, "tostring");
	for (i=1; i<=n; i++) {
		const char *s;
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call06(L, 1, 1);
		s = lua_tostring(L, -1);  /* get result */
		if (s == NULL)
			return luaL_error(L, "`tostring' must return a string to `print'");

		DebugLogV2::log.push_back(std::string(s));

		//if (i>1) fputs("\t", stdout);
		//fputs(s, stdout);
		lua_pop(L, 1);  /* pop result */
	}
	fputs("\n", stdout);
	return 0;
}

static int Printf (lua_State *L) {

	int n = lua_gettop(L);  /* number of arguments */
	int i;
	for (i=1; i<=n; i++) {
		const char *s;

		if (lua_isstring(L,i)){
			s = lua_tostring(L, i);  /* get result */
			DebugLogV2::log.push_back(std::string(s));
		}
		else{
			s = lua_tostring(L, -1);  /* get result */
			DebugLogV2::log.push_back(std::string("[PrintF]FailedMessage"));
		}
	

	}

	return 0;
}



}



std::vector<std::string> DebugLogV2::log = std::vector<std::string>();
extern "C" ShowDebugLog(lua_State* LS){


	LPCWSTR g_pwstrButtonsX_INL[1] = { L"------------OK----------------" };
	MESSAGEBOX_RESULT result;
	XOVERLAPPED m_Overlapped;
	ZeroMemory(&m_Overlapped, sizeof(XOVERLAPPED));

	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	 m_Overlapped.hEvent = hEvent;

	 XShowMessageBoxUI(0,L"DebugLog V2.0",DebugLogV2::ConcatenateStrings(DebugLogV2::log).c_str(),1,g_pwstrButtonsX_INL,1,XMB_ALERTICON,&result,&m_Overlapped);
	DWORD waitResult = WaitForSingleObject(hEvent, INFINITE);
	CloseHandle(hEvent);

	return 0;
}

extern "C" ClearDebugLog(lua_State* LS){

	DebugLogV2::log.clear();
	return 0;
}







void DebugLogV2::MessageUtilGlobalInstall(lua_State* L)
{

	if  (L == 0){
		WRITE_DWORD(0x8203B8AC,Printf);
		WRITE_DWORD(0x82049094,luaB_print);

		if (!HookV2::IsNotEmulatedHardWare && DebugLogV2::ThreadLog){
			const int stackSize = 65536;
			HANDLE Thr = CreateThread( NULL, stackSize, ThreadProc, (VOID *)0, CREATE_SUSPENDED, NULL );
			ResumeThread(Thr);
		}
	}
	else{
		lua_register06(L,"ShowDebugLog",ShowDebugLog);
		lua_register06(L,"ClearDebugLog",ClearDebugLog);
	}
}

