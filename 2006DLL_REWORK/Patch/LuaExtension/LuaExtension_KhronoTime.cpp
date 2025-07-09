#include "LuaExtension_KhronoTime.h"

#include <boost/function.hpp>



#include "LuaExtension_Misc.h"
#include <Core/Debug.h>

#include <GameImp.h>
#include <KhronoController.h>
#include <KhronoControlInputListener.h>
#include <SceneParamManager.h>
#include <ISceneParam.h>
#include <SceneLightParam.h>
#include <NamedActor.h>
#include <SceneOLSParam.h>
#include <SceneKhronosParam.h>
#include <xtl.h>

#include <LuaSystem.h>
#include <Sox/FileLoaderARC.h>
#include <Sox/FileHandleARC.h>
#include <Sox/IResource.h>
#include <Sox/FileSystemARC.h>

#define _USE_MATH_DEFINES
#include <math.h>



namespace DebugLogV2{



	extern "C" KhronoControllerMeta__NEW(lua_State* L,void* ptr){

		lua_getglobal(L,"Memory");
		lua_pushlightuserdata(L,ptr);
		lua_pcall06(L,1,1,0);
		luaL_getmetatable06(L, "KhronoControllerMeta");
		lua_setmetatable06(L,-2);
		return 1;
	}

	extern "C" SceneParamManagerMeta__NEW(lua_State* L,void* ptr){

		lua_getglobal(L,"Memory");
		lua_pushlightuserdata(L,ptr);
		lua_pcall06(L,1,1,0);
		luaL_getmetatable06(L, "SceneParamManagerMeta");
		lua_setmetatable06(L,-2);
		return 1;
	}


	extern "C" SceneIParamMeta__NEW(lua_State* L,void* ptr){

		lua_getglobal(L,"Memory");
		lua_pushlightuserdata(L,ptr);
		lua_pcall06(L,1,1,0);
		luaL_getmetatable06(L, "SceneIParamMeta");
		lua_setmetatable06(L,-2);
		return 1;
	}

	extern "C" SceneLightParamMeta__NEW(lua_State* L,void* ptr){

		lua_getglobal(L,"Memory");
		lua_pushlightuserdata(L,ptr);
		lua_pcall06(L,1,1,0);

		luaL_getmetatable06(L, "SceneLightParamMeta");
		lua_setmetatable06(L,-2);
		return 1;
	}


	extern "C" SceneLightParamMeta__GetDirectionalLightNI(lua_State* L){

		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::SceneLightParam* c =  (Sonicteam::SceneLightParam*)lua_touserdata(L,-1);lua_pop(L,1);

		const char* param_name = "Main";
		if (lua_isstring(L,2)){
			param_name = lua_tostring(L,2);	
		}
		Sonicteam::SceneLightParam::DirectionalLight* dlight =  c->MainSub[param_name].get();
		Misc::MemoryMetaNew(L,dlight);

		return 1;
	}

	extern "C" SceneLightParamMeta__GetDirectionalLightMain(lua_State* L){

		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::SceneLightParam* c =  (Sonicteam::SceneLightParam*)lua_touserdata(L,-1);lua_pop(L,1);
		Sonicteam::SceneLightParam::DirectionalLight* dlight =  c->_DirectionalLight.get();
		Misc::MemoryMetaNew(L,dlight);
		return 1;
	}
	extern "C" SceneLightParamMeta__GetDirectionalLightSub(lua_State* L){

		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::SceneLightParam* c =  (Sonicteam::SceneLightParam*)lua_touserdata(L,-1);lua_pop(L,1);
		Sonicteam::SceneLightParam::DirectionalLight* dlight =  c->_DirectionalLightSub.get();
		Misc::MemoryMetaNew(L,dlight);
		return 1;
	}

	extern "C" SceneLightParamMeta__GetAmbientLightNI(lua_State* L){

		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::SceneLightParam* c =  (Sonicteam::SceneLightParam*)lua_touserdata(L,-1);lua_pop(L,1);
		const char* param_name = "Global";
		if (lua_isstring(L,2)){param_name = lua_tostring(L,2);}
		Sonicteam::SceneLightParam::AmbientLight* dlight =  c->Global[param_name].get();
		Misc::MemoryMetaNew(L,dlight);
		return 1;
	}

