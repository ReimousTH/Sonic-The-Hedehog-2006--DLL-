#include "LuaExtension_ObjectIO.h"
#include "LuaExtension_Misc.h"

#include <Sox/Scenery/World.h>
#include <Sox/Scenery/SPAabbTree.h>
#include <Sox/Scenery/SPWorld.h>


namespace DebugLogV2{


	//////////////////////////////////////////////////////////////////////////
	Sonicteam::Actor* GetObjX(std::vector<Sonicteam::Actor*>& _actor,std::vector<size_t>& _actorid) {

		Sonicteam::GameImp* impl = Misc::GetGameIMP();
		Sonicteam::ActorManager* mgr = impl->GetActorManager().get();

		for (int i = 0;i<mgr->LastActorIndex;i++){
			_actor.push_back(mgr->Actor[i]);
			_actorid.push_back(mgr->ActorID[i]);
		}


		return NULL; // nullptr if no match found
	}

	Sonicteam::Actor* GetObj(std::vector<Sonicteam::Prop::SceneActor*>& _actor, std::vector<std::string>& _name, std::string& obj_to_find) {
		Sonicteam::GameImp* impl = Misc::GetGameIMP();
		Sonicteam::Prop::Manager* mgr = impl->GamePropManager.get();
		Sonicteam::Actor* first_found = nullptr;

		for (LinkSoxNode<Sonicteam::Prop::Scene>::iterator* it = mgr->SceneLink.Next; 
			it != &mgr->SceneLink; 
			it = it->Next) 
		{
			Sonicteam::Prop::Scene* _scene = it->_this;

			// Explicit iterator for std::map<std::string, int>
			std::map<std::string, int>::iterator map_it;
			for (map_it = _scene->ScenePlacament.begin(); map_it != _scene->ScenePlacament.end(); ++map_it) {
				const std::string& name = map_it->first;
				int id = map_it->second;

				// Check if name starts with obj_to_find
				if (name.rfind(obj_to_find, 0) == 0) {
					Sonicteam::Prop::SceneActor* sactor = &_scene->SceneObject[id];
					_actor.push_back(sactor);
					_name.push_back(name);

				}

			}
		}
		return NULL; // nullptr if no match found
	}
	//////////////////////////////////////////////////////////////////////////




