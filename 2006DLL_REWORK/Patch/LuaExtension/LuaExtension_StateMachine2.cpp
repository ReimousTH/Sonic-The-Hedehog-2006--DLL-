#include "LuaExtension_StateMachine2.h"



#define SM2LIB_NAME_META "StateIMachineMeta"
#define SM2LIB_NAME "StateIMachine"


#include <Player/State/IMachine.h>
#include <Player/State/IContext.h>
#include <boost/weak_ptr.hpp>

#define SM2_OnStateStart 1
#define SM2_OnStateUpdate 2
#define SM2_OnStateEnd 3

#define SM2_OnStateStartP 4
#define SM2_OnStateUpdateP 5
#define SM2_OnStateEndP 6



namespace DebugLogV2{



	bool ProcessStateLua(Sonicteam::Player::State::IMachine *a1, double a2,int ID,int _type){

		bool result  = false;
		Sonicteam::Player::State::Machine2* _this = dynamic_cast<Sonicteam::Player::State::Machine2*>(a1);
		Sonicteam::Player::State::IContext* _context =   _this->MashineContext.get();

		if ((CachedStatesConnect.find(a1) != CachedStatesConnect.end()) && (CachedStatesConnect[a1].find(ID) != CachedStatesConnect[a1].end()) )
		{
			void* StatePointerARG0 = (void*)_this->CurrentState.get();
			void* StateIMachineARG1 = (void*)a1;
			void* ContextARG2 = (void*)_this->GetMashineContext().get();
			void* FieldsARG3 = 0;


			std::vector<LuaLCommonObjectSaveC> StatesConnect = CachedStatesConnect[a1][ID][_type];
			for (std::vector<LuaLCommonObjectSaveC>::iterator it = StatesConnect.begin();it != StatesConnect.end();it++){
				//ShowXenonMessage(L"MSG","ASG");
				lua_State* L =  it->LuaHandle;
				int ref = it->LuaRefObject;
				result |= it->ReplaceMode;


				lua_rawgeti06(L,LUA_REGISTRYINDEX,ref); //self.
				lua_getglobal06(L,"CommonObjectRef");lua_pushlightuserdata(L,StatePointerARG0);lua_pushlightuserdata(L,StateIMachineARG1);lua_pushlightuserdata(L,ContextARG2);lua_pushlightuserdata(L,0);lua_pcall06(L,4,1,0);
				lua_pcall06(L,1,0,0);
			}
		}
		return result;
	}

	void __fastcall Machine2Update(Sonicteam::Player::State::IMachine *a1, double a2){

		Sonicteam::Player::State::Machine2* _this = dynamic_cast<Sonicteam::Player::State::Machine2*>(a1);
		Sonicteam::Player::State::IContext* _context =   _this->MashineContext.get();
		if (_context) _context->ICOnPostInputTick();

		if (_this->PreState != _this->PostState){

		}
		BranchTo(0x8221E610,int,_this);
		if (_context){
			if (Sonicteam::SoX::AI::State<Sonicteam::Player::State::IContext>* _state = _this->CurrentState.get()){


				int ID = _this->GetCurrentMashineStateID();
				ProcessStateLua(a1,a2,-1,SM2_OnStateUpdate);
				ProcessStateLua(a1,a2,ID,SM2_OnStateUpdate);
				_state->OnStateUpdate(a2);
				ProcessStateLua(a1,a2,-1,SM2_OnStateUpdateP);
				ProcessStateLua(a1,a2,ID,SM2_OnStateUpdateP);
			}
		}
		BranchTo(0x8221E610,int,_this);

		if (_context) _context->ICOnInputTick(a2);


		return;
	}