	extern "C" SceneLightParamMeta__GetAmbientLight(lua_State* L){

		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::SceneLightParam* c =  (Sonicteam::SceneLightParam*)lua_touserdata(L,-1);lua_pop(L,1);
		const char* param_name = "Global";
		if (lua_isstring(L,2)){param_name = lua_tostring(L,2);}
		Sonicteam::SceneLightParam::AmbientLight* dlight =  c->_AmbientLight.get();
		Misc::MemoryMetaNew(L,dlight);
		return 1;
	}

	extern "C" SceneLightParamMeta__GetCurrentDirectionalLight(lua_State* L){
		lua_pushstring(L, "ptr");
		lua_gettable(L, 1);
		Sonicteam::SceneLightParam* c = (Sonicteam::SceneLightParam*)lua_touserdata(L, -1);
		lua_pop(L, 1);

		int index_1 = 0;
		int index_2 = 0;
		if (lua_isnumber(L, 2)) { index_1 = lua_tonumber(L, 2); }
		if (lua_isnumber(L, 3)) { index_2 = lua_tonumber(L, 3); }

		Sonicteam::SceneLightParam::CurrentDirectionalLight* dlight = NULL;
		if (index_1 < c->_CurrentDirectionalLight.size() && 
			index_2 < c->_CurrentDirectionalLight[index_1].size()) {    
				dlight = c->_CurrentDirectionalLight[index_1][index_2].get();
		}

		Misc::MemoryMetaNew(L, dlight);
		return 1;

	}
	extern "C" SceneLightParamMeta__GetCurrentAmbientLight(lua_State* L){

		lua_pushstring(L, "ptr");
		lua_gettable(L, 1);
		Sonicteam::SceneLightParam* c = (Sonicteam::SceneLightParam*)lua_touserdata(L, -1);
		lua_pop(L, 1);
		const char* param_name = "Global";

		int index_1 = 0;
		if (lua_isnumber(L, 2)) { index_1 = lua_tonumber(L, 2); }

		Sonicteam::SceneLightParam::CurrentAmbientLight* dlight = NULL;
		if (index_1 < c->_CurrentAmbientLight.size()) {  // Changed from <= to <
			dlight = c->_CurrentAmbientLight[index_1].get();
		}

		Misc::MemoryMetaNew(L, dlight);
		return 1;
	}



	extern "C" SceneParamManagerMeta__GetSceneIParam(lua_State* L){

		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::SceneParamManager* c =  (Sonicteam::SceneParamManager*)lua_touserdata(L,-1);lua_pop(L,1);
		const char* param_name = lua_tostring(L,2);

		if (c->SceneParams.find(param_name) == c->SceneParams.end()){
			SceneIParamMeta__NEW(L,0);
		}
		else{

			if (strcmp(param_name,"Light") == 0){
				SceneLightParamMeta__NEW(L,c->SceneParams[param_name].get());
			}
			else{
				SceneIParamMeta__NEW(L,c->SceneParams[param_name].get());
			}
		}
		return 1;
	}

	extern "C" SceneParamManagerMeta__GetName(lua_State* L){

		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::ISceneParam* c =  (Sonicteam::ISceneParam*)lua_touserdata(L,-1);lua_pop(L,1);
		lua_pushstring(L,c->GetParamName());
		return 1;
	}

	
	
	