	//////////////////////////////////////////////////////////////////////////
	extern int "C" ACTOR__CREATEMETATABLE(lua_State* L,void* ptr){

		lua_getglobal(L,"Memory");
		lua_pushlightuserdata(L,ptr);
		lua_pcall06(L,1,1,0);
		luaL_getmetatable06(L, "ActorMeta");
		lua_setmetatable06(L,-2);
		return 1;
	}
	extern int "C" ACTOR__GETTYPE(lua_State* L){

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Actor* sactor =  (Sonicteam::Actor* )lua_touserdata06(L,-1);
		lua_pushstring06(L,sactor->GetObjectType()); //yay
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////




	//////////////////////////////////////////////////////////////////////////
	extern "C" int PropMeta__METATABLE(lua_State* L,Sonicteam::Prop::SceneActor* prop){

		lua_getglobal(L,"Memory");
		lua_pushlightuserdata(L,prop);
		lua_pcall06(L,1,1,0);

		luaL_getmetatable06(L, "SceneActorMeta");
		lua_setmetatable06(L,-2);

		return 1;
	}


	extern "C" int PropMeta__GETPROPINSTANCE(lua_State* L){

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Prop::SceneActor* sactor =  (Sonicteam::Prop::SceneActor* )lua_touserdata06(L,-1);
		PROPINSTANCE__CREATEMETATABLE(L,sactor->ObjActorHandle->PropInstance.get());
		return 1;
	}

	extern "C" int PropMeta__GETACTOR(lua_State* L){

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Prop::SceneActor* sactor =  (Sonicteam::Prop::SceneActor* )lua_touserdata06(L,-1);
		ACTOR__CREATEMETATABLE(L,sactor->ObjActor);


		return 1;
	}

	extern "C" int PropMeta__GETFIXTURE(lua_State* L){

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Prop::SceneActor* sactor =  (Sonicteam::Prop::SceneActor* )lua_touserdata06(L,-1);
		FIXTURE__CREATEMETATABLE(L,sactor->ObjActor);
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	extern "C" int PROPINSTANCE__CREATEMETATABLE(lua_State* L,void* ptr){

		lua_getglobal(L,"Memory");
		lua_pushlightuserdata(L,ptr);
		lua_pcall06(L,1,1,0);

		luaL_getmetatable06(L, "PropInstanceMeta");
		lua_setmetatable06(L,-2);


		return 1;
	}

	extern "C" int PROPINSTANCE__GETINSTANCESETDATA(lua_State* L){

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Prop::Instance* instance =  (Sonicteam::Prop::Instance*)lua_touserdata06(L,-1);
		INSTANCESETDATA__CREATEMETATABLE(L,instance->InstanceSetData);
		return 1;
	}
	extern "C" int PROPINSTANCE__GETINSTANCECLASS(lua_State* L){

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Prop::Instance* instance =  (Sonicteam::Prop::Instance*)lua_touserdata06(L,-1);
		INSTANCECLASS__CREATEMETATABLE(L,(void*)instance->InstanceClass.get());
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////




	//////////////////////////////////////////////////////////////////////////
	extern "C" int FIXTURE__CREATEMETATABLE(lua_State* L,void* ptr){

		lua_getglobal(L,"Memory");
		lua_pushlightuserdata(L,ptr);
		lua_pcall06(L,1,1,0);

		luaL_getmetatable06(L, "FixtureMeta");
		lua_setmetatable06(L,-2);


		return 1;
	}
	//////////////////////////////////////////////////////////////////////////




	//////////////////////////////////////////////////////////////////////////
	extern "C" int INSTANCESETDATA__CREATEMETATABLE(lua_State* L,void* ptr){

		lua_getglobal(L,"Memory");
		lua_pushlightuserdata(L,ptr);
		lua_pcall06(L,1,1,0);

		luaL_getmetatable06(L, "InstanceSetDataMeta");
		lua_setmetatable06(L,-2);


		return 1;
	}
	//////////////////////////////////////////////////////////////////////////




	//////////////////////////////////////////////////////////////////////////
	extern "C" int INSTANCECLASS__CREATEMETATABLE(lua_State* L,void* ptr){

		lua_getglobal(L,"Memory");
		lua_pushlightuserdata(L,ptr);
		lua_pcall06(L,1,1,0);

		luaL_getmetatable06(L, "InstanceClassMeta");
		lua_setmetatable06(L,-2);


		return 1;
	}

	extern "C" int INSTANCECLASS__GETPROPDATA(lua_State* L){

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Prop::Class* cls =  (Sonicteam::Prop::Class* )lua_touserdata06(L,-1);

		
	
		Memory__CreateMetatable(L,(int)cls->ClassPropData,0);
		return 1;
	}


	//////////////////////////////////////////////////////////////////////////


	extern "C" int GetSceneActorEx(lua_State* L){


		const char* _obj = lua_tostring(L,1);

		std::vector<Sonicteam::Prop::SceneActor*> _actor;
		std::vector<std::string> _actor_name;
		GetObj(_actor,_actor_name,std::string(_obj));



		lua_newtable06(L);

		size_t table_index = 1;
		std::vector<std::string>::iterator name_it = _actor_name.begin();
		for(std::vector<Sonicteam::Prop::SceneActor*>::iterator it = _actor.begin(); 
			it != _actor.end() && name_it != _actor_name.end(); 
			++it, ++name_it) 
		{
			lua_pushstring(L, name_it->c_str());

			PropMeta__METATABLE(L, *it);

			// Store key-value pair
			lua_rawset(L, -3);  // -3 = table position
		}




		return 1;
	} 

	extern "C" int GetSceneActor(lua_State* L){


		const char* _obj = lua_tostring(L,1);

		std::vector<Sonicteam::Prop::SceneActor*> _actor;
		std::vector<std::string> _actor_name;
		GetObj(_actor,_actor_name,std::string(_obj));

		lua_newtable06(L);

		size_t table_index = 1;
		std::vector<std::string>::iterator name_it = _actor_name.begin();
		for(std::vector<Sonicteam::Prop::SceneActor*>::iterator it = _actor.begin(); 
			it != _actor.end() && name_it != _actor_name.end(); 
			++it, ++name_it) 
		{
			if ((*it)->ObjActor == 0) continue;
			lua_pushstring(L, name_it->c_str());
			PropMeta__METATABLE(L, *it);
			lua_rawset(L, -3);  // -3 = table position
		}


		return 1;
	} 


	extern "C" int GetActorALL(lua_State* L){



		std::vector<Sonicteam::Actor*> _actor;
		std::vector<std::size_t> _actor_name;
		GetObjX(_actor,_actor_name);


		lua_newtable06(L);

		size_t table_index = 1;

		std::vector<size_t>::iterator name_it = _actor_name.begin();

		for(std::vector<Sonicteam::Actor*>::iterator it = _actor.begin(); 
			it != _actor.end() && name_it != _actor_name.end(); 
			++it, ++name_it) 
		{
			lua_pushnumber(L, *name_it);
			ACTOR__CREATEMETATABLE(L, *it);
			lua_rawset(L, -3);  // -3 = table position
		}




		return 1;

	}


	//////////////////////////////////////////////////////////////////////////


	extern "C" int ReloadScene(lua_State* L){



		Sonicteam::GameImp* impl = Misc::GetGameIMP();
		Sonicteam::Prop::Manager* mgr = impl->GamePropManager.get();




		for (int i = 0;i<6;i++){
			Sonicteam::Prop::Scene* _scene = impl->GamePropScene[i].get();
			if (!_scene) continue;

			Sonicteam::SoX::Scenery::SPWorld *  spworld =  _scene->PropSceneWorld.get();
			std::vector<Sonicteam::SoX::Scenery::SPAabbNodeVector> _dummy_;
			_dummy_.push_back(Sonicteam::SoX::Scenery::SPAabbNodeVector(0,0,0,0,0,0,0,0));
			spworld->SceneryAabbTree.get()->SPAabbTreeInitialize(spworld,&_dummy_[0],1); //Reset Render Blocks (despawns all objects)
			_scene->SceneObject.clear(); 
			_scene->ScenePlacament.clear();
			_scene->ScenePlacamentGroup.clear();
			//_scene->ScenePropInstance.clear();
			_scene->ScenePropData = 0;

		}




		Sonicteam::ActorManager* actor_manager = impl->GameActorManager.get();

		for (int i = 0;i<actor_manager->LastActorIndex;i++){
			Sonicteam::Actor* actor = (actor_manager->Actor[i]);
			if (actor == NULL) continue;
			const char* actor_t =  actor->GetObjectType();

			bool is_forced_actor = strcmp(actor_t,"class Sonicteam::Player::Object") == 0 ||
				strcmp(actor_t,"DependGame") == 0 ||
				strcmp(actor_t,"Entities")  == 0 ||
				strcmp(actor_t,"EnemyThread")  == 0 ||
				strcmp(actor_t,"Players")  == 0 ||
				strcmp(actor_t,"Cameras")  == 0 ||
				strcmp(actor_t,"DependCameras")  == 0 ||
				strcmp(actor_t,"SystemDependCameras")  == 0 ||
				strcmp(actor_t,"GameRootTask")  == 0 ||
				strcmp(actor_t,"Cameraman")  == 0;

			if (!is_forced_actor){
				AddMessage("DLL[CLEAR] : %s",actor_t);
				actor->DestroyObject(1);
			}	
		}
		//actor_manager->LastActorIndex = 0;
		//actor_manager->LastActorID = 0;
		//impl->GameActorManager.get()


		for (int i = 0;i<6;i++){
			//impl->GamePropScene[i].release(); //release pointer 
			//impl->GameImpSceneryWorlds[i].release();
			
		}




		return 0;
	}


	void GlobalInstall_ObjectIO(lua_State* LS)
	{

		lua_register06(LS,"GetSceneActorEx", GetSceneActorEx);
		lua_register06(LS,"GetSceneActor", GetSceneActor);
		lua_register06(LS,"GetActorALL", GetActorALL);

		lua_register06(LS,"ReloadScene", ReloadScene);

		luaL_newmetatable06(LS, "SceneActorMeta");
		lua_pushstring06(LS,"__index"); lua_pushvalue(LS,-2); lua_settable06(LS,-3); // __index = PlayerMeta
		lua_newtable06(LS);lua_pushstring06(LS, "__index");luaL_getmetatable06(LS, "MemoryMeta");lua_settable06(LS, -3); lua_setmetatable06(LS, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })
		

		lua_pushstring06(LS, "GetActor");lua_pushcfunction06(LS, PropMeta__GETACTOR);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "GetFixture");lua_pushcfunction06(LS, PropMeta__GETFIXTURE);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "GetInstance");lua_pushcfunction06(LS, PropMeta__GETPROPINSTANCE);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		
		
		lua_pop(LS,1);



		luaL_newmetatable06(LS, "FixtureMeta");
		lua_pushstring06(LS,"__index"); lua_pushvalue(LS,-2); lua_settable06(LS,-3); // __index = PlayerMeta
		lua_newtable06(LS);lua_pushstring06(LS, "__index");luaL_getmetatable06(LS, "MemoryMeta");lua_settable06(LS, -3); lua_setmetatable06(LS, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })
		lua_pushstring06(LS, "GetType");lua_pushcfunction06(LS, ACTOR__GETTYPE);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pop(LS,1);


