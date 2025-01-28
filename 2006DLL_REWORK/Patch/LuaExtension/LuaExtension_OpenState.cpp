#include "LuaExtension_OpenState.h"


#define  LEOS_StateOnValue(v1,v2) (v1 << 4 | v2)

#define  LEOS_StateOnAny		LEOS_StateOnValue(-1,1) //-0xF
#define  LEOS_StateOnAnyPost	LEOS_StateOnValue(-1,2) //-0xE
#define  LEOS_StateOnAnyNo		LEOS_StateOnValue(-1,3) //-0xD

#define  LEOS_StateOnStartPre	LEOS_StateOnValue(1,1) //0x11
#define  LEOS_StateOnStartPost	LEOS_StateOnValue(1,2) //0x12
#define  LEOS_StateOnStartNo	LEOS_StateOnValue(1,3) //0x13

#define  LEOS_StateOnUpdatePre	LEOS_StateOnValue(2,1) //0x21
#define  LEOS_StateOnUpdatePost	LEOS_StateOnValue(2,2) //0x22
#define  LEOS_StateOnUpdateNo	LEOS_StateOnValue(2,3) //0x23

#define  LEOS_StateOnEndPre		LEOS_StateOnValue(3,1) //0x31
#define  LEOS_StateOnEndPost	LEOS_StateOnValue(3,2) //0x32
#define  LEOS_StateOnEndNo		LEOS_StateOnValue(3,3) //0x33


namespace DebugLogV2{


	int OpenStateExEnabled = false;


	std::map<int,std::map<int, std::map<int, std::map<int, std::vector<LEOS_Store>>>>>   LEOS_CACHE_STATE;
	bool LEOS_CACHE_STATE_LOCK = false;

	//map[BornUserDataLua][StateTypeID][StateID][StateWhen] -> vector[... -> lua_ref func -> value]
	
	bool ExecuteLuaFunction(const LEOS_Store& store,void* StatePointer,void* Context,int stateID,int when,double delta) {


		lua_State* L = (lua_State*)store.L;
		lua_rawgeti06(L,LUA_REGISTRYINDEX,store.L_Ref);
		lua_pushlightuserdata(L,StatePointer);
		lua_pushlightuserdata(L,Context);
		lua_pushnumber(L,stateID);
		lua_pushnumber(L,when);
		lua_pushnumber(L,delta);
		lua_pcall06(L,5,1,0);
		bool result = lua_toboolean(L,-1);
		lua_pop(L,1);
		
		return result;
		//ShowXenonMessage(L"MSG","AS");
	}

	bool ProcessStateLuaEX(Sonicteam::Player::State::Machine2* machine2,Sonicteam::SoX::AI::State<Sonicteam::Player::State::IContext>* CatchState,Sonicteam::Player::State::IContext* context, int StateID,int WHEN_ORIGIN, int WHEN, double delta) {



		// Retrieve the virtual function table (vft) pointer from the context
		int vft = *(int*)context;
		std::map<int, int> vft_to_index; // Mapping of vft to index
		bool block = false; // Flag to indicate if execution should be blocked


		int index = -1;


		int machine = reinterpret_cast<int>(machine2); // Cast Machine2* to int
		index = *(int*)(machine + 0x78); // Access the memory offset safely

		for (std::map<int,std::map<int, std::map<int, std::map<int, std::vector<LEOS_Store>>>>>::iterator LEOS_CACHE_STATE_1 = LEOS_CACHE_STATE.begin();LEOS_CACHE_STATE_1 != LEOS_CACHE_STATE.end();LEOS_CACHE_STATE_1++){

			// Check if there is cached state information for this index
			if (LEOS_CACHE_STATE_1->second.find(index) != LEOS_CACHE_STATE_1->second.end()) {
				std::map<int, std::map<int, std::vector<LEOS_Store>>> typed_cache = LEOS_CACHE_STATE_1->second[index];

				// Check if there is cached state information for the given StateID
				if (typed_cache.find(StateID) != typed_cache.end()) {
					std::map<int, std::vector<LEOS_Store>> typed_stated_cache = typed_cache[StateID];
					{
						// Handle specific WHEN values
						if (typed_stated_cache.find(WHEN) != typed_stated_cache.end()) {
							const std::vector<LEOS_Store>& stores = typed_stated_cache[WHEN];

							if (!stores.empty() && ((WHEN & 3) == 3)) block = true;

							for (size_t i = 0; i < stores.size(); ++i) {
								bool after = ExecuteLuaFunction(stores[i],CatchState,context,StateID,WHEN_ORIGIN,delta); // Execute the Lua function for this specific state and time.
								if (after) block = true;
							}
						}
					}
				}

				// Check if there is cached state information for the given StateID == -1
				if (typed_cache.find(-1) != typed_cache.end()) {
					std::map<int, std::vector<LEOS_Store>> typed_stated_cache = typed_cache[-1];
					{
						// Handle specific WHEN values
						if (typed_stated_cache.find(WHEN) != typed_stated_cache.end()) {
							const std::vector<LEOS_Store>& stores = typed_stated_cache[WHEN];

							if (!stores.empty() && ((WHEN & 3) == 3)) block = true;

							for (size_t i = 0; i < stores.size(); ++i) {
								bool after = ExecuteLuaFunction(stores[i],CatchState,context,StateID,WHEN_ORIGIN,delta); // Execute the Lua function for this specific state and time.
								if (after) block = true;
							}
						}
					}
				}


			}
		}


		return block; // Return whether execution was blocked or not.
	}

	
	
