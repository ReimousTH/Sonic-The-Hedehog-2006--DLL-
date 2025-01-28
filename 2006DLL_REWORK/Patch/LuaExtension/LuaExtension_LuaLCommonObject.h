#pragma once

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}


#include "MessagesUtil.h"
#include <xtl.h>

#include <iostream>
#include <tchar.h>
#include <map>
#include <string>



#include <Player/Input/IListener.h>
#include <Player/State/IMachine.h>
#include <Player/State/CommonContext.h>
#include <Player/State/FastContext.h>
#include <boost/any.hpp>

#include <Player/State/CommonObject.h>
#include "LuaExtension_Memory.h"




#define _DWORD DWORD
#define _BYTE BYTE


namespace DebugLogV2 {




	struct LuaLCommonObjectSave{
		lua_State* LuaHandle;
		int LuaRefObject;

	public:
		LuaLCommonObjectSave(lua_State* LuaHandle,int LuaRefObject){
			this->LuaHandle = LuaHandle;
			this->LuaRefObject = LuaRefObject; 
		}
		LuaLCommonObjectSave(){}
	};

	struct LuaLCommonObjectSaveC{
		lua_State* LuaHandle;
		int LuaRefObject;
		int _type;
		bool ReplaceMode;

	public:
		LuaLCommonObjectSaveC(lua_State* LuaHandle,int LuaRefObject,int _type,bool ReplaceMode){
			this->LuaHandle = LuaHandle;
			this->LuaRefObject = LuaRefObject; 
			this->_type = _type;
			this->ReplaceMode = ReplaceMode;
		}
		LuaLCommonObjectSaveC(){}
	};


	static std::map<Sonicteam::Player::State::IMachine*,std::map<int,LuaLCommonObjectSave>> CachedStates;

	static std::map<Sonicteam::Player::State::IMachine*,std::map<int,std::map<int,std::vector<LuaLCommonObjectSaveC>>>> CachedStatesConnect;


	//TODO, refind in game constructor and make modification to atleast StoreStateID as second argument, to be able at least detect state info
	class LuaLCommonObject:Sonicteam::Player::State::CommonObject{

	public:
		lua_State* L;
		int ref;

		static lua_State* LastLState;
		LuaLCommonObject(Sonicteam::Player::State::IMachine* machine,int StateID):CommonObject(machine){
			if (CachedStates.find(machine) ==  CachedStates.end()) return;
			L = CachedStates[machine][StateID].LuaHandle;
			ref = CachedStates[machine][StateID].LuaRefObject;


		}
		~LuaLCommonObject(){

		}


		virtual void Object2Start() override
		{


			lua_rawgeti(L,LUA_REGISTRYINDEX,ref); //self.

			lua_pushstring06(L,"OnStart");
			lua_gettable(L,-2);


			lua_getglobal06(L,"CommonObjectRef");
			lua_pushlightuserdata(L,(void*)this->ObjectMashine);
			lua_pushlightuserdata(L,(void*)this->CObjContext);
			lua_pushlightuserdata(L,(void*)0);
			lua_pcall06(L,3,1,0);

			lua_pcall06(L,1,0,0);



			//	lua_pushnumber(BaseLua.L,Context);
			//	lua_pushnumber(BaseLua.L,a2);
			//	lua_pushnumber(BaseLua.L,Player-0x20);
			//	lua_pcall(BaseLua.L,3,0,0);

		}

		virtual unsigned int Object2Update(float) override
		{
			int result = 0;

			return result;

		}

		virtual void Object2End() override
		{

		}

		virtual void Object2PreUpdateCmn(float) override
		{

		}

		virtual void Object2PostUpdateCmn(float) override
		{

		}

		virtual void DestroyObject(unsigned int flag) override
		{
			LuaLCommonObject::~LuaLCommonObject();
			Sonicteam::SoX::Memory::IUDestructible::DestroyObject(this,flag);
		}

	};







	

	typedef boost::shared_ptr<LuaLCommonObject> (*StateConstructorEX)(Sonicteam::Player::State::IMachine*,int StateID);

	typedef boost::shared_ptr<LuaLCommonObject> CustomSharedPtr;
	static boost::shared_ptr<LuaLCommonObject> LuaLCommonObjectBoost(Sonicteam::Player::State::IMachine* machine,int StateID) {
		
		
		return CustomSharedPtr(new LuaLCommonObject(machine,StateID),Sonicteam::SoX::Memory::BoostDestructorHelper<LuaLCommonObject>());
	
	}
	
	void GlobalInstall_LuaLCommonObject(lua_State* LS);


	extern "C" int LuaLCommonObject__NEW(lua_State* L);

	extern "C" int LuaLCommonObject__GetIMachine(lua_State*);



	


}