	//Sonicteam::SoX::AI::StateMashine<IContext>*
	void OnStateChange(Sonicteam::Player::State::Machine2* _this,boost::shared_ptr<Sonicteam::SoX::AI::State<Sonicteam::Player::State::IContext>> NextState,Sonicteam::Player::State::IContext* context){
		

		if (context){

			
			if (Sonicteam::Player::State::IMachine* _thus = dynamic_cast<Sonicteam::Player::State::IMachine*>(_this))ProcessStateLua(_thus,0,-1,SM2_OnStateEnd);
			if (Sonicteam::Player::State::IMachine* _thus = dynamic_cast<Sonicteam::Player::State::IMachine*>(_this))ProcessStateLua(_thus,0,_thus->GetCurrentMashineStateID(),SM2_OnStateEnd);
			if (Sonicteam::SoX::AI::State<Sonicteam::Player::State::IContext>* state =  _this->CurrentState.get()) state->OnStateEnd();
			if (Sonicteam::Player::State::IMachine* _thus = dynamic_cast<Sonicteam::Player::State::IMachine*>(_this))ProcessStateLua(_thus,0,-1,SM2_OnStateEndP);
			if (Sonicteam::Player::State::IMachine* _thus = dynamic_cast<Sonicteam::Player::State::IMachine*>(_this))ProcessStateLua(_thus,0,_thus->GetCurrentMashineStateID(),SM2_OnStateEndP);

			if (Sonicteam::Player::State::IMachine* _thus = dynamic_cast<Sonicteam::Player::State::IMachine*>(_this))ProcessStateLua(_thus,0,-1,SM2_OnStateStart);
			if (Sonicteam::Player::State::IMachine* _thus = dynamic_cast<Sonicteam::Player::State::IMachine*>(_this))ProcessStateLua(_thus,0,_thus->GetCurrentMashineStateID(),SM2_OnStateStart);
			if (Sonicteam::SoX::AI::State<Sonicteam::Player::State::IContext>* state =  NextState.get()) state->OnStateStart(0);
			if (Sonicteam::Player::State::IMachine* _thus = dynamic_cast<Sonicteam::Player::State::IMachine*>(_this))ProcessStateLua(_thus,0,-1,SM2_OnStateStartP);
			if (Sonicteam::Player::State::IMachine* _thus = dynamic_cast<Sonicteam::Player::State::IMachine*>(_this))ProcessStateLua(_thus,0,_thus->GetCurrentMashineStateID(),SM2_OnStateStartP);
		}
		
		
		_this->CurrentState = NextState;
	}


	void GlobalInstall_StateMachine2(lua_State* LS)
	{
		if (LS == 0){

		//	WRITE_DWORD(0x8200BADC,Machine2Update);
		//	WRITE_DWORD(0x8200BB04,OnStateChange);

			return;
		}


		lua_register06(LS, SM2LIB_NAME, StateMachine2__NEW);
		luaL_newmetatable06(LS, SM2LIB_NAME_META);


		//		lua_pushstring06(LS,"ID"); lua_pushnumber(LS,0); lua_settable06(LS,-3); // ID = 0
		//		lua_pushstring06(LS,"ActorPTR"); lua_pushlightuserdata(LS,0); lua_settable06(LS,-3); // ActorPTR = 0
		//		lua_pushstring06(LS,"AI"); lua_pushboolean(LS,false); lua_settable06(LS,-3); // AI = 0


		lua_newtable06(LS);lua_pushstring06(LS, "__index");luaL_getmetatable06(LS, "MemoryMeta");lua_settable06(LS, -3); lua_setmetatable06(LS, -2); //setmetatable(StateMashineMeta, { __index = MemoryMeta })

		lua_pushstring06(LS, "GetIMachine");lua_pushcfunction06(LS, StateIMachine__GetIMachine);lua_settable06(LS, -3); // Equivalent to table["GetIMachine"] = StateIMachine__SetStateID
		lua_pushstring06(LS, "AddState");lua_pushcfunction06(LS, StateIMachine__AddState);lua_settable06(LS, -3); // Equivalent to table["AddState"] = StateIMachine__GetStateID
		lua_pushstring06(LS, "RemoveStateByID");lua_pushcfunction06(LS, StateIMachine__RemoveStateByID);lua_settable06(LS, -3); // Equivalent to table["RemoveStateByID"] = StateIMachine__GetStateID
		lua_pushstring06(LS, "OnStateConnect");lua_pushcfunction06(LS, StateMachine2__OnStateConnect);lua_settable06(LS, -3); // Equivalent to table["RemoveStateByID"] = StateIMachine__GetStateID


		lua_pop(LS,1);


		lua_register06(LS,"LuaCommonStateConnectRef",LuaCommonStateConnectRefConstructor);



		lua_State* L = LS;

		luaL_newmetatable06(L, "LuaCommonStateConnectRefMeta");
		lua_pushstring06(L,"__index"); lua_pushvalue(L,-2); lua_settable06(L,-3); // __index = MemoryMeta
	//	lua_pushstring06(L,"ptr"); lua_pushlightuserdata(L,(void*)0); lua_settable06(L,-3); // __index = MemoryMeta


		lua_pushstring06(L, "Disconnect"); lua_pushcfunction06(L,LuaCommonStateConnectRef__Disconnect);lua_settable06(L, -3);




		lua_pop(LS,1);



		return;


	}