		luaL_newmetatable06(LS, "ActorMeta");
		lua_pushstring06(LS,"__index"); lua_pushvalue(LS,-2); lua_settable06(LS,-3); // __index = PlayerMeta
		lua_newtable06(LS);lua_pushstring06(LS, "__index");luaL_getmetatable06(LS, "MemoryMeta");lua_settable06(LS, -3); lua_setmetatable06(LS, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })
		lua_pushstring06(LS, "GetType");lua_pushcfunction06(LS, ACTOR__GETTYPE);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pop(LS,1);

		luaL_newmetatable06(LS, "PropInstanceMeta");
		lua_pushstring06(LS,"__index"); lua_pushvalue(LS,-2); lua_settable06(LS,-3); // __index = PlayerMeta
		lua_newtable06(LS);lua_pushstring06(LS, "__index");luaL_getmetatable06(LS, "MemoryMeta");lua_settable06(LS, -3); lua_setmetatable06(LS, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })
		lua_pushstring06(LS, "GetInstanceSetData");lua_pushcfunction06(LS, PROPINSTANCE__GETINSTANCESETDATA);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "GetInstanceClass");lua_pushcfunction06(LS, PROPINSTANCE__GETINSTANCECLASS);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pop(LS,1);


		luaL_newmetatable06(LS, "InstanceSetDataMeta");
		lua_pushstring06(LS,"__index"); lua_pushvalue(LS,-2); lua_settable06(LS,-3); // __index = PlayerMeta
		lua_newtable06(LS);lua_pushstring06(LS, "__index");luaL_getmetatable06(LS, "MemoryMeta");lua_settable06(LS, -3); lua_setmetatable06(LS, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })
		//lua_pushstring06(LS, "GetInstanceSetData");lua_pushcfunction06(LS, PROPINSTANCE__GETINSTANCESETDATA);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pop(LS,1);


		luaL_newmetatable06(LS, "InstanceClassMeta");
		lua_pushstring06(LS,"__index"); lua_pushvalue(LS,-2); lua_settable06(LS,-3); // __index = PlayerMeta
		lua_newtable06(LS);lua_pushstring06(LS, "__index");luaL_getmetatable06(LS, "MemoryMeta");lua_settable06(LS, -3); lua_setmetatable06(LS, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })
		lua_pushstring06(LS, "GetPropData");lua_pushcfunction06(LS, INSTANCECLASS__GETPROPDATA);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pop(LS,1);


		








		return;
	

	}


}