	extern "C" SceneParamManagerMeta__Initialize(lua_State* L){

		lua_pushstring(L, "ptr");
		lua_gettable(L, 1);
		Sonicteam::ISceneParam* c = (Sonicteam::ISceneParam*)lua_touserdata(L, -1);
		lua_pop(L, 1);

		// Initialize arguments
		Sonicteam::ISPApplySPArg init_args;
		memset(&init_args, 0, sizeof(init_args));

		// Get implementation
		Sonicteam::DocMarathonImp* impl = *(Sonicteam::DocMarathonImp**)(*(DWORD*)0x82D3B348 + 0x180);

		// Get resource manager
		Sonicteam::SoX::ResourceManager* ResourceManager = (Sonicteam::SoX::ResourceManager*)0x82D3B224;


		Sonicteam::SoX::ResourceManager::LoadResourceFromArc(&Sonicteam::LuaSystemManager::getInstanceQuick(),lua_tostring(L,2),lua_tostring(L,2),0,0);
		//ResourceManager->LoadResource(&Sonicteam::LuaSystemManager::getInstanceQuick(),"test",0,0);


		/*
		int index = -1;
		ResourceManager->GetResourceHandle(index,std::string("test"));

		// Get Lua resource
		Sonicteam::SoX::IResource* lua_file_iresource = 
			Sonicteam::LuaSystemManager::getInstanceQuick().GetMgrResource(NULL);

	



		// Bind using the wrapper
		IFileHandle_BFUNCTION_TYPE func = 
			boost::bind(&Sonicteam::SoX::IResource::ResourceLoadFinal,lua_file_iresource,_1,_2);

		// Get filename
		const char* file = ".lua";
		if (lua_isstring(L, 2))
			file = lua_tostring(L, 2);

		// Set resource string - UNSAFE direct assignment
		lua_file_iresource->ResourceStr3 = file;
		std::string file_path =  lua_file_iresource->GetResourceName(file);



		// Load file
		 REF_TYPE(Sonicteam::SoX::IFileHandle) handle = 
			Sonicteam::SoX::FileLoaderARC::getInstanceQuick().LoadFile(
			file_path, 
			func);


		 handle.get()->LoadHandle();
		 */
	

		//PushBreakPoint(__FILE__,__LINE__,"%x",handle.get());
		/*

		init_args.scene_param_manager = impl->DocGetSceneParamManager();
		init_args.doc = impl;
		init_args.scene_file_lua 
		init_args.game_mode = 0;

		*/
		//c->InitializeSceneParam(0);
		return 0;
	}
	extern "C" SceneParamManagerMeta__Update(lua_State* L){

		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::ISceneParam* c =  (Sonicteam::ISceneParam*)lua_touserdata(L,-1);lua_pop(L,1);

		double tick = 1.0;
		if (lua_isnumber(L,2)){
			tick = lua_tonumber(L,2);
		}
		c->SceneParamUpdate(tick);
		return 0;
	}
	extern "C" SceneParamManagerMeta__Apply(lua_State* L){

		lua_pushstring06(L,"ptr"); 	lua_gettable(L,1);
		Sonicteam::ISceneParam* c =  (Sonicteam::ISceneParam*)lua_touserdata(L,-1);lua_pop(L,1);


		ISPApplyArg1 arg1;
		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);

		arg1.doc = impl;
		arg1.device = (Sonicteam::SoX::Graphics::Device*)impl->DocMyGraphicDevice;

		c->ApplySceneParam(arg1);
		return 0;
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
		int mode = lua_tonumber(L,2); //0 - Auto, 1 - Manual
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
		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		Sonicteam::SoX::Graphics::Device* graph = (Sonicteam::SoX::Graphics::Device*)impl->DocGetMyGraphicDevice();


		Sonicteam::ISceneParam* bloom =  impl->DocGetSceneParamManager()->SceneParams["Bloom"].get();
		Sonicteam::SceneParamManager* param_mgr = impl->DocGetSceneParamManager();
		Sonicteam::SceneOLSParam* ols = static_cast<Sonicteam::SceneOLSParam*>(param_mgr->SceneParams["OLS"].get());
		Sonicteam::SceneKhronosParam* khronos = static_cast<Sonicteam::SceneKhronosParam*>(param_mgr->SceneParams["Khronos"].get());



	
		// Set core time values
		c->khrono_time = lua_tonumber(L,2);
		c->khrono_time_pass = 0.0;
		c->khrono_game_time = (c->khrono_time / 24.0f) * 256.0f;

		// Update khronos parameters
		khronos->khrono_prm_normalized_time = c->khrono_time * (1.0f / 24.0f);
		khronos->khrono_prm_time = c->khrono_game_time;

		// Calculate sun position angles
		float azimuth_deg = (c->khrono_time - 6.0f) * 15.0f;
		if (azimuth_deg < 0.0f) 
			azimuth_deg += 360.0f;

		float angles[2] = {
			-azimuth_deg * (M_PI / 180.0f),  // Azimuth in radians (negated)
			-ols->unk18                  // Current elevation (negated)
		};

	


	
	
