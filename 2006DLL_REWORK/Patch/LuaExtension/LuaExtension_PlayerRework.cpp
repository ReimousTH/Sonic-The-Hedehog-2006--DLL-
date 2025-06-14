#include "LuaExtension_PlayerRework.h"

#define PLIB_NAME_META "PlayerMeta"
#define PLIB_NAME "Player"

#include <Player/State/Machine2.h>

namespace DebugLogV2{





	void GlobalInstall_PlayerRework(lua_State* LS)
	{
		if (LS == 0){
			WRITE_DWORD(0x82195AAC,POWERPC_ADDIS(11,0,POWERPC_HI((unsigned int)&FixedPlayerFunctionsTables)));
			WRITE_DWORD(0x82195AB4,POWERPC_ADDI(11,11,POWERPC_LO((unsigned int)&FixedPlayerFunctionsTables)));
			return;
		}

		
		lua_register06(LS, PLIB_NAME, Player__NEW);
		luaL_newmetatable06(LS, PLIB_NAME_META);

	
		lua_pushstring06(LS,"ID"); lua_pushnumber(LS,0); lua_settable06(LS,-3); // ID = 0
		lua_pushstring06(LS,"ActorPTR"); lua_pushlightuserdata(LS,0); lua_settable06(LS,-3); // ActorPTR = 0
		lua_pushstring06(LS,"AI"); lua_pushboolean(LS,false); lua_settable06(LS,-3); // AI = 0
		lua_pushstring06(LS,"__index"); lua_pushvalue(LS,-2); lua_settable06(LS,-3); // __index = PlayerMeta

	
		lua_newtable06(LS);lua_pushstring06(LS, "__index");luaL_getmetatable06(LS, "MemoryMeta");lua_settable06(LS, -3); lua_setmetatable06(LS, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })

	

