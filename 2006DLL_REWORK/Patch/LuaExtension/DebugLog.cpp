#include "DebugLog.h"

#include <Prop/Manager.h>
#include <Prop/GenerateArea.h>
#include <Prop/Scene.h>

namespace DebugLogV2{


	int __declspec( naked ) sub_825DB498H(lua_State* LS){

		__asm{
			mflr      r12
				stw       r12, -8(r1)
				std       r31, -0x10(r1)

				stwu      r1, -0x60(r1)
				lis r11,0x825D
				ori r11,r11,0xB4A8
				mtctr r11
				bctr
		}
	}





	HOOK(int,__cdecl ,sub_825DB498,0x825DB498,lua_State* LS){
		




		BranchTo(0x825D9660,int,LS); //luaopen_base06
		BranchTo(0x825D7D18,int,LS); //luaopen_table06
		luaL_openlibf06(LS, 0,(const luaL_reg*)0x8203B8A8,15);
		BranchTo(0x82639830,int,LS); //open script
		

		luaopen_debug(LS);
		luaopen_string(LS);
	

	
		MessageUtilGlobalInstall(LS);
		DebugLabel_GlobalInstall(LS);
		BitLibGlobalInstall(LS);
		PlayerLIB_GlobalInstall(LS);
		STRLIB_GlobalInstall(LS);
		MemoryLIB_GlobalInstall(LS);
		BufferLIB_GlobalInstall(LS);
		Uint64LIB_GlobalInstall(LS);
		//VectorLIB_GlobalInstall(LS);
	
	




		VectorRLIB_GlobalInstall(LS);
		MainDisplayTask_GlobalInstall(LS);
		GameImp_GlobalInstall(LS);
		MessageReceiver_GlobalInstall(LS);
		
		GlobalInstall_LuaLCommonObject(LS);
		GlobalInstall_StateIMachine(LS);
		GlobalInstall_StateMachine2(LS);
		GlobalInstall_PlayerRework(LS);
		OpenState_GlobalInstall(LS);

	
		return sub_825DB498H(LS);
	}


	int __declspec( naked ) PlayBGMH(lua_State* LS){

		__asm{
			mflr      r12
				stw       r12, -8(r1)
				std       r30, -0x18(r1)
				std       r31, -0x10(r1)

				lis r11,0x8246
				ori r11,r11,0x23E8
				mtctr r11
				bctr

		}
	}
	HOOK(int,__cdecl ,PlayBGM,0x824623D8,lua_State* LS){



		
		int dt =   (int)lua_touserdata(LS, -10002); 
		BranchTo(0x8216B360,int,dt,lua_tostring(LS,1));



		return PlayBGMH(LS);
	}





	int __fastcall sub_821E0970(int a1, _BYTE r4_0){
		return BranchTo(0x821E0978,int,a1 -0x20,r4_0);
	}

	HOOK(void,__fastcall,nullsub_restore,0x821E0968,char* a1,char* a2,char* a3){
		if ((unsigned int)a1 > 0x40000000 && (unsigned int)a1 < 0x92000000){


	
		

			char chr = *a1;
			if (chr != 0 && isalpha(chr)){
				DebugLogV2::log.push_back(std::string(a1));
			}
					
			
	
	

			

		}

	}



	void GlobalInstall()
	{


		
		//WRITE_DWORD(0x820079F4,sub_821E0970); //MOVE
		//INSTALL_HOOK(nullsub_restore);

		INSTALL_HOOK(sub_825DB498); //MathLibReplacement
		INSTALL_HOOK(PlayBGM); //FIX



		GlobalInstall_StateMachine2(0);
		GlobalInstall_PlayerRework(0);
		GameLIB_GlobalInstall(0);
		MessageUtilGlobalInstall(0);
		OpenState_GlobalInstall(0);


	}
}