	extern "C" int LuaCommonStateConnectRefConstructor(lua_State* L){

		lua_newtable06(L); // Create a new table for the instance // local self = {}

		// Second table for metatable set
		//	lua_newtable06(L);
		//	lua_pushstring06(L, "__index");luaL_getmetatable06(L, "MemoryMeta");lua_settable06(L, -3); // { __index = MemoryMeta }
		//	lua_setmetatable06(L, -2); // setmetatable(self, { __index = MemoryMeta })

		luaL_getmetatable06(L, "LuaCommonStateConnectRefMeta");
		lua_setmetatable06(L, -2); // setmetatable(self, { __index = MemoryMeta })


		lua_pushstring06(L, "StateIDType");lua_pushvalue06(L,1);lua_settable06(L, -3); // self.ptr = ptr
		lua_pushstring06(L, "StateID");lua_pushvalue06(L,2);lua_settable06(L, -3); // self.ptr = ptr
		lua_pushstring06(L, "FunctionRef");lua_pushvalue06(L,3);lua_settable06(L, -3); // self.ptr = ptr
		lua_pushstring06(L, "IMachinePTR");lua_pushvalue06(L,4);lua_settable06(L, -3); // self.ptr = ptr




		return 1;

	}
	extern "C" int LuaCommonStateConnectRef__Disconnect(lua_State* L){

	
		lua_pushstring06(L,"StateIDType");lua_gettable(L,1);
		int StateIDType = lua_tonumber(L,-1);

		lua_pushstring06(L,"StateID");lua_gettable(L,1);
		int StateID = lua_tonumber(L,-1);


		lua_pushstring06(L,"FunctionRef");lua_gettable(L,1);
		int FunctionRef = (int)lua_touserdata(L,-1);

		lua_pushstring06(L,"IMachinePTR");lua_gettable(L,1);
		Sonicteam::Player::State::IMachine* ptr = (Sonicteam::Player::State::IMachine*)lua_touserdata(L,-1);



		std::vector<LuaLCommonObjectSaveC>* vec = &CachedStatesConnect[ptr][StateID][StateIDType];

		for (std::vector<LuaLCommonObjectSaveC>::iterator it = vec->begin();it!=vec->end();it++ ){
			if (it->LuaRefObject == FunctionRef){
				vec->erase(it);
				break;
			}

		}

		return 1;
	}






	extern "C" int StateMachine2__CreateMetatable(lua_State* L,int ptr){



		lua_getglobal06(L, "Memory");
		lua_pushvalue(L,1); //LuaFirstArg
		lua_pcall06(L, 1, 1, 0); // Memory(ptr) -> {}::Memory


		// Second table for metatable set
		lua_newtable06(L);
		lua_pushstring06(L, "__index");luaL_getmetatable06(L, SM2LIB_NAME_META);lua_settable06(L, -3); // { __index = MemoryMeta }
		lua_setmetatable06(L, -2); // setmetatable(self, { __index = MemoryMeta })





		return 1;
	}
	extern "C" int StateMachine2__NEW(lua_State* L)
	{
		StateMachine2__CreateMetatable(L,0);
		return 1;
	}