	void OnStateChangeRework(Sonicteam::Player::State::Machine2* _this,boost::shared_ptr<Sonicteam::SoX::AI::State<Sonicteam::Player::State::IContext>> NextState,Sonicteam::Player::State::IContext* context){

		Sonicteam::Player::State::Machine2* machine2 = dynamic_cast<Sonicteam::Player::State::Machine2*>(_this);

		if (NextState){

			if (_this->CurrentState){

				if (!ProcessStateLuaEX(machine2,_this->CurrentState.get(),context,_this->PreState,LEOS_StateOnEndPre,LEOS_StateOnAnyNo,0)){
					if (!ProcessStateLuaEX(machine2,_this->CurrentState.get(),context,_this->PreState,LEOS_StateOnEndPre,LEOS_StateOnAny,0)){
						if (!ProcessStateLuaEX(machine2,_this->CurrentState.get(),context,_this->PreState,LEOS_StateOnEndNo,LEOS_StateOnEndNo,0)){
							if (!ProcessStateLuaEX(machine2,_this->CurrentState.get(),context,_this->PreState,LEOS_StateOnEndPre,LEOS_StateOnEndPre,0)){
								_this->CurrentState.get()->OnStateEnd();
								ProcessStateLuaEX(machine2,_this->CurrentState.get(),context,_this->PreState,LEOS_StateOnEndPost,LEOS_StateOnEndPost,0);
							}
						}
						ProcessStateLuaEX(machine2,_this->CurrentState.get(),context,_this->PreState,LEOS_StateOnEndPost,LEOS_StateOnAnyPost,0);
					}
				}
			}

			if (!ProcessStateLuaEX(machine2,NextState.get(),context,_this->PreState,LEOS_StateOnStartPre,LEOS_StateOnAnyNo,0)){
				if (!ProcessStateLuaEX(machine2,NextState.get(),context,_this->PostState,LEOS_StateOnStartPre,LEOS_StateOnAny,0)){
					if (!ProcessStateLuaEX(machine2,NextState.get(),context,_this->PostState,LEOS_StateOnStartNo,LEOS_StateOnStartNo,0)){
						if (!ProcessStateLuaEX(machine2,NextState.get(),context,_this->PostState,LEOS_StateOnStartPre,LEOS_StateOnStartPre,0)){
							(NextState)->OnStateStart(context);
							ProcessStateLuaEX(machine2,NextState.get(),context,_this->PostState,LEOS_StateOnStartPost,LEOS_StateOnStartPost,0);
						}
					}
					ProcessStateLuaEX(machine2,NextState.get(),context,_this->PostState,LEOS_StateOnStartPost,LEOS_StateOnAnyPost,0);
				}

			}

		}
		_this->PreState = _this->PostState;
		_this->CurrentState = NextState;

	}


	
	void StateMachine2Tick(Sonicteam::Player::State::IMachine* _this, double a2){

		Sonicteam::Player::State::Machine2* machine2 = dynamic_cast<Sonicteam::Player::State::Machine2*>(_this);
		if ((machine2->MashineContext.get()))  machine2->MashineContext->ICOnPostInputTick();
		BranchTo(0x8221E610,int,machine2,a2);

		if ((machine2->MashineContext.get()) && machine2->CurrentState.get()){

			if (!ProcessStateLuaEX(machine2,machine2->CurrentState.get(),machine2->MashineContext.get(),machine2->PostState,LEOS_StateOnUpdatePre,LEOS_StateOnAnyNo,a2)){
				if (!ProcessStateLuaEX(machine2,machine2->CurrentState.get(),machine2->MashineContext.get(),machine2->PostState,LEOS_StateOnUpdatePre,LEOS_StateOnAny,a2)){
					if (!ProcessStateLuaEX(machine2,machine2->CurrentState.get(),machine2->MashineContext.get(),machine2->PostState,LEOS_StateOnUpdateNo,LEOS_StateOnUpdateNo,a2)){
						if (!ProcessStateLuaEX(machine2,machine2->CurrentState.get(),machine2->MashineContext.get(),machine2->PostState,LEOS_StateOnUpdatePre,LEOS_StateOnUpdatePre,a2)){
							machine2->CurrentState->OnStateUpdate(a2);
							ProcessStateLuaEX(machine2,machine2->CurrentState.get(),machine2->MashineContext.get(),machine2->PostState,LEOS_StateOnUpdatePost,LEOS_StateOnUpdatePost,a2);
						}
					}
					ProcessStateLuaEX(machine2,machine2->CurrentState.get(),machine2->MashineContext.get(),machine2->PostState,LEOS_StateOnUpdatePost,LEOS_StateOnAnyPost,a2);
				}
			}
		}
		BranchTo(0x8221E610,int,machine2,a2);

		if ((machine2->MashineContext.get()))  machine2->MashineContext->ICOnInputTick(a2);

	}



