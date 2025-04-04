#include "LuaExtension_LoadReAttachArc.h"

#include <Sox/ResourceManager.h>
#include <Sox/IResourceMgr.h>
#include <Core/Debug.h>

#include <System/Singleton.h>
#include <System/CreateStatic.h>

#include <Sox/FileSystemARC.h>

namespace DebugLogV2{



	//arc
	extern "C" int LoadArc(lua_State* L){

		int args = lua_gettop(L);
		if (args <= 0) {lua_pushnil(L);return 1;}


		const char* File_Name = lua_tostring(L,1);
		int param_1 = 0;
		int param_2 = 1;
		if (args > 1) param_1 = lua_tonumber(L,2);
		if (args > 2) param_2 = lua_toboolean(L,3);


		Sonicteam::SoX::ResourceManager* ResourceManager = (Sonicteam::SoX::ResourceManager*)0x82D3B224;

		Sonicteam::SoX::IResourceMgr* ArcMgr = (Sonicteam::SoX::IResourceMgr*)(0x82D366D4);
		Sonicteam::SoX::IResource* ArcHandle =  ArcMgr->GetMgrResource(Sonicteam::SoX::IResourceMgrParam(param_1,param_2));
		ArcHandle->ResourceStr3 = std::string(File_Name);
		ArcHandle->ResourceLoad(0,0);
		ArcHandle->AddReference();


		lua_pushlightuserdata(L,ArcHandle);
		return 1;
	}
	extern "C" int UnloadArc(lua_State* L){
		Sonicteam::SoX::IResource* ArcHandle = (Sonicteam::SoX::IResource*)lua_touserdata(L,1);
		ArcHandle->Release();
		return 0;
	}
	extern "C" int AttachArc(lua_State* L){

		Sonicteam::SoX::IResource* ArcHandle = (Sonicteam::SoX::IResource*)lua_touserdata(L,1);
		Sonicteam::SoX::IResourceMgr* ArcMgr = (Sonicteam::SoX::IResourceMgr*)(0x82D366D4);
		Sonicteam::SoX::ResourceManager* ResourceManager = (Sonicteam::SoX::ResourceManager*)0x82D3B224;


		int index = -1;
		for (std::map<int,Sonicteam::SoX::HoldMGR>::iterator it = ResourceManager->ManagerResouceMgr.begin();it!=ResourceManager->ManagerResouceMgr.end();it++){
			if (it->second.Mgr == ArcMgr) {
				index = it->first;
				break;
			}
		}


		bool result = false;
		if (ResourceManager->ResourceRegistryHandle[index].find(ArcHandle->ResourceStr3) == ResourceManager->ResourceRegistryHandle[index].end()){
			ResourceManager->ResourceRegistryHandle[index][ArcHandle->ResourceStr3] =  REF_TYPE(Sonicteam::SoX::IResource)(ArcHandle);
			result = true;
		}



		lua_pushboolean(L,result);
		return 1;
	}

	extern "C" int DetachArc(lua_State* L){

		Sonicteam::SoX::IResource* ArcHandle = (Sonicteam::SoX::IResource*)lua_touserdata(L,1);
		Sonicteam::SoX::IResourceMgr* ArcMgr = (Sonicteam::SoX::IResourceMgr*)(0x82D366D4);
		Sonicteam::SoX::ResourceManager* ResourceManager = (Sonicteam::SoX::ResourceManager*)0x82D3B224;


		int index = -1;
		for (std::map<int,Sonicteam::SoX::HoldMGR>::iterator it = ResourceManager->ManagerResouceMgr.begin();it!=ResourceManager->ManagerResouceMgr.end();it++){
			if (it->second.Mgr == ArcMgr) {
				index = it->first;
				break;
			}
		}

		ResourceManager->ResourceRegistryHandle[index].erase(ResourceManager->ResourceRegistryHandle[index].find(ArcHandle->ResourceStr3));
	

		lua_pushboolean(L,true);
		return 1;
	}




	//resource


	struct FindArc{
		void* File;
		size_t FileSize;
		void Free(){
			if (File) delete File;
		}
	};