	extern "C" int StateIMachine__GetIMachine(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);

		Sonicteam::Player::State::Machine2* ptr = (Sonicteam::Player::State::Machine2* )lua_touserdata(L,-1);
		if (Sonicteam::Player::State::IMachine* machine = dynamic_cast<Sonicteam::Player::State::IMachine*>(ptr)){

			lua_getglobal06(L, "StateIMachine");
			lua_pushlightuserdata(L,machine); //LuaFirstArg
			lua_pcall06(L, 1, 1, 0); // StateIMachine(ptr) -> {}::Memory
		}
		else{
			lua_pushnil(L);
		}

		return 1;
	}

	extern "C" int StateIMachine__AddState(lua_State* L)
	{

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);

		Sonicteam::Player::State::Machine2* ptr = (Sonicteam::Player::State::Machine2* )lua_touserdata(L,-1);
		std::vector<std::pair<int,StateConstructorEX>>* vec = (std::vector<std::pair<int,StateConstructorEX>>*)&ptr->MashineStates;
		int StateID =0;
		StateID = lua_tonumber(L,2);
		vec->push_back(std::pair<int,StateConstructorEX>(StateID,&LuaLCommonObjectBoost));

		lua_pushvalue06(L,3);
		int ref = luaL_ref(L,LUA_REGISTRYINDEX);



		CachedStates[dynamic_cast<Sonicteam::Player::State::IMachine*>(ptr)][StateID] = LuaLCommonObjectSave(L,ref);


		return 0;
	}

	extern "C" int StateIMachine__RemoveStateByID(lua_State* L)
	{
		return 0;
	}

	extern "C" int StateMachine2__OnStateConnect(lua_State*L)
	{
		int args = lua_gettop(L);
		const char* arg1 = lua_tostring(L,2);
		int StateID = lua_tonumber(L,3);

		bool ReplaceMode = false;
		if ( args > 5 && lua_isboolean(L,5)){
			ReplaceMode = lua_toboolean(L,5);
		}


		int _type = 0;

		if (strcmp("OnStart",arg1) == 0){
			_type = SM2_OnStateStart;
		}
		else if (strcmp("OnUpdate",arg1) == 0){
			_type = SM2_OnStateUpdate;
		}
		else if (strcmp("OnEnd",arg1) == 0){
			_type = SM2_OnStateEnd;
		}
		else if (strcmp("OnStartP",arg1) == 0){
			_type = SM2_OnStateStartP;
		}
		else if (strcmp("OnUpdateP",arg1) == 0){
			_type = SM2_OnStateUpdateP;
		}
		else if (strcmp("OnEndP",arg1) == 0){
			_type = SM2_OnStateEndP;
		}
		


		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::State::Machine2* ptr = (Sonicteam::Player::State::Machine2* )lua_touserdata(L,-1);

		lua_pushvalue06(L,4);
		int ref = luaL_ref(L,LUA_REGISTRYINDEX);

	
		//ShowXenonMessage(L"MSG",(int)dynamic_cast<Sonicteam::Player::State::IMachine*>(ptr),0);
		CachedStatesConnect[dynamic_cast<Sonicteam::Player::State::IMachine*>(ptr)][StateID][_type].push_back(LuaLCommonObjectSaveC(L,ref,_type,ReplaceMode));





		lua_getglobal06(L, "LuaCommonStateConnectRef");
		lua_pushnumber(L,_type);
		lua_pushnumber(L,StateID);
		lua_pushlightuserdata(L,(void*)ref);
		lua_pushlightuserdata(L,(void*)dynamic_cast<Sonicteam::Player::State::IMachine*>(ptr));
		lua_pcall06(L,4, 1, 0); // Memory(ptr) -> {}::Memory


		return 1;




	}

}