		lua_pushstring06(LS, "GetName");lua_pushcfunction06(LS, PlayerR__GetName);lua_settable06(LS, -3); // Equivalent to table["GetName"] = PlayerR__GetName
		lua_pushstring06(LS, "SetName");lua_pushcfunction06(LS, PlayerR__SetName);lua_settable06(LS, -3); // Equivalent to table["GetName"] = PlayerR__GetName
		lua_pushstring06(LS, "SWAP");lua_pushcfunction06(LS, PlayerR__SWAP);lua_settable06(LS, -3); // Equivalent to table["SWAP"] = PlayerR__SWAP
		lua_pushstring06(LS, "Reload");lua_pushcfunction06(LS, PlayerR__Reload);lua_settable06(LS, -3); // Equivalent to table["Reload"] = PlayerR__Reload
		lua_pushstring06(LS, "GetPosition");lua_pushcfunction06(LS, PlayerR__GetPosition);lua_settable06(LS, -3); // Equivalent to table["GetPosition"] = PlayerR__GetPosition
		lua_pushstring06(LS, "SetPosition");lua_pushcfunction06(LS, PlayerR__SetPosition);lua_settable06(LS, -3); // Equivalent to table["SetPosition"] = PlayerR__SetPosition
		lua_pushstring06(LS, "SetScale");lua_pushcfunction06(LS, PlayerR__SetScale);lua_settable06(LS, -3); // Equivalent to table["SetPosition"] = PlayerR__SetPosition
		lua_pushstring06(LS, "SetActorPTR");lua_pushcfunction06(LS, PlayerR__SetActorPTR);lua_settable06(LS, -3); // Equivalent to table["SetActorPTR"] = PlayerR__SetActorPTR
		lua_pushstring06(LS, "GetActorPTR");lua_pushcfunction06(LS, PlayerR__GetActorPTR);lua_settable06(LS, -3); // Equivalent to table["SetActorPTR"] = PlayerR__SetActorPTR

		
		lua_pushstring06(LS, "GetActorID");lua_pushcfunction06(LS, PlayerR__GetActorID);lua_settable06(LS, -3); // Equivalent to table["GetActorID"] = PlayerR__GetActorID
		lua_pushstring06(LS, "GetStateID");lua_pushcfunction06(LS, PlayerR__GetStateID);lua_settable06(LS, -3); // Equivalent to table["GetStateID"] = PlayerR__GetStateID
		lua_pushstring06(LS, "SetStateID");lua_pushcfunction06(LS, PlayerR__SetStateID);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "SetStateIDF");lua_pushcfunction06(LS, PlayerR__SetStateIDForce);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "GetMachine2");lua_pushcfunction06(LS, PlayerR__GetMachine2);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID


		lua_pushstring06(LS, "OpenPackage");lua_pushcfunction06(LS, PlayerR__OpenPackage);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "OpenFrame");lua_pushcfunction06(LS, PlayerR__OpenFrame);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "OpenInput");lua_pushcfunction06(LS, PlayerR__OpenInput);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "OpenModel");lua_pushcfunction06(LS, PlayerR__OpenModel);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "OpenZock");lua_pushcfunction06(LS, PlayerR__OpenZock);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "OpenPerformance");lua_pushcfunction06(LS, PlayerR__OpenPerformance);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "OpenBody");lua_pushcfunction06(LS, PlayerR__OpenBody);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "OpenAutomaticDead");lua_pushcfunction06(LS, PlayerR__OpenAutomaticDead);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "OpenSound");lua_pushcfunction06(LS, PlayerR__OpenSound);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "OpenEffect");lua_pushcfunction06(LS, PlayerR__OpenEffect);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "OpenOther");lua_pushcfunction06(LS, PlayerR__OpenOther);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "OpenImpulse");   lua_pushcfunction06(LS, PlayerR__OpenImpulse);   lua_settable06(LS, -3);
		lua_pushstring06(LS, "OpenPath");      lua_pushcfunction06(LS, PlayerR__OpenPath);      lua_settable06(LS, -3);
		lua_pushstring06(LS, "OpenLightDash"); lua_pushcfunction06(LS, PlayerR__OpenLightDash); lua_settable06(LS, -3);
		lua_pushstring06(LS, "OpenScore");     lua_pushcfunction06(LS, PlayerR__OpenScore);     lua_settable06(LS, -3);
		lua_pushstring06(LS, "OpenBodyList");  lua_pushcfunction06(LS, PlayerR__OpenBodyList);  lua_settable06(LS, -3);
		lua_pushstring06(LS, "OpenLockOn");    lua_pushcfunction06(LS, PlayerR__OpenLockOn);    lua_settable06(LS, -3);
		lua_pushstring06(LS, "OpenGameMaster");lua_pushcfunction06(LS, PlayerR__OpenGameMaster);lua_settable06(LS, -3);

		lua_pushstring06(LS, "OpenEventer");   lua_pushcfunction06(LS, PlayerR__OpenEventer);   lua_settable06(LS, -3);
		lua_pushstring06(LS, "OpenWeapons");   lua_pushcfunction06(LS, PlayerR__OpenWeapons);   lua_settable06(LS, -3);
		lua_pushstring06(LS, "OpenPostureControl"); lua_pushcfunction06(LS, PlayerR__OpenPostureControl); lua_settable06(LS, -3);
		lua_pushstring06(LS, "OpenGravity");   lua_pushcfunction06(LS, PlayerR__OpenGravity);   lua_settable06(LS, -3);
		lua_pushstring06(LS, "OpenState");     lua_pushcfunction06(LS, PlayerR__OpenState);     lua_settable06(LS, -3);
		lua_pushstring06(LS, "OpenGauge");     lua_pushcfunction06(LS, PlayerR__OpenGauge);     lua_settable06(LS, -3);

		
		lua_pushstring06(LS, "OpenOtherParameter");lua_pushcfunction06(LS, PlayerR__OpenOtherParameter);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "DynamicLink");lua_pushcfunction06(LS, PlayerR__IDynamicLink);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "IDynamicLink");lua_pushcfunction06(LS, PlayerR__IDynamicLink2);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "IEventerListener");lua_pushcfunction06(LS, PlayerR__IEventerListener);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "IVariable");lua_pushcfunction06(LS, PlayerR__IVarible);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID
		lua_pushstring06(LS, "RemovePlugin");lua_pushcfunction06(LS, PlayerR__RemovePlugin);lua_settable06(LS, -3); // Equivalent to table["SetStateID"] = PlayerR__SetStateID



		lua_pushstring06(LS,"GetIPluginByName"); lua_pushcfunction06(LS, PlayerR__GetIPluginByName);lua_settable06(LS, -3); // Equivalent to table["GetIPluginByName"] = PlayerR__GetIPluginByName
		

		
		
		lua_pop(LS,1);
		

		return;
	

	}

	 int GetPlayerActorsID(unsigned int ID,bool AI){


		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);
		Sonicteam::GameImp* gameimp_2 = *(Sonicteam::GameImp**)(impl->DocCurrentMode + 0x6C);

		int LocalPlayer = 0;
		if (gameimp == 0 || *(UINT32*)gameimp != 0x82001AEC) return -1;
		UINT32 vft =  *(UINT32*)impl->DocCurrentMode;


		if (vft != 0x82033534) return -1;
		if ( *(UINT32*)(gameimp+0x11C4) == 0) return -1;
		INT32 ActorManager = *(UINT32*)(gameimp+0x11C4);

		int ActorID =  *(int*)(gameimp + 0xE40) + (ID * 0x4C);

		if (AI){
			ActorID = gameimp_2->ObjPlayersActorID[ID];
		}
	

	

		return ActorID;
	}


	void GetPlayerActorsR(UINT32* pstack,bool AI){


		pstack[0] = 0;
		pstack[1] = 0;
		pstack[2] = 0;
		pstack[3] = 0;


		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);

		int LocalPlayer = 0;
		if (gameimp == 0 || *(UINT32*)gameimp != 0x82001AEC) return;
		UINT32 vft =  *(UINT32*)impl->DocCurrentMode;


		if (vft != 0x82033534)return;
		if ( *(UINT32*)(gameimp+0x11C4) == 0) return;
		INT32 ActorManager = *(UINT32*)(gameimp+0x11C4);

		if (!AI){
			for (int i = 0;i<4;i++){
				UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);
				int ActorID =  *(int*)(gameimp + 0xE40) + (i * 0x4C);
				if (ActorID == -1) continue;
				pstack[i] = BranchTo(0x821609D0,int,ActorManager,&ActorID);
			}
		}
		else{
			for (int i = 0;i<4;i++){
				Sonicteam::GameImp* gameimp = *(Sonicteam::GameImp**)(impl->DocCurrentMode + 0x6C);
				int ActorID  = gameimp->ObjPlayersActorID[i];
				if (ActorID == -1) continue;
				pstack[i] = BranchTo(0x821609D0,int,ActorManager,&ActorID);
			}
		}
	

	
	}

	void GetPlayerActorsPTR(UINT32* pstack,bool AI){

		
		pstack[0] = 0;
		pstack[1] = 0;
		pstack[2] = 0;
		pstack[3] = 0;


		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);

		int LocalPlayer = 0;
		if (gameimp == 0 || *(UINT32*)gameimp != 0x82001AEC) return;
		UINT32 vft =  *(UINT32*)impl->DocCurrentMode;


		if (vft != 0x82033534)return;
		if ( *(UINT32*)(gameimp+0x11C4) == 0) return;
		INT32 ActorManager = *(UINT32*)(gameimp+0x11C4);

		if (!AI){
			for (int i = 0;i<4;i++){
				Sonicteam::GameImp* gameimp_2 = *(Sonicteam::GameImp**)(impl->DocCurrentMode + 0x6C);
				UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);
				int ActorID =  *(int*)(gameimp + 0xE40) + (i * 0x4C);
				if (ActorID == -1) continue;
				Sonicteam::ActorManager* manager =  gameimp_2->GameActorManager.get();
				
				for (int j = 0;j<0xFFFF;j++){
					if (manager->ActorID[j] == ActorID) {
						pstack[i] = (UINT32)&manager->Actor[j];
						break;
					}
				}

				

				
			}
		}
		else{
			for (int i = 0;i<4;i++){
				Sonicteam::GameImp* gameimp = *(Sonicteam::GameImp**)(impl->DocCurrentMode + 0x6C);
				int ActorID  = gameimp->ObjPlayersActorID[i];
				if (ActorID == -1) continue;
				Sonicteam::ActorManager* manager =  gameimp->GameActorManager.get();

				for (int j = 0;j<0xFFFF;j++){
					if (manager->ActorID[j] == ActorID) {
						pstack[i] = (UINT32)&manager->Actor[j];
						break;
					}
				}
				
			}
		}
	}

	extern "C" int Player__CreateMetatable(lua_State* L,int ID,bool AI){
		

		UINT32 stack1[16];
		memset(&stack1,0,16*4);
		UINT32 stack2[16];
		memset(&stack2,0,16*4);
		GetPlayerActorsR((UINT32*)&stack1, AI);
		//GetPlayerActorsPTR((UINT32*)&stack2, AI); --Remove From Here to save perfomance


		lua_getglobal06(L, "Memory");
		lua_pushlightuserdata(L, (void*)stack1[ID]);
		lua_pcall06(L, 1, 1, 0); // Memory(stack1[ID]) -> {}::Memory


		// Second table for metatable set
	//	lua_newtable06(L);
	//	lua_pushstring06(L, "__index");luaL_getmetatable06(L, PLIB_NAME_META);lua_settable06(L, -3); // { __index = MemoryMeta }
	//	lua_setmetatable06(L, -2); // setmetatable(self, { __index = MemoryMeta })


		luaL_getmetatable06(L, PLIB_NAME_META);
		lua_setmetatable06(L,-2);

		lua_pushstring06(L, "ID");
		lua_pushnumber(L, ID);
		lua_settable06(L, -3); // self.ptr = ptr

		lua_pushstring06(L, "Flags");
		lua_pushlightuserdata(L, 0);
		lua_settable06(L, -3); // self.ptr = ptr


	//	lua_pushstring06(L, "ActorPTR");
		//lua_pushlightuserdata(L, (void*)stack2[ID]);
	//	lua_settable06(L, -3); // self.ActorPTR = ptr


		lua_pushstring06(L, "AI");
		lua_pushboolean(L, AI);
		lua_settable06(L, -3); // self.move = move

		return 1;
	}
	extern "C" int Player__NEW(lua_State* L)
	{
		int arg1 = 0;
		bool arg2 = false;
		int args = lua_gettop(L);
		if (args > 0){
			if (lua_isnumber(L,1)) arg1 = lua_tonumber(L,1);
		}
		if (args > 1){
			if (lua_isboolean(L,2)) arg2 = lua_toboolean(L,2);
		}
		Player__CreateMetatable(L,arg1,arg2);
		return 1;
	}

	extern "C" int PlayerR__GetName(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		int ptr = (int)lua_touserdata(L,-1);

		std::string* c_player_name =  (std::string*)(ptr + 0x1D8);
		lua_pushstring06(L,	c_player_name->c_str());
		return 1;
	}

	extern "C" int PlayerR__SetName(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* ptr = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		ptr->PlayerName = lua_tostring(L,2);
		return 0;
	}

	extern "C" int PlayerR__Reload(lua_State*L)
	{
		lua_pushstring06(L,"ID");
		lua_gettable(L,1);
		int ID = (int)lua_tonumber(L,-1);

		lua_pushstring06(L,"AI");
		lua_gettable(L,1);
		bool AI = lua_toboolean(L,-1);



		UINT32 stack[16];
		GetPlayerActorsR((UINT32*)&stack,AI);


		lua_pushstring06(L,"ptr");
		lua_pushlightuserdata(L,(void*)stack[ID]);
		lua_settable06(L,1);



		//lua_pushstring06(L,"ActorPTR");
		//GetPlayerActorsPTR((UINT32*)&stack,AI);
		//lua_pushlightuserdata(L,(void*)stack[ID]);
		//lua_settable06(L,1);



		lua_pushvalue06(L,1); //self
		return 1;

		

	}

	extern "C" int PlayerR__SWAP(lua_State*L)
	{

		int args = lua_gettop(L);
		if (args <= 1) return 0;
		const char* chara = lua_tostring(L,2); //arg2
		
		
		
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		void* ObjectPlayerPTR = lua_touserdata(L,-1);
		BranchTo(0x82195D20,int,ObjectPlayerPTR,chara);
		return 0;

	}


	extern "C" int Vector__CallLUAConstructor(lua_State* L,XMVECTOR* vector){


		lua_getglobal06(L,"Vector");

		lua_pushnumber(L,vector->x);
		lua_pushnumber(L,vector->y);
		lua_pushnumber(L,vector->z);
		lua_pushnumber(L,vector->w);

		lua_pcall06(L,4,1,0);
		return 1;

	}

	UINT32 GetPlayerPostureR(UINT32 ObjectPlayer){
		return *(UINT32 *)(ObjectPlayer + 0xDC);
	}
	XMVECTOR* GetPlayerPositionR(UINT32 PlayerPosture){
		return (XMVECTOR*)(PlayerPosture + 0xB0);
	}


	extern "C" int PlayerR__GetPosition(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		void* ObjectPlayerPTR = lua_touserdata(L,-1);
		int OBJPlayer = (int)ObjectPlayerPTR;
		XMVECTOR vector = {0};
		UINT32 PlayerPosture = GetPlayerPostureR(OBJPlayer);
		vector =  *(GetPlayerPositionR(PlayerPosture));
		Vector__CallLUAConstructor(L,&vector);

		return 1;
	}

	extern "C"  XMVECTOR Vector__GetVectorTable(lua_State* L,int idx){
		XMVECTOR vector1 = {0,0,0,1};
		if (lua_istable(L,idx))
		for (int i = 1; i <= 4; ++i) {
			lua_rawgeti06(L, idx, i); // Get value at index i

			if (lua_isnumber(L, -1)) {
				float value = lua_tonumber(L, -1);
				vector1.v[i-1] = value;
			}

			// Pop the value from the stack
			lua_pop(L, 1);
		}
		return vector1;
	}


	extern "C" int PlayerR__SetPosition(lua_State* L)
	{
		int args = lua_gettop(L)-1;
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		void* ObjectPlayerPTR = lua_touserdata(L,-1);


		int OBJPlayer = (int)ObjectPlayerPTR;
		XMVECTOR vector = {0};



		//Vector
		if (lua_istable(L,2)){
			vector = Vector__GetVectorTable(L,2);

		}
		else{

			if (args > 0 && lua_isnumber(L,2) ) vector.x = lua_tonumber(L,2); 
			if (args > 1 && lua_isnumber(L,3) ) vector.y = lua_tonumber(L,3); 
			if (args > 2 && lua_isnumber(L,4) ) vector.z = lua_tonumber(L,4); 
			if (args > 3 && lua_isnumber(L,5) ) vector.w = lua_tonumber(L,5); 
		}


		UINT32 PlayerPosture = GetPlayerPostureR(OBJPlayer);
		*(GetPlayerPositionR(PlayerPosture)) = vector;


		return 0;
	}
	extern "C" int PlayerR__SetScale(lua_State* L)
	{
		int args = lua_gettop(L)-1;
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		void* ObjectPlayerPTR = lua_touserdata(L,-1);


		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)ObjectPlayerPTR;
		XMVECTOR vector = {0};



		//Vector
		if (lua_istable(L,2)){
			vector = Vector__GetVectorTable(L,2);

		}
		else{

			if (args > 0 && lua_isnumber(L,2) ) vector.x = lua_tonumber(L,2); 
			if (args > 1 && lua_isnumber(L,3) ) vector.y = lua_tonumber(L,3); 
			if (args > 2 && lua_isnumber(L,4) ) vector.z = lua_tonumber(L,4); 
			if (args > 3 && lua_isnumber(L,5) ) vector.w = lua_tonumber(L,5); 
		}

		Sonicteam::Player::RootFrame* frame =  OBJPlayer->RootFrame.get();

		XMMATRIX currentTransform = frame->FrameGetTransformMatrix2(); // Get the current transformation matrix


		// Create a new scaling matrix
		XMMATRIX scalingMatrix = XMMatrixScaling(vector.x, vector.y, vector.z);

		// Extract translation from the current transformation matrix
		XMVECTOR translationVector = XMVectorSet(currentTransform.r[3].x, 
			currentTransform.r[3].y, 
			currentTransform.r[3].z, 
			1.0f);

		// Extract rotation (if needed) - here we assume no rotation is needed for simplicity
		XMMATRIX rotationMatrix = XMMatrixIdentity(); // Replace with actual rotation extraction if needed

		// Combine the new scaling with existing translation and rotation
		XMMATRIX newTransform = scalingMatrix * rotationMatrix * XMMatrixTranslationFromVector(translationVector);

		// Set the new transform back to the frame or object
		frame->FrameSetTransformMatrix2(rotationMatrix); // Assuming there's a method to set 




		return 0;
	}



	extern "C" int PlayerR__GetActorID(lua_State* L)
	{
		lua_pushstring06(L,"ID");
		lua_gettable(L,1);
		int ID  = lua_tonumber(L,-1);


		lua_pushstring06(L,"AI");
		lua_gettable(L,1);
		bool AI  = lua_toboolean(L,-1);

		lua_pushnumber(L,GetPlayerActorsID(ID,AI));
		return 1;
	}

	extern "C" int PlayerR__GetActorPTR(lua_State* L){


		lua_pushstring06(L,"ID");
		lua_gettable(L,1);
		int ID  = lua_tonumber(L,-1);


		lua_pushstring06(L,"AI");
		lua_gettable(L,1);
		bool AI  = lua_toboolean(L,-1);



		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);
		Sonicteam::GameImp* gameimp_2 = *(Sonicteam::GameImp**)(impl->DocCurrentMode + 0x6C);

		int LocalPlayer = 0;
		if (gameimp == 0 || *(UINT32*)gameimp != 0x82001AEC) return 0;
		UINT32 vft =  *(UINT32*)impl->DocCurrentMode;


		if (vft != 0x82033534)  return 0;

		Sonicteam::ActorManager* ActorManager = *(Sonicteam::ActorManager**)(gameimp+0x11C4);
		
		int AID = gameimp_2->PlayerGameplayerData[ID].ActorID;
		if (AI){
			AID = gameimp_2->ObjPlayersActorID[ID];
		}

	
		unsigned int left = 0;
		unsigned int right = ActorManager->LastActorIndex-1;
		int Index = 0;

		while (left <= right) {
			unsigned int mid = left + (right - left) / 2;
			if (ActorManager->ActorID[mid] == AID) {
				Index = mid;
				break;
			}

			if (ActorManager->ActorID[mid] < AID) {
				left = mid + 1;
			}
			else {
				right = mid - 1;
			}
		}



		
		lua_pushlightuserdata(L,&ActorManager->Actor[Index]);
		

		return 1;

	}

	extern "C" int PlayerR__SetActorPTR(lua_State* L)
	{
		return 0;
	}





	extern "C" int PlayerR__GetStateID(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		int OBJPlayer = (int)lua_touserdata(L,-1);
		if (OBJPlayer ){
			Sonicteam::Player::State::IMachine* Mashine = *(Sonicteam::Player::State::IMachine**)(OBJPlayer + 0xE4);
			if (Mashine){
				lua_pushnumber(L,Mashine->GetCurrentMashineStateID());
			}
			else{
				lua_pushnil(L);
			}

		}
		else{
			lua_pushnil(L);
		}


		return 1;
	}

	extern "C" int PlayerR__SetStateID(lua_State* L)
	{


		int args = lua_gettop(L)-1;
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		int OBJPlayer = (int)lua_touserdata(L,-1);
		int StateID = lua_tonumber(L,2);

		int way = 0;
		if (args >= 3) way = lua_tonumber(L,3);


		if (OBJPlayer ){
			Sonicteam::Player::State::IMachine* Mashine = *(Sonicteam::Player::State::IMachine**)(OBJPlayer + 0xE4);

			switch (way){
				case 0:
					Mashine->ChangeMashineState(StateID);
					break;
				case 1:
					Mashine->AlternativeChangeMashineState(StateID);
					break;
				case 2:
					Mashine->CompleteChangeMashineState(StateID);
					break;
			}

		}
	

		return 0;
	}

	extern "C" int PlayerR__SetStateIDForce(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		int OBJPlayer = (int)lua_touserdata(L,-1);
		int StateID = lua_tonumber(L,2);

		if (OBJPlayer ){
			Sonicteam::Player::State::IMachine* Mashine = *(Sonicteam::Player::State::IMachine**)(OBJPlayer + 0xE4);
			Sonicteam::Player::State::Machine2* m2 = static_cast<Sonicteam::Player::State::Machine2*>(Mashine);
			m2->PreState = StateID; // force WAY INTO
			if (m2->PostState != m2->PreState){
				m2->MashineStateFlag1 = 0;
				m2->MashineStateFlag2 = 0;
			}
		}

		return 0;
	}

	extern "C" int PlayerR__GetMachine2(lua_State* L)
	{

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		int OBJPlayer = (int)lua_touserdata(L,-1);
		int StateID = lua_tonumber(L,2);

		if (OBJPlayer ){
			Sonicteam::Player::State::IMachine* Mashine = *(Sonicteam::Player::State::IMachine**)(OBJPlayer + 0xE4);
		//	Sonicteam::Player::State::Machine2* Machine2 = dynamic_cast<Sonicteam::Player::State::Machine2*>(Mashine);

			lua_getglobal06(L,"StateIMachine");
			lua_pushlightuserdata(L,(void*)Mashine);
			lua_pcall06(L,1,1,0);
		}

		return 1;


	}

	extern "C" int PlayerR__GetIPluginByName(lua_State* L)
	{

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		int OBJPlayer = (int)lua_touserdata(L,-1);
		const char* plugname = lua_tostring(L,2);


		int found_plugin = 0;
		std::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>* IPluginP = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>*>(OBJPlayer + 0x114);
		for (std::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>::iterator it = IPluginP->begin(); it != IPluginP->end(); it++) {
			boost::shared_ptr<Sonicteam::Player::IPlugIn> pluginPtr = *it;
			Sonicteam::Player::IPlugIn* plugin = (*it).get();
				if  (plugin->PluginName == plugname){
					found_plugin = (int)plugin;
					break;
				}
		}

		Memory__CreateMetatable(L,found_plugin,0);
		return 1;
	}

	#define REOPEN_DYNLINK(FLAG) ((FLAG & 0xFF) << 8)
	#define REOPEN_IVAR(FLAG) ((FLAG & 0xFF) << 16)
	
	//0-255 -max
	#define REOPEN_MODEL 1
	#define REOPEN_OTHER 2
	#define REOPEN_SOUND 4
	#define REOPEN_EFFECT 8
	#define REOPEN_OPENOTHERPARAMETER 0x10



	extern "C" int PlayerR__OpenPackage(lua_State* L)
	{

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		int OBJPlayer = (int)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);

		const char* PackageName = lua_tostring(L,2);


		lua_replace(L,1);

	


		BranchTo(0x8219B948,int,L);

		return 0;
	}


	extern "C" int PlayerR__OpenFrame(lua_State* L)
	{
		int arg_count = lua_gettop(L);
		int arg2 = lua_tonumber(L,2);



		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);


		BranchTo(0x8219BAE8,int,L);


		lua_pushlightuserdata(L,OBJPlayer->FindPluginLast("frame").get());
		return 1;


	}

	extern "C" int PlayerR__OpenInput(lua_State* L)
	{
		int arg_count = lua_gettop(L);
		int arg2 = lua_tonumber(L,2);



		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);


		BranchTo(0x8219BBE8 ,int,L);

		const char* find_array[] = {
			"input",
			"input",
			"input snowboard",
			"input vehicle", //not reg
			"input"
		};


		lua_pushlightuserdata(L,OBJPlayer->FindPluginLast(find_array[arg2]).get()); //module



		return 1;

	}

	extern "C" int PlayerR__OpenZock(lua_State* L)
	{
		int arg_count = lua_gettop(L);
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);
		BranchTo(0x8219CA80,int,L);
		lua_pushlightuserdata(L,OBJPlayer->FindPluginLast("zock").get()); //module
		return 1;
	}

	extern "C" int PlayerR__OpenPerformance(lua_State* L)
	{
		lua_pushlightuserdata(L,(void*)0);
		return 1;
	}

	extern "C" int PlayerR__OpenImpulse(lua_State* L)
	{
		int arg_count = lua_gettop(L);
	


		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);


		BranchTo(0x8219CBC8,int,L);

		lua_pushlightuserdata(L,OBJPlayer->FindPluginLast("impulse").get()); //module


		return 1;

	}

	extern "C" int PlayerR__OpenPath(lua_State* L)
	{
		int arg_count = lua_gettop(L);
		int arg2 = lua_tonumber(L,2);



		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);


		BranchTo(0x8219CD10,int,L);

		lua_newtable06(L);

		lua_pushlightuserdata(L,OBJPlayer->PlayerPath.get());
		lua_rawseti06(L,-2,1);

		lua_pushlightuserdata(L,OBJPlayer->PlayerPathGuide.get());
		lua_rawseti06(L,-2,2);

		lua_pushlightuserdata(L,OBJPlayer->PlayerPathCollision.get());
		lua_rawseti06(L,-2,3);

		


		return 1;

	}

	extern "C" int PlayerR__OpenLightDash(lua_State* L)
	{
		int arg_count = lua_gettop(L);



		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);


		BranchTo(0x821FAB88,int,L);


		lua_pushlightuserdata(L,OBJPlayer->FindPluginLast("path_ld").get()); //module
		return 1;

	}

	extern "C" int PlayerR__OpenScore(lua_State* L)
	{
		int arg_count = lua_gettop(L);

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);

		BranchTo(0x8219D030,int,L);

		
		lua_pushlightuserdata(L,OBJPlayer->PlayerScore.get());
		return 1;
	}

	extern "C" int PlayerR__OpenBodyList(lua_State* L)
	{
		int arg_count = lua_gettop(L);

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);

		BranchTo(0x8219D178,int,L);

		lua_pushlightuserdata(L,OBJPlayer->FindPluginLast("bodylist").get()); //module
		return 1;

	}

	extern "C" int PlayerR__OpenLockOn(lua_State* L)
	{
		int arg_count = lua_gettop(L);

		const char* arg2 = lua_tostring(L, 2);

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);

		BranchTo(0x8219D220,int,L);

		lua_pushlightuserdata(L,OBJPlayer->FindPluginLast(arg2).get()); //module
		return 1;
	}

	extern "C" int PlayerR__OpenGameMaster(lua_State* L)
	{
		int arg_count = lua_gettop(L);



		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);

		BranchTo(0x8219D378 ,int,L);

		lua_pushlightuserdata(L,OBJPlayer->PlayerGameMaster.get());
		return 1;
	}

	extern "C" int PlayerR__OpenEventer(lua_State* L)
	{
		int arg_count = lua_gettop(L);

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);

		BranchTo(0x8219D538 ,int,L);

		lua_pushlightuserdata(L,OBJPlayer->PlayerEventer.get());
		return 1;
	}

	extern "C" int PlayerR__OpenBody(lua_State* L)
	{
		int arg_count = lua_gettop(L);
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);
		BranchTo(0x8219E430,int,L);
		lua_pushlightuserdata(L,OBJPlayer->FindPluginLast("physicsbody").get()); //module
		return 1;
	}

	extern "C" int PlayerR__OpenAutomaticDead(lua_State* L)
	{
		return 0;
	}

	extern "C" int PlayerR__OpenWeapons(lua_State* L)
	{
		int arg_count = lua_gettop(L);
		int arg2 = lua_tonumber(L,2);

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);


		BranchTo(0x8219E670,int,L);

		const char* find_array[] = {
			"omega_weapons",
			"shadow_weapons",
			"sonic_weapons",
			"venice_weapons",
			"tails_weapons",
			"amy_weapons",
			"blaze_weapons",
			"blaze_weapons", //knuckles_weapons
			"blaze_weapons", //rouge_weapons
			"sonic_weapons", //princess_weapons
			"boss_silver_weapons",
			"",
			"supershadow_weapons",
			"venice_weapons"
		};


		lua_pushlightuserdata(L,OBJPlayer->FindPluginLast(find_array[arg2]).get()); //module



		return 1;


	}


	extern "C" int PlayerR__OpenModel(lua_State* L)
	{
		int arg_count = lua_gettop(L);
		int arg2 = lua_tonumber(L,2);




		lua_pushstring06(L,"Flags");
		lua_gettable(L,1);
		int Flags = (int)lua_touserdata(L,-1);

		Flags |= REOPEN_DYNLINK(REOPEN_MODEL);

		lua_pushstring06(L,"Flags");
		lua_pushlightuserdata(L,(void*)Flags);
		lua_settable06(L,1);


		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);


		BranchTo(0x8219BDF0,int,L);

		const char* find_array[] = {
			"model",
			"model",
			"model_eff",
			"model",
			"model",
			"model"
		};


		lua_pushlightuserdata(L,OBJPlayer->FindPluginLast(find_array[arg2]).get()); //module
		

	
		return 1;
	}

	extern "C" int PlayerR__OpenPostureControl(lua_State* L)
	{
		int arg_count = lua_gettop(L);
		int arg2 = lua_tonumber(L,2);



		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);


		BranchTo(0x8219C468,int,L);

	
		lua_pushlightuserdata(L,OBJPlayer->PlayerPosture.get()); //module
		return 1;


	}

	extern "C" int PlayerR__OpenGravity(lua_State* L)
	{
		int arg_count = lua_gettop(L);
	

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);


		BranchTo(0x8219C850,int,L);
		lua_pushlightuserdata(L,OBJPlayer->PlayerGravity.get());
		return 1;


	}

	extern "C" int PlayerR__OpenState(lua_State* L)
	{
		int arg_count = lua_gettop(L);


		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);


		BranchTo(0x8219C960,int,L);


		lua_pushlightuserdata(L,OBJPlayer->PlayerMachine.get());
		return 1;
	}

	extern "C" int PlayerR__OpenSound(lua_State* L)
	{
		lua_pushstring06(L,"Flags");
		lua_gettable(L,1);
		int Flags = (int)lua_touserdata(L,-1);
		Flags |= REOPEN_DYNLINK(REOPEN_SOUND);

		lua_pushstring06(L,"Flags");
		lua_pushlightuserdata(L,(void*)Flags);
		lua_settable06(L,1);


		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);


		BranchTo(0x8219DD88,int,L);
		lua_pushlightuserdata(L,OBJPlayer->FindPluginLast("sound").get()); //module


		return 1;

	}

	extern "C" int PlayerR__OpenEffect(lua_State* L)
	{
		lua_pushstring06(L,"Flags");
		lua_gettable(L,1);
		int Flags = (int)lua_touserdata(L,-1);
		Flags |= REOPEN_DYNLINK(REOPEN_EFFECT);

		lua_pushstring06(L,"Flags");
		lua_pushlightuserdata(L,(void*)Flags);
		lua_settable06(L,1);


		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);


		BranchTo(0x8219D5E8,int,L);

		lua_pushlightuserdata(L,OBJPlayer->FindPluginLast("effect").get()); //module

		return 1;

	}

	extern "C" int PlayerR__OpenOther(lua_State* L)
	{
	
		int arg_count = lua_gettop(L);

		int arg2 = lua_tonumber(L,2);



		lua_pushstring06(L,"Flags");
		lua_gettable(L,1);
		int Flags = (int)lua_touserdata(L,-1);
		Flags |= REOPEN_DYNLINK(REOPEN_OTHER);

		lua_pushstring06(L,"Flags");
		lua_pushlightuserdata(L,(void*)Flags);
		lua_settable06(L,1);



		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);




		BranchTo(0x8219E9F0,int,L);



		const char* find_array[] = {
			"homing",
			"chaos snap",
			"homing",
			"talk",
			"rodeo",
			"amigo change",
			"waterslider",
			"rodeo",
			"item",
			"homing"
		};


		lua_pushlightuserdata(L,OBJPlayer->FindPluginLast(find_array[arg2]).get()); //module




		return 1;

	}

	extern "C" int PlayerR__OpenGauge(lua_State* L)
	{
		int arg_count = lua_gettop(L);
		int arg2 = lua_tonumber(L,2);



		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		lua_pushlightuserdata(L,(void*)&OBJPlayer);
		lua_replace(L,1);


		BranchTo(0x8219EFB8,int,L);


		lua_pushlightuserdata(L,OBJPlayer->PlayerGauge.get());

		return 1;


	}

	extern "C" int PlayerR__OpenOtherParameter(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);
		
		Sonicteam::Player::OtherParameter* OtherParameter = BranchTo(0x82220FB8,Sonicteam::Player::OtherParameter*,malloc(0x4C),OBJPlayer->Cameraman);
		OtherParameter->ObjectPlayerActorID = OBJPlayer->ActorID;
		OtherParameter->ActorManager = OBJPlayer->GameImp.lock()->GetActorManager();
		OtherParameter->WorldHavok = OBJPlayer->GameImp.lock()->GetWorldHavok();
		OtherParameter->ObjectPlayerRootFrame = OBJPlayer->RootFrame;
		OtherParameter->DocMarathon = (Sonicteam::DocMarathonImp*)OBJPlayer->TaskEngineDoc;
		OtherParameter->ObjectPlayerComboAttackManager = OBJPlayer->PlayerComboAttackManager;

	
		boost::shared_ptr<Sonicteam::Player::OtherParameter> ptr(OtherParameter);
		OBJPlayer->PlayerPlugins.push_back(ptr);

		


		lua_pushstring06(L,"Flags");
		lua_gettable(L,1);
		int Flags = (int)lua_touserdata(L,-1);
		Flags |= REOPEN_DYNLINK(REOPEN_OPENOTHERPARAMETER);
		lua_pushstring06(L,"Flags");
		lua_pushlightuserdata(L,(void*)Flags);
		lua_settable06(L,1);


		lua_pushlightuserdata(L,dynamic_cast<Sonicteam::Player::IPlugIn*>(OtherParameter));



		return 1;
	}

	extern "C" int PlayerR__IDynamicLink(lua_State* L)
	{

		return 0;
	}

	

	extern "C" int PlayerR__IVarible(lua_State* L) {
		int args = lua_gettop(L);
		lua_pushstring(L, "ptr");
		lua_gettable(L, 1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L, -1);

		REF_TYPE(Sonicteam::LuaSystem) p;
		p = BranchTo(0x821EA260, REF_TYPE(Sonicteam::LuaSystem), &std::string(lua_tostring(L, 2)), 0x82003380, 0x1D);

		for (int i = 3; i <= args; i++) { // Changed <args to <= args
			for (std::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>::iterator it =   OBJPlayer->PlayerPlugins.begin();it != OBJPlayer->PlayerPlugins.end();it++) { // Use range-based for loop for clarity
	
				if (lua_isstring(L, i)) {
					const char* pluginName = lua_tostring(L, i);
					if ((*it)->PluginName == pluginName) {
						Sonicteam::Player::IVariable* variable = dynamic_cast<Sonicteam::Player::IVariable*>((*it).get());
						if (variable) { // Check if the cast was successful
							variable->OnVarible(&p.param);
						}
					}
				} else if (lua_isuserdata(L, i)) {
					Sonicteam::Player::IPlugIn* userdataPlugin = static_cast<Sonicteam::Player::IPlugIn*>(lua_touserdata(L, i));
					if ((*it).get() == userdataPlugin) {
						Sonicteam::Player::IVariable* variable = dynamic_cast<Sonicteam::Player::IVariable*>((*it).get());
						if (variable) { // Check if the cast was successful
							variable->OnVarible(&p.param);
						}
					}
				}
			}
		}

		return 0;
	}




	extern "C" int PlayerR__IDynamicLink2(lua_State* L)
	{
		int args = lua_gettop(L);

		lua_pushstring(L, "ptr");
		lua_gettable(L, 1);

		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L, -1);
		lua_pop(L, 1); // Remove the userdata from the stack

		for (int i = 2; i <= args; ++i)
		{
			boost::shared_ptr<Sonicteam::Player::IPlugIn> find_plugin;

			//Find Plugin by Name or Userdata
			for (std::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>::iterator it = OBJPlayer->PlayerPlugins.begin();
				it != OBJPlayer->PlayerPlugins.end(); ++it)
			{
				if (!(*it)) continue; // Safe check for null plugin

				if (lua_isstring(L, i))
				{
					const char* plugin_name = lua_tostring(L, i);
					if (plugin_name && (*it)->PluginName == plugin_name) // Add null check for lua_tostring and PluginName
					{
						find_plugin = *it;
						break;
					}
				}
				else if (lua_isuserdata(L, i))
				{
					Sonicteam::Player::IPlugIn* plugin_ptr = (Sonicteam::Player::IPlugIn*)lua_touserdata(L, i);
					if (plugin_ptr && (*it).get() == plugin_ptr) //Safe check
					{
						find_plugin = *it;
						break;
					}
				}
			}


			// Find and Link IDynamicLinks based on userdata
			for (std::vector<boost::shared_ptr<Sonicteam::Player::IDynamicLink>>::iterator it = OBJPlayer->PlayerIDynamicLink.begin();
				it != OBJPlayer->PlayerIDynamicLink.end(); ++it)
			{
				if (!(*it)) continue; //Safe check

				if (lua_isuserdata(L, i))
				{
					Sonicteam::Player::IDynamicLink* dynamic_link_ptr = (Sonicteam::Player::IDynamicLink*)lua_touserdata(L, i);
					if (dynamic_link_ptr && (*it).get() == dynamic_link_ptr)
					{
						for (std::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>::iterator jt = OBJPlayer->PlayerPlugins.begin();
							jt != OBJPlayer->PlayerPlugins.end(); ++jt)
						{
							if (*jt) //Safe check
								(*it)->OnLink(*jt);
						}
						break; // Found the IDynamicLink, no need to continue searching
					}
					/*
					Sonicteam::Player::IDynamicLink* dynamic_posture_ptr = dynamic_cast<Sonicteam::Player::IDynamicLink*>((Sonicteam::Player::IPostureControl*)lua_touserdata(L, i));
					if (dynamic_link_ptr && (*it).get() == dynamic_posture_ptr)
					{
						for (std::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>::iterator jt = OBJPlayer->PlayerPlugins.begin();
							jt != OBJPlayer->PlayerPlugins.end(); ++jt)
						{
							if (*jt) //Safe check
								(*it)->OnLink(*jt);
						}
						break; // Found the IDynamicLink, no need to continue searching
					}
					*/


				}
			}



			if (find_plugin.get()){
				if (OBJPlayer->PlayerEventer) // Null check for PlayerEventer
					OBJPlayer->PlayerEventer->OnLink(find_plugin);

				for (std::vector<boost::shared_ptr<Sonicteam::Player::IDynamicLink>>::iterator it = OBJPlayer->PlayerIDynamicLink.begin();
					it != OBJPlayer->PlayerIDynamicLink.end(); ++it)
				{
					if (*it) //Safe check
						(*it)->OnLink(find_plugin);
				}

				boost::shared_ptr<Sonicteam::Player::IDynamicLink> variable = boost::dynamic_pointer_cast<Sonicteam::Player::IDynamicLink>(find_plugin);
				if (variable)
				{
					for (std::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>::iterator it = OBJPlayer->PlayerPlugins.begin();
						it != OBJPlayer->PlayerPlugins.end(); ++it)
					{
						if (*it) //Safe check
							variable->OnLink(*it);
					}
				}
			}
		}

		return 0;
	}

	extern "C" int PlayerR__IEventerListener(lua_State* L)
	{

		return 0;
	}

	extern "C" int PlayerR__RemovePlugin(lua_State* L)
	{

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::Player::ObjectPlayer* OBJPlayer = (Sonicteam::Player::ObjectPlayer*)lua_touserdata(L,-1);


		const char* plug =  lua_tostring(L,2);

		if (strcmp(plug,"Eventer") == 0){
			OBJPlayer->RemovePlugin(OBJPlayer->PlayerEventer);
		}





		OBJPlayer->RemovePlugin(plug);


		return 0;
	}


}