	extern "C" int FindInArc(std::string file_name){
		Sonicteam::SoX::ResourceManager* ResourceManager = (Sonicteam::SoX::ResourceManager*)0x82D3B224;
		Sonicteam::SoX::IResourceMgr* ArcMgr = (Sonicteam::SoX::IResourceMgr*)(0x82D366D4);
		int index = -1;
		for (std::map<int,Sonicteam::SoX::HoldMGR>::iterator it = ResourceManager->ManagerResouceMgr.begin();it!=ResourceManager->ManagerResouceMgr.end();it++){
			if (it->second.Mgr == ArcMgr) {
				index = it->first;
				break;
			}
		}
		/*

		for (std::map<std::string,REF_TYPE(Sonicteam::SoX::IResource)>::iterator it = ResourceManager->ManagerRegistryIndex[index].begin();ResourceManager->ManagerRegistryIndex[index].end();it++){
			
			void* Handle = (void*)it->second.param;
			void* HeaderTable1 = *(void**)((int)Handle + 0x74);
			void* HeaderTable2 = *(void**)((int)Handle + 0x78);
			int HeaderTableCount = *(int*)((int)Handle + 0x7C);
			void* StringTable = *(void**)((int)Handle + 0x80);

		

		}
	*/
		return 0;
	}


	extern "C" int LoadResource(lua_State* L){
		/*
		const char* ResouceMGR = lua_tostring(L,1);
		const char* ResouceName = lua_tostring(L,2);
		Sonicteam::SoX::ResourceManager* ResourceManager = (Sonicteam::SoX::ResourceManager*)0x82D3B224;

		if (ResourceManager->ResourceRegistryHandle.find(std::ResouceName) == ResourceManager->ResourceRegistryHandle.end()){
			lua_pushnil(L);
			return 1;
		}
		int Index = ResourceManager->ManagerRegistryIndex[ResouceName];
		Sonicteam::SoX::HoldMGR* HoldMGR =  &ResourceManager->ManagerResouceMgr[Index];
		Sonicteam::SoX::IResourceMgr* Mgr = HoldMGR->Mgr;
		Sonicteam::SoX::IResourceMgrParam mgr_param = Sonicteam::SoX::IResourceMgrParam(HoldMGR->Flag01,HoldMGR->Flag02); // default-param
		Sonicteam::SoX::IResource* Resource =  Mgr->GetMgrResource(mgr_param);
		Resource->ResourceStr3 = std::string(ResouceName);
		Resource->ResourceLoad();



		Resource->AddReference();
		lua_pushlightuserdata(L,Resource);
		*/
		return 1;
	}

	extern "C" GetDirectoryARC(lua_State* L){

		Sonicteam::SoX::FileSystemArc* arc =  &Sonicteam::SoX::FileSystemArc::getInstance();
		int count = lua_gettop(L);
		const char* arg1 = lua_tostring(L,1); //path
		const char* arg2 = lua_tostring(L,2); //ext
		std::vector<std::string> out;

		arc->FSDirectoryGetFiles(out,std::string(arg1),std::string(arg2));
		lua_newtable06(L);

		size_t count2 = 1;
		for (std::vector<std::string>::iterator it = out.begin();it!=out.end();it++){

			lua_pushstring06(L,it->c_str());
			lua_rawseti06(L,-2,count2++);
		}
		out.clear();
		return 1;
	}

	extern "C" GetPathARC(lua_State* L){


		Sonicteam::SoX::FileSystemArc* arc =  &Sonicteam::SoX::FileSystemArc::getInstance();
		const char* arg1 = lua_tostring(L,1); //path
		size_t index2 = lua_tonumber(L,2); //ext
		lua_pushstring06(L,arc->FSGetPath(std::string(arg1),index2).c_str());
		return 1;
	}

	extern "C" PathExistARC(lua_State* L){


		Sonicteam::SoX::FileSystemArc* arc =  &Sonicteam::SoX::FileSystemArc::getInstance();
		const char* arg1 = lua_tostring(L,1); //path
		lua_pushboolean(L,arc->FSPathExist(std::string(arg1)));
		return 1;
	}
	
	void GlobalInstall__LoadReAttachArc(lua_State* L)
	{
		if (L == 0){

			return;
		}

		lua_register06(L,"LoadArc",LoadArc);
		lua_register06(L,"UnloadArc",UnloadArc); 
		lua_register06(L,"AttachArc",AttachArc);
		lua_register06(L,"DetachArc",DetachArc);


		lua_register06(L,"GetDirectoryARC",GetDirectoryARC);
		lua_register06(L,"GetPathARC",GetPathARC);
		lua_register06(L,"PathExistARC",PathExistARC);



		lua_register06(L,"LoadResource",LoadResource);


	}

}