		// CORRECTED function typedef based on disassembly
		typedef int* (__fastcall *UpdateLightingFn)(
			double timer,          // f1
			void* graph,// r4
			void* bloom, // r5
			void* ols1, // r6
			float* angles          // r7
			);

		// Get the function address
		UpdateLightingFn updateLighting = (UpdateLightingFn)0x822703A0;

		// CORRECTED call with proper register mapping
		int* result = updateLighting(
			c->khrono_time,         // f1
			graph,         // r4
			bloom,         // r5
			ols,           // r6 (same as SceneOLSParam in this context)
			angles         // r7
			);

	

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
	extern "C" GetSceneParamManager(lua_State* L){

		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		SceneParamManagerMeta__NEW(L,impl->DocGetSceneParamManager());
		return 1;
	}
	


	int KhronoTime_GlobalInstall(lua_State* LS)
	{



	
		lua_register06(LS,"GetKhronoController", GetKhronoController);
		lua_register06(LS,"GetSceneParamManager", GetSceneParamManager);



		luaL_newmetatable06(LS, "SceneParamManagerMeta");
		lua_pushstring06(LS,"__index"); lua_pushvalue(LS,-2); lua_settable06(LS,-3); // __index = PlayerMeta
		lua_newtable06(LS);lua_pushstring06(LS, "__index");luaL_getmetatable06(LS, "MemoryMeta");lua_settable06(LS, -3); lua_setmetatable06(LS, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })
		lua_pushstring06(LS, "GetSceneParam");lua_pushcfunction06(LS, SceneParamManagerMeta__GetSceneIParam);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pop(LS,1);


		luaL_newmetatable06(LS, "SceneIParamMeta");
		lua_pushstring06(LS,"__index"); lua_pushvalue(LS,-2); lua_settable06(LS,-3); // __index = PlayerMeta
		lua_newtable06(LS);lua_pushstring06(LS, "__index");luaL_getmetatable06(LS, "MemoryMeta");lua_settable06(LS, -3); lua_setmetatable06(LS, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })
		lua_pushstring06(LS, "GetName");lua_pushcfunction06(LS, SceneParamManagerMeta__GetName);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "Initialize");lua_pushcfunction06(LS, SceneParamManagerMeta__Initialize);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "Update");lua_pushcfunction06(LS, SceneParamManagerMeta__Update);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "ApplyToScene");lua_pushcfunction06(LS, SceneParamManagerMeta__Apply);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pop(LS,1);


		luaL_newmetatable06(LS, "SceneLightParamMeta");
		lua_pushstring06(LS,"__index"); lua_pushvalue(LS,-2); lua_settable06(LS,-3); // __index = PlayerMeta
		lua_newtable06(LS);lua_pushstring06(LS, "__index");luaL_getmetatable06(LS, "SceneIParamMeta");lua_settable06(LS, -3); lua_setmetatable06(LS, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })
		lua_pushstring06(LS, "GetDirectionalLightNI");lua_pushcfunction06(LS, SceneLightParamMeta__GetDirectionalLightNI);lua_settable06(LS, -3); 
		lua_pushstring06(LS, "GetAmbientLightNI");lua_pushcfunction06(LS, SceneLightParamMeta__GetAmbientLightNI);lua_settable06(LS, -3); 
		lua_pushstring06(LS, "GetAmbientLight");lua_pushcfunction06(LS, SceneLightParamMeta__GetAmbientLight);lua_settable06(LS, -3); 
		lua_pushstring06(LS, "GetDirectionalLightMain");lua_pushcfunction06(LS, SceneLightParamMeta__GetDirectionalLightMain);lua_settable06(LS, -3); 
		lua_pushstring06(LS, "GetDirectionalLightSub");lua_pushcfunction06(LS, SceneLightParamMeta__GetDirectionalLightSub);lua_settable06(LS, -3); 
		lua_pushstring06(LS, "GetCurrentDirectionalLight");lua_pushcfunction06(LS, SceneLightParamMeta__GetCurrentDirectionalLight);lua_settable06(LS, -3); 
		lua_pushstring06(LS, "GetCurrentAmbientLight");lua_pushcfunction06(LS, SceneLightParamMeta__GetCurrentAmbientLight);lua_settable06(LS, -3); 
	
		lua_pop(LS,1);


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