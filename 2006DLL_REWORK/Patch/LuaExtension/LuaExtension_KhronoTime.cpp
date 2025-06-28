#include "LuaExtension_KhronoTime.h"
#include <Core/Debug.h>

#include <GameImp.h>
#include <KhronoController.h>
#include <KhronoControlInputListener.h>
#include <NamedActor.h>


namespace DebugLogV2{



	extern "C" KhronoControllerMeta__NEW(lua_State* L,void* ptr){

		lua_getglobal(L,"Memory");
		lua_pushlightuserdata(L,ptr);
		lua_pcall06(L,1,1,0);
		luaL_getmetatable06(L, "KhronoControllerMeta");
		lua_setmetatable06(L,-2);
		return 1;
	}


	extern "C" KhronoControllerMeta__GetKhronoMode(lua_State* L){

		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::KhronoController* c =  (Sonicteam::KhronoController*)lua_touserdata(L,-1);lua_pop(L,1);
		lua_pushnumber(L,c->khrono_flag_0x60);
		return 1;
	}
	extern "C" KhronoControllerMeta__GetKhronoTimePass(lua_State* L){

		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::KhronoController* c =  (Sonicteam::KhronoController*)lua_touserdata(L,-1);lua_pop(L,1);
		lua_pushnumber(L,c->khrono_time_pass);
		return 1;
	}
	extern "C" KhronoControllerMeta__GetKhronoTime(lua_State* L){

		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::KhronoController* c =  (Sonicteam::KhronoController*)lua_touserdata(L,-1);lua_pop(L,1);
		lua_pushnumber(L,c->khrono_time);
		return 1;
	}
	extern "C" KhronoControllerMeta__GetKhronoBias(lua_State* L){

		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::KhronoController* c =  (Sonicteam::KhronoController*)lua_touserdata(L,-1);lua_pop(L,1);
		lua_pushnumber(L,c->khrono_time_bias);
		return 1;
	}


	extern "C" KhronoControllerMeta__SetKhronoMode(lua_State* L){

		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::KhronoController* c =  (Sonicteam::KhronoController*)lua_touserdata(L,-1);lua_pop(L,1);
		int mode = lua_tonumber(L,2); //1 - Manual, 2 - Auto
		c->khrono_flag_0x60 = mode; 
		return 0;
	}
	extern "C" KhronoControllerMeta__KhronoTimePass(lua_State* L){
		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::KhronoController* c =  (Sonicteam::KhronoController*)lua_touserdata(L,-1);lua_pop(L,1);
		c->khrono_time_pass = lua_tonumber(L,2);
		return 0;
	}

	extern "C" KhronoControllerMeta__KhronoSetTime(lua_State* L){
		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::KhronoController* c =  (Sonicteam::KhronoController*)lua_touserdata(L,-1);lua_pop(L,1);

		c->khrono_time = lua_tonumber(L,2);;
		c->khrono_time_pass = 0.0f;
		c->khrono_flag_0x60 = 0;

		return 0;
	}

	extern "C" KhronoControllerMeta__KhronoSetBias(lua_State* L){
		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::KhronoController* c =  (Sonicteam::KhronoController*)lua_touserdata(L,-1);lua_pop(L,1);
		c->khrono_time_bias = lua_tonumber(L,2);
		return 0;
	}




	extern "C" GetKhronoController(lua_State* L){



		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		UINT32 vft =  *(UINT32*)impl->DocCurrentMode;
		Sonicteam::GameImp* gameimp = *(Sonicteam::GameImp**)(impl->DocCurrentMode + 0x6C);
		Sonicteam::KhronoController* controller = NULL;
		for (LinkSoxNode<Sonicteam::SoX::Component>::iterator* it = gameimp->GameNamedActorSystemDependCameras->ListComponent.Next;it != &gameimp->GameNamedActorSystemDependCameras->ListComponent;it = it->Next){
			Sonicteam::SoX::Component* task =  it->_this;
			//AddMessage("task : %x",task);
			if (Sonicteam::KhronoController* c = dynamic_cast<Sonicteam::KhronoController*>(task)){
				controller = c;
				break;
			}
		}
		KhronoControllerMeta__NEW(L,controller);
		return 1;
	}
	


	int KhronoTime_GlobalInstall(lua_State* LS)
	{



	
		lua_register06(LS,"GetKhronoController", GetKhronoController);


		luaL_newmetatable06(LS, "KhronoControllerMeta");
		lua_pushstring06(LS,"__index"); lua_pushvalue(LS,-2); lua_settable06(LS,-3); // __index = PlayerMeta
		lua_newtable06(LS);lua_pushstring06(LS, "__index");luaL_getmetatable06(LS, "MemoryMeta");lua_settable06(LS, -3); lua_setmetatable06(LS, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })


		lua_pushstring06(LS, "SetMode");lua_pushcfunction06(LS, KhronoControllerMeta__SetKhronoMode);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		
		
		lua_pushstring06(LS, "GetTimePass");lua_pushcfunction06(LS, KhronoControllerMeta__GetKhronoTimePass);lua_settable06(LS, -3);
		lua_pushstring06(LS, "GetTime");lua_pushcfunction06(LS, KhronoControllerMeta__GetKhronoTime);lua_settable06(LS, -3);
		lua_pushstring06(LS, "GetBias");lua_pushcfunction06(LS, KhronoControllerMeta__GetKhronoBias);lua_settable06(LS, -3);


		lua_pushstring06(LS, "SetTimePass");lua_pushcfunction06(LS, KhronoControllerMeta__KhronoTimePass);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "SetTime");lua_pushcfunction06(LS, KhronoControllerMeta__KhronoSetTime);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "SetBias");lua_pushcfunction06(LS, KhronoControllerMeta__KhronoSetBias);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID


		lua_pop(LS,1);
	

		return 0;
	}


}