	int __fastcall GameScriptDestreoy(DWORD *a1, char a2){
		 BranchTo(0x8249FA18,int,a1,a2);
		 return 1;
	}

	int OpenState_GlobalInstall(lua_State* L)
	{

	//	if (!DebugLogV2::OpenStateExEnabled) return false;
		//CRASH, take shadow, now crissis city, instant jump off CAR, MOVE, then crash why??
		if (L == 0){


			WRITE_DWORD(0x820033B4 ,OpenStateRework);
			WRITE_DWORD(0x8200BB04,OnStateChangeRework);
	
			
			//	WRITE_DWORD(0x8200BB0C,Machine2OnStateChangeWeird);
		//	WRITE_DWORD(0x8202B050,GameScriptDestreoy); //USELES O_O WHY?

			WRITE_DWORD(0x8200BADC,StateMachine2Tick);

			WRITE_DWORD(0x8221E7DC,0x60000000); // v2->PreState = v2->PostState
		//	WRITE_DWORD(0x8238FD88,0x60000000);

			WRITE_DWORD(0x8219C994,0x3860007C);  //li        r3, 0x7C # 'x' # a1 for statemachine2
			return 0;
		}

		
		lua_register06(L, "OpenStateEx", OpenState__NEW);
		luaL_newmetatable06(L, "OpenStateExMeta");
		lua_pushstring06(L,"__index"); lua_pushvalue(L,-2); lua_settable06(L,-3); // __index = OpenStateExMeta

		lua_pushstring06(L,"Connect"); lua_pushcfunction06(L,Connect); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pushstring06(L,"Clear"); lua_pushcfunction06(L,Clear); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pushstring06(L,"__gc"); lua_pushcfunction06(L,OpenState__GC); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pop(L,1);



		lua_register06(L, "OpenStateExConnect", OpenStateConnect__NEW);
		luaL_newmetatable06(L, "OpenStateExConnectMeta");
		lua_pushstring06(L,"__index"); lua_pushvalue(L,-2); lua_settable06(L,-3); // __index = OpenStateExMeta
		lua_pushstring06(L,"Disconnect"); lua_pushcfunction06(L,OpenStateConnectDisconnect); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pop(L,1);



		return 0;
	}

	extern "C" Clear(lua_State* L){

		int elm = (int)lua_touserdata06(L,1);


		if (LEOS_CACHE_STATE.find(elm) != LEOS_CACHE_STATE.end()){
			LEOS_CACHE_STATE.erase(LEOS_CACHE_STATE.find(elm));
		}
	

		
		

		return 0;
	}
	extern "C" Connect(lua_State* L){
		
		lua_State* LuaHandle = L;
		int user_data = (int)lua_touserdata06(L,1);
		int c_module_state = lua_tonumber06(L,2); // Sonic,Shadow,Silver,.... StateContext 
		int StateID = lua_tonumber06(L,3); // 
		int StateWHEN = lua_tonumber06(L,4); //  LEOS_State_%

		lua_pushvalue06(L,5);
		int StateFuncFromLua = luaL_ref06(L,LUA_REGISTRYINDEX);
		//luaL_unref(L,LUA_REGISTRYINDEX,StateFuncFromLua); // Stable For Now Finally :)
	


		LEOS_Store store =  LEOS_Store();store.L = (int)L; store.L_Ref = StateFuncFromLua;
		LEOS_CACHE_STATE[user_data][c_module_state][StateID][StateWHEN].push_back(store);
		OpenStateConnect__CreateMetatable(L,user_data,c_module_state,StateID,StateWHEN,StateFuncFromLua);


		return 1;
	}

