#pragma once

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}


#include <Prop/Scene.h>



namespace DebugLogV2 {



	// Actor related functions
	extern "C" int ACTOR__CREATEMETATABLE(lua_State* L, void* ptr);
	extern "C" int ACTOR__GETTYPE(lua_State* L);

	// PropMeta related functions
	extern "C" int PropMeta__METATABLE(lua_State* L, Sonicteam::Prop::SceneActor* prop);
	extern "C" int PropMeta__GETPROPINSTANCE(lua_State* L);
	extern "C" int PropMeta__GETACTOR(lua_State* L);
	extern "C" int PropMeta__GETFIXTURE(lua_State* L);

	// PropInstance related functions
	extern "C" int PROPINSTANCE__CREATEMETATABLE(lua_State* L, void* ptr);
	extern "C" int PROPINSTANCE__GETINSTANCESETDATA(lua_State* L);
	extern "C" int PROPINSTANCE__GETINSTANCECLASS(lua_State* L);

	// Fixture related functions
	extern "C" int FIXTURE__CREATEMETATABLE(lua_State* L, void* ptr);

	// InstanceSetData related functions
	extern "C" int INSTANCESETDATA__CREATEMETATABLE(lua_State* L, void* ptr);

	// InstanceClass related functions
	extern "C" int INSTANCECLASS__CREATEMETATABLE(lua_State* L, void* ptr);

	// Utility functions
	Sonicteam::Actor* GetObjX(std::vector<Sonicteam::Actor*>& _actor, std::vector<size_t>& _actorid);
	Sonicteam::Actor* GetObj(std::vector<Sonicteam::Prop::SceneActor*>& _actor, std::vector<std::string>& _name, std::string& obj_to_find);

	// ObjectIO
	extern "C" int GetSceneActorEx(lua_State* L);
	extern "C" int GetSceneActor(lua_State* L);
	extern "C" int GetActorALL(lua_State* L);




	void GlobalInstall_ObjectIO(lua_State* LS);



}