	extern "C" int OpenStateRework(lua_State* L) {
		BranchTo(0x8219C960, int, L);

		int ObjPlayer = *(int*)lua_topointer(L, 1);
		int MachineType = lua_tonumber(L,2);
		Sonicteam::Player::State::IMachine* plr_imachine = *(Sonicteam::Player::State::IMachine**)(ObjPlayer + 0xE4);
		Sonicteam::Player::State::Machine2* plr_machine2 = static_cast<Sonicteam::Player::State::Machine2*>(plr_imachine);
		int machine2 = (int)plr_machine2;
		*(int*)(machine2 + 0x78)= MachineType;
		return 0;
	}





	extern "C" int OpenState__CreateMetatable(lua_State* L){
		lua_newuserdata06(L,4);
		luaL_getmetatable06(L, "OpenStateExMeta");
		lua_setmetatable06(L,-2);
		return 1;
	}

	extern "C" int OpenStateConnect__CreateMetatable(lua_State* L,int user_data,int c_module_state,int StateID, int StateWHEN,int StateFuncFromLua)
	{
		lua_newtable06(L);
		luaL_getmetatable06(L, "OpenStateExConnectMeta");
		lua_setmetatable06(L,-2);




		lua_pushstring06(L,"user_data");
		lua_pushlightuserdata(L,(void*)user_data);
		lua_settable06(L,-3);


		lua_pushstring06(L,"c_module_state");
		lua_pushlightuserdata(L,(void*)c_module_state);
		lua_settable06(L,-3);


		lua_pushstring06(L,"StateID");
		lua_pushlightuserdata(L,(void*)StateID);
		lua_settable06(L,-3);


		lua_pushstring06(L,"StateWHEN");
		lua_pushlightuserdata(L,(void*)StateWHEN);
		lua_settable06(L,-3);

		lua_pushstring06(L,"StateFuncFromLua");
		lua_pushlightuserdata(L,(void*)StateFuncFromLua);
		lua_settable06(L,-3);



		return 1;
	}

	extern "C" int DisconnectLeosState(int user_data,int c_module_state,int StateID,int StateWHEN,int StateFuncFromLua ){

		// Find and remove the corresponding LEOS_Store entry
		std::vector<LEOS_Store> _states_ = LEOS_CACHE_STATE[user_data][c_module_state][StateID][StateWHEN];

		

		for (std::vector<LEOS_Store>::iterator it = _states_.begin();it != _states_.end();it++){
			if (it->L_Ref == (int)StateFuncFromLua) {
				luaL_unref((lua_State*)it->L, LUA_REGISTRYINDEX, it->L_Ref);
				_states_.erase(it);
				break; //
			}
		}

		return 0;

	}

	extern "C" int OpenStateConnectDisconnect(lua_State* L)
	{

		lua_pushstring06(L,"user_data");
		lua_gettable(L,1);
		int user_data = (int)lua_touserdata(L,-1);


		lua_pushstring06(L,"c_module_state");
		lua_gettable(L,1);
		int c_module_state = (int)lua_touserdata(L,-1);


		lua_pushstring06(L,"StateID");
		lua_gettable(L,1);
		int StateID = (int)lua_touserdata(L,-1);


		lua_pushstring06(L,"StateWHEN");
		lua_gettable(L,1);
		int StateWHEN = (int)lua_touserdata(L,-1);

		lua_pushstring06(L,"StateFuncFromLua");
		lua_gettable(L,1);
		int StateFuncFromLua = (int)lua_touserdata(L,-1);


		DisconnectLeosState(user_data,c_module_state,StateID,StateWHEN,StateFuncFromLua);
		return 1;

	}

	extern "C" int OpenStateConnect__NEW(lua_State* L)
	{
		//OpenStateConnect__CreateMetatable(L,lua_tonumber(L,1));
		return 1;
	}

	extern "C" int OpenState__NEW(lua_State* L)
	{
		OpenState__CreateMetatable(L);
		return 1;
	}
	extern "C" int OpenState__GC(lua_State* L){
		Clear(L);
		return 0;
	}


	



}