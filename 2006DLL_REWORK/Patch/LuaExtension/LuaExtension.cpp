#include "LuaExtension.h"

namespace DebugLogV2{

	




	int DebugLabel_GlobalInstall(lua_State* LS)
	{

		lua_register06(LS,"DebugLabel",DebugLabel_new);
		luaL_newmetatable06(LS,"DebugLabel");



		lua_pushstring06(LS, "__gc"); lua_pushcfunction06(LS, DebugLabel_delete); lua_settable06(LS, -3);

		lua_pushvalue(LS, -1);  lua_pushstring06(LS, "__index"); lua_pushvalue(LS,-2);  lua_settable06(LS,-3);

		lua_pushstring06(LS, "SetText"); lua_pushcfunction06(LS, DebugLabel_SetText); 	lua_settable06(LS, -3);

		lua_pushstring06(LS, "SetPosition"); lua_pushcfunction06(LS, DebugLabel_SetPosition); 	lua_settable06(LS, -3);

		lua_pop(LS,1);

		return 1;
	}

	DebugLabel_new(lua_State* L)
	{

		const char* msg = lua_tostring(L,1);
		int PX = lua_tonumber(L,2);
		int PY = lua_tonumber(L,3);


		std::string msgt =  std::string(msg);
		int length = msgt.length() + 1;
		wchar_t* wcharPtr = new wchar_t[length];
		std::memset(wcharPtr, 0, length * sizeof(wchar_t));
		std::mbstowcs(wcharPtr, msgt.c_str(), length);
		msgt.clear();


		*reinterpret_cast<int**>(lua_newuserdata06(L, sizeof(int*))) = (int*)SpawnMessage(wcharPtr,PX,PY);

		luaL_getmetatable06(L,"DebugLabel");
		lua_setmetatable06(L, -2);

		free(wcharPtr);

		return 1;

	}

	int DebugLabel_delete(lua_State* L)
	{
#ifdef ALTERNATIVE_UTIL_MSG
		DMSG_UI* obj  = *(DMSG_UI**)lua_touserdata(L, 1);
		obj->Release();
#else
	BranchTo(0x8262BA68,int,*reinterpret_cast<int**>(lua_touserdata(L, 1)),1); //Destroy from mem
#endif

	

		return 0;
	}

	int DebugLabel_SetText(lua_State* L)
	{
		int* TextEntity = (*reinterpret_cast<int**>(luaL_checkudata(L, 1, "DebugLabel")));
		UINT ud = (UINT)TextEntity;

		const char* msg = lua_tostring(L,2);
		std::string msgt =  std::string(msg);
		int length = msgt.length() + 1;
		wchar_t* wcharPtr = new wchar_t[length];
		std::memset(wcharPtr, 0, length * sizeof(wchar_t));
		std::mbstowcs(wcharPtr, msgt.c_str(), length);
		msgt.clear();
		EditMessage((UINT32)ud,wcharPtr);
		free(wcharPtr);
		return 0;
	}

	int DebugLabel_SetPosition(lua_State* L)
	{
		int* TextEntity = (*reinterpret_cast<int**>(luaL_checkudata(L, 1, "DebugLabel")));
		UINT ud = (UINT)TextEntity;
		float x  = lua_tonumber(L,2);
		float y = lua_tonumber(L,3);
		ChangeMessagePosition(ud,x,y);
		return 0;
	}

	
	


	static const struct luaL_reg PET [] = {
		{"GetPlayerInput", GetPlayerInput},
		{"GetPlayerRawInput", GetPlayerRawInput},
		{"GetPlayerPosition",GetPlayerPosition},
		{"SetPlayerPosition",SetPlayerPosition},
		{"print",PrintNext},
		{NULL, NULL}  /* sentinel */
	};





	struct ObjectSetParamData{

	};

	struct ObjectSetData{

		ObjectSetData(const char* ObjectName,const char* ObjectTypeName,XMVECTOR* Position,XMVECTOR* Rotation){
			this->ObjectName = ObjectName;
			this->ObjectTypeName = ObjectTypeName;
			this->Position.x = Position->x;
			this->Position.y = Position->y;
			this->Position.z = Position->z;

			this->ObjectUnkFloatArray [0] = 0x40;
			this->ObjectUnkFloatArray [1] = 0x0;
			this->ObjectUnkFloatArray [2] = 0x0;
			this->StartInactive = 0;


			this->ObjectUnkFloat2 = 0;
			this->ObjectUnkFloat3 = 0;
			this->ObjectUnkFloat4 = 0;
			this->ObjectUnkFloat5 = 40000;


			this->Rotation.x = Rotation->x;
			this->Rotation.y = Rotation->y;
			this->Rotation.z = Rotation->z;
			this->Rotation.w = Rotation->w;

			this->ParamsCount = 0;
			
	/*

			this->ObjectUnkFloat6 = 0;
			this->ObjectUnkFloat7 = 0;
			this->ObjectUnkFloat8 = 2;
			*/

		}
	
		const char* ObjectName;
		const char* ObjectTypeName;
		//BELL INFO
		char ObjectUnkFloatArray[3]; // 40 00 00
		char StartInactive; //0

		float ObjectUnkFloat2; //=0
		float ObjectUnkFloat3; //=0
		float ObjectUnkFloat4; //=0
		XMFLOAT3 Position;
		float ObjectUnkFloat5; //40000
		XMFLOAT4 Rotation;

		unsigned int ParamsCount;
		ObjectSetParamData* Params;

	};


	 enum ObjectDataType
	{
		Boolean,
		Int32,
		Single,
		String,
		Vector3,
		UInt32 = 6
	};


	struct dummy_container_entityhandle_Object{
		int V1;
		int V2;
		int EntityHandle;
		int EntityObject;

		dummy_container_entityhandle_Object(int EntityHandle,int EntityObject){
			this->V1 = 4;
			this->V2 = 0;
			this->EntityObject = EntityObject;
			this->EntityHandle = EntityHandle;
		}


	 };

	extern "C" int GameLIB_DocGamemodeGetActorManager(){

		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		UINT32 vft =  *(UINT32*)impl->DocCurrentMode;

		if (vft == 0x82033534){ //GameMode

			UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);
			UINT32 ActorManager = *(UINT32*)(gameimp+0x11C4);
			return ActorManager;
		}
		return 0;
	}


	extern "C" UINT32* GameLIB_PlayerObjGetActorPTR(int ObjPlayer){

		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		UINT32 vft =  *(UINT32*)impl->DocCurrentMode;

		if (vft == 0x82033534){ //GameMode

			UINT32 ActorManager = GameLIB_DocGamemodeGetActorManager();
			UINT32 ActorMangerActorsCount = *(UINT32*)(ActorManager+0x80000);
			for (int i = 0;i<ActorMangerActorsCount;i++)
			{
				UINT32 Actor = *(UINT32*)(ActorManager+0x40000+(i*4));
				UINT32 ActorID = *(UINT32*)(ActorManager+0x4 +(0x4*i));
				if (Actor == ObjPlayer){
					return (UINT32*)(ActorManager+0x40000+(i*4)) ;
				}
			}

		}
		return 0;

	}

	extern "C" int GameLIB_PlayerObjSetActorID(int ObjPlayer,int ID){

		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		UINT32 vft =  *(UINT32*)impl->DocCurrentMode;

		if (vft == 0x82033534){ //GameMode


			UINT32 ActorManager = GameLIB_DocGamemodeGetActorManager();
			UINT32 ActorMangerActorsCount = *(UINT32*)(ActorManager+0x80000);
			for (int i = 0;i<ActorMangerActorsCount;i++)
			{
				UINT32 Actor = *(UINT32*)(ActorManager+0x40000+(i*4));
				UINT32 ActorID = *(UINT32*)(ActorManager+0x4 +(0x4*i));
				if (Actor == ObjPlayer){
					*(UINT32*)(ActorManager+0x4 +(0x4*i)) = ID;
					return 0;
				}

			}
		}
		return -1;
	}

	extern "C" int GameLIB_PlayerObjToActorID(int ObjPlayer){

		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		UINT32 vft =  *(UINT32*)impl->DocCurrentMode;

		if (vft == 0x82033534){ //GameMode


			UINT32 ActorManager = GameLIB_DocGamemodeGetActorManager();
			UINT32 ActorMangerActorsCount = *(UINT32*)(ActorManager+0x80000);
			for (int i = 0;i<ActorMangerActorsCount;i++)
			{
				UINT32 Actor = *(UINT32*)(ActorManager+0x40000+(i*4));
				UINT32 ActorID = *(UINT32*)(ActorManager+0x4 +(0x4*i));
				if (Actor == ObjPlayer){
					return ActorID;
				}

			}
		}
		return -1;
	}

	extern "C" GameLIB_PreloadPlayer(lua_State* L){

		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		Sonicteam::GameImp* gameimp = *(Sonicteam::GameImp**)(impl->DocCurrentMode + 0x6C);

		Sonicteam::SoX::Message message =  Sonicteam::SoX::Message(0x15016,0xD,0);
		gameimp->OnMessageRecieved(&message);

		return 0;
	}
	extern "C" GameLIB_LoadEvent(lua_State* L){
		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		Sonicteam::GameImp* gameimp = *(Sonicteam::GameImp**)(impl->DocCurrentMode + 0x6C);

	//	Sonicteam::SoX::Message message =  Sonicteam::SoX::Message(0x15032,(int)lua_tostring(L,1),(int)lua_tostring(L,1));
	//	gameimp->OnMessageRecieved(&message);


		
	
	//	gameimp->OnMessageRecieved(&Sonicteam::SoX::Message(0x1502F,0x0,0));



		return 0;
	}

	extern "C" GameLIB_PlayerIndexToActorIDRestore(lua_State* L){

		int args = lua_gettop(L);

		int index = 0;
		if (args > 1){
			index = lua_tonumber(L,1);
		}


		
		UINT32 pstack[0x10];
		GetPlayerActors((UINT32*)&pstack,index);

		if (pstack[index] != 0){
			lua_pushnumber(L,GameLIB_PlayerObjToActorID(pstack[index]));
		
		}
		else{
			lua_pushnumber(L,-1);
		}

		return 1;


	}
	extern "C" GameLIB_NewActorRestore(lua_State* L){
	
	
		int atgs = lua_gettop(L);
		XMVECTOR Pos = {0};
		XMVECTOR Rot = {0};
		const char* Placement = "default";
		bool _Save_ = false;
		bool _Preload_ = false;
		int PlacementIndex = 0;
		if (lua_istable(L,3) &&  atgs > 2){

			lua_pushvalue06(L,3);
			lua_pushstring06(L,"Position");
			lua_gettable(L,-2); 
			Pos = Vector__GetVectorTable(L,-1);
			lua_pop(L,2);



			lua_pushvalue06(L,3);
			lua_pushstring06(L,"Rotation");
			lua_gettable(L,-2); 
			Rot = Vector__GetVectorTable(L,-1);
			lua_pop(L,2);

			lua_pushvalue06(L,3);
			lua_pushstring06(L,"Placement");
			lua_gettable(L,-2); 

			if (lua_isstring(L,-1)){
				Placement = lua_tostring(L,-1);
			}
			lua_pop(L,2);


			lua_pushvalue06(L,3);
			lua_pushstring06(L,"Save");
			lua_gettable(L,-2); 

			if (lua_isboolean(L,-1)){
				_Save_ = lua_toboolean(L,-1);
			}

			lua_pop(L,2);


			lua_pushvalue06(L,3);
			lua_pushstring06(L,"Preload");
			lua_gettable(L,-2); 

			if (lua_isboolean(L,-1)){
				_Preload_ = lua_toboolean(L,-1);
			}
			
			lua_pop(L,2);



		}
		const char* PlacementTypeTable[] = {"default","design","particle","test","area","pathobj"};
		for (int i = 0;i<6;i++){
			if (strcmp(PlacementTypeTable[i],Placement) == 0){
				PlacementIndex = i;
				break;;
			}
		}










		std::map<std::string, boost::any> _params;

		if (lua_istable(L,2) && atgs > 1) {
			lua_pushnil(L);
			while (lua_next(L, 2) != 0) {
				if (lua_isstring(L, -2)) {
					const char* keyF = lua_tostring(L, -2);
					std::string key = keyF;
					int valueType = lua_type(L, -1);
					switch (valueType) {
			case LUA_TBOOLEAN:
				_params[key] = static_cast<bool>(lua_toboolean(L, -1));
				break;
			case LUA_TNUMBER:
				_params[key] = static_cast<float>(lua_tonumber(L, -1));
				break;
			case LUA_TSTRING:
				_params[key] = std::string(lua_tostring(L, -1));
				break;
					}
				}

				// Pop the value, keep the key for the next iteration
				lua_pop(L, 1);
			}

			// Assuming Lua state is initialized and used in the code
			lua_pop(L, 1);
		}



		const char* OBJ_ID =  lua_tostring(L,1);
		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		Sonicteam::GameImp* gameimp = *(Sonicteam::GameImp**)(impl->DocCurrentMode + 0x6C);
		Sonicteam::Prop::Manager* GamePropManager =   gameimp->GamePropManager.get();
		Sonicteam::Prop::ClassRegistry* PropSceneClass = GamePropManager->PropClassRegistry.get();


		if (gameimp->GamePropActorCreators.get()->ActorCreator.find(OBJ_ID) == gameimp->GamePropActorCreators.get()->ActorCreator.end()){
			lua_pushnil(L);
			return 1;
		}

		Sonicteam::Prop::Class* RefObjectTypePropClass = PropSceneClass->_registry_[std::string(OBJ_ID)];

	
		std::map<std::string,int> _props_origin_params;


		std::vector<std::pair<std::string,boost::any>> _sorted_params;

		Sonicteam::Prop::ClassPropParamData* PropData =   RefObjectTypePropClass->ClassPropData;
		int PropParamsCount =  PropData->ClassParamInfoCount;
		Sonicteam::Prop::ClassPropParamInfo* PropParamsDataPTR =  PropData->ClassParamInfo;



		for (int i = 0;i<PropParamsCount;i++){
			Sonicteam::Prop::ClassPropParamInfo* ParameterDataPTR = &PropParamsDataPTR[i];
			const char* ParameterName = (const char*)&ParameterDataPTR->ParamName;
			int ParameterType = ParameterDataPTR->ParamType;

			std::string key = std::string(ParameterName);
			_props_origin_params[key] = ParameterType;

			if (_params.find(key) ==  _params.end()){

				switch (ParameterType){
				case Boolean:
					_params[key] = boost::any(false);
					break;
				case Int32:
					_params[key] = boost::any(0.0f);
					break;
				case Single:
					_params[key] = boost::any(0.0f);
					break;
				case String:
					_params[key] = std::string("");
					break;
				case UInt32:
					_params[key] = boost::any(0.0f);
					break;
				default:
					break;

				}

			}



			_sorted_params.push_back(std::make_pair(std::string(ParameterName),_params[std::string(ParameterName)]));
		}




		///


		Sonicteam::Prop::Scene* PropScenePTR = gameimp->GamePropScene[PlacementIndex].get();
		int LastIndex = PropScenePTR->ScenePropInstance.size();
		char bufferX[512];
		sprintf(bufferX,"%s%d",OBJ_ID,LastIndex); //dashpanel 101


	
		Sonicteam::Prop::InstanceSetData* ObjData =  new Sonicteam::Prop::InstanceSetData(bufferX, OBJ_ID, Pos, Rot);
		ObjData->ParamsCount = PropParamsCount;
	




		ObjData->Params = (Sonicteam::Prop::InstanceSetDataParams*)malloc06(0x28 * ObjData->ParamsCount);
		memset(ObjData->Params,0,0x28 * ObjData->ParamsCount);
		Sonicteam::Prop::InstanceSetDataParams* prm = ObjData->Params;


		for (int i = 0;i<PropParamsCount;i++){
			int IPrm = (int)&prm[i];
			int ParamType = _props_origin_params[_sorted_params[i].first];
			*(unsigned int*)IPrm = ParamType;
			const char* s = 0;
			std::string STRX;
			//	ShowXenonMessage(L"MSG",it->first.c_str());




			switch (ParamType){
				case Boolean:
					*(int*)(IPrm + 0x4) = 	(int)boost::any_cast<bool>(_sorted_params[i].second);
					break;
				case Int32:
					*(int*)(IPrm + 0x4) = 	(int)boost::any_cast<float>(_sorted_params[i].second);
					//ShowXenonMessage(L"MSG",(int)boost::any_cast<float>(it->second),0);
					break;
				case Single:
					*(float*)(IPrm + 0x4) = (float)boost::any_cast<float>(_sorted_params[i].second);
					//ShowXenonMessage(L"MSG",(float)boost::any_cast<float>(it->second),0);
					break;
					//TODO : Free (mem later)
				case String:
					if (boost::any_cast<std::string>(_sorted_params[i].second).size() > 0)
					{
						STRX = boost::any_cast<std::string>(_sorted_params[i].second);
						const char* s = (const char*)malloc06(STRX.length()+1);
						memset((void*)s,0,STRX.length()+1);
						memcpy((void*)s,STRX.c_str(),STRX.size());
						*(const char**)(IPrm + 0x4) = s;
					}
					//	ShowXenonMessage(L"MSG",boost::any_cast<std::string>(it->second)->
					break;
				case UInt32:
					//ShowXenonMessage(L"MSG",(int)boost::any_cast<float>(it->second),0);
					*(unsigned int*)(IPrm + 0x4) = (unsigned int)boost::any_cast<float>(_sorted_params[i].second);
					//	ShowXenonMessage(L"MSG",(int)boost::any_cast<float>(it->second),0);
					break;
				default:
					break;

			}




		}



		REF_TYPE(Sonicteam::SoX::RefCountObject) obj = 0;
		REF_TYPE(Sonicteam::SoX::RefCountObject) params = 0;


		if (strcmp(OBJ_ID,"objectphysics") == 0 && _Preload_){
			std::string object = boost::any_cast<std::string>(_params["objectName"]);

			REF_TYPE(Sonicteam::SoX::RefCountObject) obj = BranchTo(0x8227F438,REF_TYPE(Sonicteam::SoX::RefCountObject),gameimp);
			REF_TYPE(Sonicteam::SoX::RefCountObject) params =  BranchTo(0x8227F198,REF_TYPE(Sonicteam::SoX::RefCountObject),obj.get(),object.c_str()); //now i need keep that object
			BranchTo(0X8228DF40,int,params,0);

		
		
		}





		
// BranchTo(0x8245A080,Sonicteam::Prop::Instance*,malloc06(0x14),PropScenePTR,ObjData,&RefObjectTypePropClass);
		


	//	Sonicteam::Prop::EntityHandle* EntityHandle = BranchTo(0x82461128,Sonicteam::Prop::EntityHandle*,malloc06(0x1c),PropScenePTR,LastIndex);

		Sonicteam::Prop::Instance* InstnceProp = new Sonicteam::Prop::Instance(PropScenePTR,ObjData,REF_TYPE(Sonicteam::Prop::Class)(RefObjectTypePropClass));
		Sonicteam::Prop::EntityHandle* EntityHandle = new Sonicteam::Prop::EntityHandle(PropScenePTR,LastIndex);
		EntityHandle->PropInstance = REF_TYPE(Sonicteam::Prop::Instance)(InstnceProp);
		Sonicteam::Prop::ActorCreatorCreationData buffer = Sonicteam::Prop::ActorCreatorCreationData(REF_TYPE(Sonicteam::Prop::Instance)(InstnceProp),REF_TYPE(Sonicteam::Prop::EntityHandle)(EntityHandle),0,std::string(InstnceProp->InstanceClass.get()->ClassPropData->ClassName));
		
		
		if (_Save_){
		
			PropScenePTR->ScenePropInstance.push_back(REF_TYPE(Sonicteam::Prop::Instance)(InstnceProp));
			PropScenePTR->ScenePlacament[ObjData->ObjectName] = LastIndex;


			std::vector<Sonicteam::SoX::Scenery::SPAabbNodeVector> vector_test = std::vector<Sonicteam::SoX::Scenery::SPAabbNodeVector>(LastIndex + 1);
			for (int i = 0;i<vector_test.size();i++){

				Sonicteam::Prop::InstanceSetData* in = PropScenePTR->ScenePropInstance[i].get()->InstanceSetData;

				vector_test[i].minX= in->Position.x - in->DrawDistance;
				vector_test[i].minY= in->Position.y - in->DrawDistance;
				vector_test[i].minZ= in->Position.z - in->DrawDistance;

				vector_test[i].maxX= in->Position.x + in->DrawDistance;
				vector_test[i].maxY= in->Position.y + in->DrawDistance;
				vector_test[i].maxZ= in->Position.z + in->DrawDistance;
			}


			PropScenePTR->PropSceneWorld.get()->SceneryAabbTree.get()->SPAabbTreeInitialize(PropScenePTR->PropSceneWorld.get(),&vector_test[0],LastIndex + 1);


			Sonicteam::Prop::SceneActor sactor;
			sactor.Flag1 = 0x40000000;
			sactor.Flag2 = 4;
			sactor.ObjActor = 0;
			sactor.ObjActorHandle = 0;
			PropScenePTR->SceneObject.push_back(sactor);
		
		}

		

		Sonicteam::Prop::Manager* PropManger = PropScenePTR->PropManager;

		
		Sonicteam::Prop::ActorCreators* ActorCreatorsVar =  PropManger->ActorCreators.lock().get();
		Sonicteam::Actor* obj_actor =  ActorCreatorsVar->ActorCreator[std::string(OBJ_ID)]->CreateActor(ActorCreatorsVar->NamedActor,&ActorCreatorsVar->GameImp,&buffer);
	


	
	
		//EntityHandleAndObjectVector->push_back(dummy_container_entityhandle_Object(EntityHandle,result));
		//InstancePropVector->push_back(InstnceProp);


		lua_pushlightuserdata(L,obj_actor);


		return 1;
	}

	int GameLIB_GlobalInstall(lua_State* LS)
	{	
		//WRITE_DWORD(0x8228E3B0,0x60000000);
		WRITE_DWORD(0x82026A04,GameLIB_NewActorRestore);
		WRITE_DWORD(0x820269C4,GameLIB_PlayerIndexToActorIDRestore);
		//WRITE_DWORD(0x820269D4,GameLIB_PreloadPlayer);
		//WRITE_DWORD(0x82026A0C,GameLIB_LoadEvent);
		return 0;
	}


	int PlayerLIB_GlobalInstall(lua_State* LS){
		luaL_openlib06(LS,"player",PET,0);
		return 1;
	}
	// PlayerCamera
	int PlayerLIB_GlobalInstallOLD(lua_State* LS)
	{

		luaL_openlib06(LS,"player",PET,0);


		lua_register06(LS,"Player",Player_NEW);
		luaL_newmetatable06(LS,"Player");


		lua_pushstring06(LS, "__gc"); lua_pushcfunction06(LS, Player___GC); lua_settable06(LS, -3);
		lua_pushvalue(LS, -1);  lua_pushstring06(LS, "__index"); lua_pushvalue(LS,-2);  lua_settable06(LS,-3);

		lua_pushstring06(LS, "GetPTR"); lua_pushcfunction06(LS, Player_GetPTR); 	lua_settable06(LS, -3);

		lua_pushstring06(LS, "Reload"); lua_pushcfunction06(LS, Player_RELOAD); 	lua_settable06(LS, -3);

		lua_pushstring06(LS, "Swap"); lua_pushcfunction06(LS, Player_SWAP); 	lua_settable06(LS, -3);

		lua_pushstring06(LS, "GetName"); lua_pushcfunction06(LS, Player__GetName); 	lua_settable06(LS, -3);


		lua_pushstring06(LS, "GetCamera"); lua_pushcfunction06(LS, Player__GetCamera); 	lua_settable06(LS, -3);

		lua_pushstring06(LS, "GetPosition"); lua_pushcfunction06(LS, Player__GetPosition); 	lua_settable06(LS, -3);
		lua_pushstring06(LS, "SetPosition"); lua_pushcfunction06(LS, Player__SetPosition); 	lua_settable06(LS, -3);

		lua_pushstring06(LS, "GetRotation"); lua_pushcfunction06(LS, Player__GetRotation); 	lua_settable06(LS, -3);
		lua_pushstring06(LS, "SetRotation"); lua_pushcfunction06(LS, Player__SetRotation); 	lua_settable06(LS, -3);


		lua_pushstring06(LS, "GetStateID"); lua_pushcfunction06(LS, Player__GetCurrentStateID); 	lua_settable06(LS, -3);
		lua_pushstring06(LS, "SetStateID"); lua_pushcfunction06(LS, Player__SetCurrentStateID); 	lua_settable06(LS, -3);



		lua_pushstring06(LS, "GetActorID"); lua_pushcfunction06(LS, Player__GetActorID); 	lua_settable06(LS, -3);
		lua_pushstring06(LS, "SetActorID"); lua_pushcfunction06(LS, Player__SetActorID); 	lua_settable06(LS, -3);
		lua_pushstring06(LS, "SetActorPTR"); lua_pushcfunction06(LS, Player__SetActorPTR); 	lua_settable06(LS, -3);



		lua_pushstring06(LS, "OpenPostureControl"); lua_pushcfunction06(LS, Player_OpenPostureControl); 	lua_settable06(LS, -3);
		lua_pushstring06(LS, "OpenInput"); lua_pushcfunction06(LS, Player_OpenInput); 	lua_settable06(LS, -3);
		lua_pushstring06(LS, "OpenPackage"); lua_pushcfunction06(LS, Player_OpenPackage); 	lua_settable06(LS, -3);
		lua_pushstring06(LS, "OpenModel"); lua_pushcfunction06(LS, Player_OpenModel); 	lua_settable06(LS, -3);
		lua_pushstring06(LS, "OpenFrame"); lua_pushcfunction06(LS, Player_OpenFrame); 	lua_settable06(LS, -3);
		lua_pushstring06(LS, "ReloadContext"); lua_pushcfunction06(LS, Player_ReloadContext); 	lua_settable06(LS, -3);
		lua_pushstring06(LS, "ReloadSound"); lua_pushcfunction06(LS, Player_ReloadSound); 	lua_settable06(LS, -3);

		lua_pop(LS,1);


		//lua_register06(LS,"Player",Player_NEW);
		luaL_newmetatable06(LS,"Cameraman");
		lua_register06(LS,"Cameraman",Camera__NEW);


	//	lua_pushstring06(LS, "__gc"); lua_pushcfunction06(LS, Player___GC); lua_settable06(LS, -3);
		lua_pushvalue(LS, -1);  lua_pushstring06(LS, "__index"); lua_pushvalue(LS,-2);  lua_settable06(LS,-3);

		lua_pushstring06(LS, "GetPTR"); lua_pushcfunction06(LS, Camera_GetPTR); 	lua_settable06(LS, -3);
		lua_pushstring06(LS, "GetPosition"); lua_pushcfunction06(LS, Camera_GetPosition); 	lua_settable06(LS, -3);
		lua_pushstring06(LS, "GetRotation"); lua_pushcfunction06(LS, Camera_GetRotation); 	lua_settable06(LS, -3);



		lua_pop(LS,1);



		return 1;

	

		



		lua_pop(LS,1);



		return 0;
	}

	
	// Free MyObject instance by Lua garbage collection
	static int Player___GC(lua_State* L){
//		delete *reinterpret_cast<Player_NEWS**>(lua_touserdata(L, 1));
		return 0;
	}
	extern "C" Player_RELOAD(lua_State* L){
	
		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		UINT32 pstack[16];
		GetPlayerActors((UINT32*)&pstack,PE->IsAI);
		PE->ObjectPlayer = (int*) pstack[PE->Index];
		PE->ObjectPlayerActorPTR = GameLIB_PlayerObjGetActorPTR((int)PE->ObjectPlayer);


		return 0;
	}

	extern "C" Player_GetPTR(lua_State* L)
	{


		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		lua_pushlightuserdata(L,(void*)PE->ObjectPlayer);;

		return 1;

	}



	extern "C" Player_NEW(lua_State* L)
	{


		int args = lua_gettop(L);
		
		Player_NEWS* z = (Player_NEWS*)malloc06(sizeof(Player_NEWS));
		if (args > 0){
			z->Index = lua_tonumber(L,1);
		}
		

		UINT32 pstack[16];

		if (lua_isboolean(L,2))
			z->IsAI  = lua_toboolean(L,2);
		else{
			z->IsAI = false;
		}


		GetPlayerActors((UINT32*)&pstack,z->IsAI);
		z->ObjectPlayer = (int*) pstack[z->Index];


	

	
		*reinterpret_cast<Player_NEWS**>(lua_newuserdata06(L, sizeof(Player_NEWS*))) = z;

		luaL_getmetatable06(L,"Player");
		lua_setmetatable06(L, -2);
		

		

		return 1;

	}



	extern "C" Player__GetCamera(lua_State* L)
	{
		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));

	
		int OBJPlayer = (int)PE->ObjectPlayer;

		int PlayerCAM = (OBJPlayer+ 0x94);

		if (PlayerCAM > 0x94){
			PlayerCAM  = *(int*)PlayerCAM;
		}
		else{
			PlayerCAM = 0;
		}




		lua_newtable06(L);

		lua_pushstring06(L,"ptr");
		lua_pushlightuserdata(L,(void*)(PlayerCAM-0x20));
		lua_settable06(L,-3);

		lua_pushstring06(L,"index");
		lua_pushnumber(L,PE->Index);
		lua_settable06(L,-3);



		luaL_getmetatable06(L,"Cameraman");
		lua_setmetatable06(L, -2);


		return 1;



	}

	extern "C" Player_SWAP(lua_State* L){

		int args = lua_gettop(L);
		if (args <= 1) return 0;

		const char* chara = lua_tostring(L,2);

		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		//PE->ObjectPlayer
			
		BranchTo(0x82195D20,int,PE->ObjectPlayer,chara);
		return 0;


	}




	
	extern "C" Player__GetName(lua_State* L)
	{
		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		int OBJPlayer = (int)PE->ObjectPlayer;
 
		std::string* c_player_name =  (std::string*)(OBJPlayer + 0x1D8);

		lua_pushstring06(L,	c_player_name->c_str());
	
		return 1;



	}



	extern "C" Player__GetPosition(lua_State* L)
	{
		
		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		int OBJPlayer = (int)PE->ObjectPlayer;
		XMVECTOR vector = {0};
		UINT32 PlayerPosture = GetPlayerPosture(OBJPlayer);
		vector =  *(GetPlayerPosition(PlayerPosture));

		
		Vector__CallLUAConstructor(L,&vector);

		return 1;
	}

	extern "C" Player__SetPosition(lua_State* L)
	{
		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		int OBJPlayer = (int)PE->ObjectPlayer;

		XMVECTOR vector = {0};
		vector = Vector__GetVectorTable(L,2);
		UINT32 PlayerPosture = GetPlayerPosture(OBJPlayer);
		*GetPlayerPosition(PlayerPosture) = vector;


		return 0;
	}






	extern "C" Player__GetRotation(lua_State* L)
	{
		
		return 0;
	}

	extern "C" Player__SetRotation(lua_State* L)
	{
		
		return 0;
	}

	extern "C" Player__GetScoreCount(lua_State* L)
	{


	}

	extern "C" Player__GetLiveCount(lua_State* L)
	{

	}

	extern "C" Player__GetRingsCount(lua_State* L)
	{

	}

	extern "C" Player__SetScoreCount(lua_State* L)
	{

	}

	extern "C" Player__SetLiveCount(lua_State* L)
	{

	}

	extern "C" Player__SetRingsCount(lua_State* L)
	{

	}


	extern "C" int Player__GetCurrentStateID(lua_State* L)
	{
		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		int OBJPlayer = (int)PE->ObjectPlayer;
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

	extern "C" int Player__SetCurrentStateID(lua_State* L)
	{
		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		int OBJPlayer = (int)PE->ObjectPlayer;
		if (OBJPlayer){
			Sonicteam::Player::State::IMachine* Mashine = *(Sonicteam::Player::State::IMachine**)(OBJPlayer + 0xE4);
			if (Mashine){
				int ID = lua_tonumber(L,2);
				Mashine->ChangeMashineState(ID);
			}
		}




		return 0;

	}

	extern "C" int Player__GetActorID(lua_State* L)
	{

		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		int OBJPlayer = (int)PE->ObjectPlayer;

		lua_pushnumber(L,GameLIB_PlayerObjToActorID(OBJPlayer));

		return 1;
	}

	extern "C" int Player__SetActorID(lua_State* L)
	{
		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		int OBJPlayer = (int)PE->ObjectPlayer;

		GameLIB_PlayerObjSetActorID(OBJPlayer,lua_tonumber(L,2));

		return 0;
	}

	extern "C" int Player__SetActorPTR(lua_State* L)
	{
		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		int OBJPlayer = (int)PE->ObjectPlayer;
		int ID = (int)lua_touserdata(L,2);

		*PE->ObjectPlayerActorPTR = ID;

	

		return 0;
	}





	extern "C" Camera__NEW(lua_State* L){



		int args = lua_gettop(L);
			int index = 0;
		if (args > 1){
			index  = lua_tonumber(L,1);
		}

		UINT32 cstack[0x10];
		GetCameraManActors((UINT32*)&cstack);

	


		lua_newtable06(L);


		lua_pushstring06(L,"ptr");
		lua_pushlightuserdata(L,(void*)(cstack[index]));
		lua_settable06(L,-3);


		lua_pushstring06(L,"index");
		lua_pushnumber(L,index);
		lua_settable06(L,-3);



		luaL_getmetatable06(L,"Cameraman");
		lua_setmetatable06(L, -2);




		return 1;
	}
	extern "C" Camera_GetPTR(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		return 1;


		


	}

	extern "C" Camera_GetPosition(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		unsigned int ptr =  (unsigned int)lua_touserdata(L,-1);

		XMVECTOR vector = {0};
		if (ptr == 0){


		}
		else{
				vector =  *(XMVECTOR*)(ptr + 0x80 + 0x20);
		}
	
	
		Vector__CallLUAConstructor(L,&vector);	
		return 1;

	}

	extern "C" Camera_GetRotation(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		unsigned int ptr =  (unsigned int)lua_touserdata(L,-1);

		XMVECTOR vector = {0};
		if (ptr == 0){

		}
		else{
			vector =  *(XMVECTOR*)(ptr + 0x70 + 0x20);
		}


		Vector__CallLUAConstructor(L,&vector);	
		return 1;
	}


	
	extern "C" Player_OpenFrame(lua_State* L){

		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		_DWORD ObjPlayer =  (_DWORD)PE->ObjectPlayer;
		int index = 0;



	
		int v2 = ObjPlayer;
		int v3 = malloc06(0x160);
		int v5;
		if ( v3 )
		{
	
			int v4 = 	BranchTo(0x8223B458,int,v3);
			v5 = v4;
			if ( v4 )
			{
				BranchTo(0x82659610,int,v4);

				XMMATRIX matrix = XMMATRIX(1,0,0,0,
										   0,1,0,0,
										   0,0,1,0,
										   0,0,0,1);
							
				
				BranchTo(0x8223B208,int,v5,&matrix);
			
			
			}
		}
		else
		{
			v5 = 0;
		}
		if ( *(_DWORD *)(v2 + 0x154)  )
		{
			int v8 = v5;
			if ( v5 )
				BranchTo(0x82659610,int,v5);
			(*(void (__fastcall **)(_DWORD, int *))(**(_DWORD **)(v2 + 0x154) + 0xC))(*(_DWORD *)(v2 + 0x154), &v8);
			if ( v8 )
				BranchTo(0x82581E38 ,int,v8);
		}
		if ( v5 )
			BranchTo(0x82659610,int,v5);
		_DWORD v6 = *(_DWORD *)(v2 + 0xCC);
		*(_DWORD *)(v2 + 0xCC) = v5;
		if ( v6 )
			BranchTo(0x82581E38 ,int,v6);
		if ( v5 )
			BranchTo(0x82581E38 ,int,v5);
	
	
	


		return 0;
	}




	extern "C" Player_OpenModel(lua_State* L){

		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		_DWORD ObjPlayer =  (_DWORD)PE->ObjectPlayer;
		int index = 0;
		int ModelPlayer = *(int*)(ObjPlayer + 0xD4);

		const char* lua_name = lua_tostring(L,2);


		Sonicteam::LuaSystem* p;


		BranchTo(0x821EA260,int,&p,&std::string(lua_name),0x82003380,0x1D);


		//Sonicteam::LuaSystem::LoadInitResource(p,std::string("player/shadow.lua"));

		BranchTo(0x82239740,int,ModelPlayer + 0x20,&p);
		p->Release();

		


		

	



		
		return 0;
	}






	extern "C" Player_ReloadContext(lua_State* L){

		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		_DWORD ObjPlayer =  (_DWORD)PE->ObjectPlayer;
		int index = 0;
		Sonicteam::Player::State::IMachine* Mashine = *(Sonicteam::Player::State::IMachine**)(ObjPlayer + 0xE4);
		

		Sonicteam::Player::State::IContext* context =  Mashine->GetMashineContext().get();



		const char* lua_name = lua_tostring(L,2);


		Sonicteam::LuaSystem* p;
		BranchTo(0x821EA260,int,&p,&std::string(lua_name),0x82003380,0x1D);

		context->OnVarible(&p);


	
		p->Release();











		return 0;
	}





	extern "C" Player_ReloadSound(lua_State* L)
	{
		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		_DWORD ObjPlayer =  (_DWORD)PE->ObjectPlayer;
		int index = 0;
	


		Sonicteam::LuaSystem* p;
			const char* lua_name = lua_tostring(L,2);
		BranchTo(0x821EA260,int,&p,&std::string(lua_name),0x82003380,0x1D);
	


		std::vector<boost::shared_ptr<Sonicteam::Player::INotification>>* Plugins = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::INotification>>*>(ObjPlayer + 0x2DC);

		for (std::vector<boost::shared_ptr<Sonicteam::Player::INotification>>::iterator it = Plugins->begin(); it != Plugins->end(); it++) {
			boost::shared_ptr<Sonicteam::Player::INotification> pluginPtr = *it;

			if (Sonicteam::Player::IPlugIn* plugin = dynamic_cast<Sonicteam::Player::IPlugIn*>(pluginPtr.get())) {

				if (plugin->PluginName == "sound"){

					int a1 = (int)plugin;

					int *v6; // r27
					int v7 = 0x8200EB78;
					int v8; // r11
					int v9; // r3
					char *v10; // r28
					int v11; // r3
					int v12; // r3
					int result; // r3


					v6 = (int *)(a1 + 0x5C);
					
					int ModelPlayer = *(int*)(ObjPlayer + 0xD4);

					v9 = *(_DWORD *)(a1 + 0x2C);
					Sonicteam::SoX::RefCountObject* CommonSound;
					Sonicteam::SoX::RefCountObject* SpecificSound;
					BranchTo(0x82618068,int,&CommonSound,ObjPlayer + 0xD0,"common");
					BranchTo(0x82618068,int,&SpecificSound,ObjPlayer + 0xD0,"specific");

					(*(void (__fastcall **)(int, int))(*(_DWORD *)v9 + 4))(v9, (int)&CommonSound);
					(*(void (__fastcall **)(int, int))(*(_DWORD *)v9 + 4))(v9, (int)&SpecificSound);





					do
					{
						v8 = *(_DWORD *)v7;
						if ( !*(_DWORD *)v7 )
							v8 = (int)lua_name;
						v9 = *(_DWORD *)(a1 + 0x2C);
						v10 = *(char **)(v7 + 4);
						if ( v9  )
						{
							v11 = (*(int (__fastcall **)(int, int))(*(_DWORD *)v9 + 0x14))(v9, v8);
							v12 = (*(int (__fastcall **)(_DWORD, int, char *))(**(_DWORD **)(a1 + 0x2C) + 0x1C))(
								*(_DWORD *)(a1 + 0x2C),
								v11,
								v10);
						}
						else
						{
							v12 = 0;
						}
						v7 += 8;
						*v6++ = v12;
					}
					while (v7 != 0x8200ED20 );
	

						BranchTo(0x822668A8,int,a1 + 0x20,&ModelPlayer);



					break;
				}
			
			}
		}
		if (p) p->Release();

	}

	extern "C" Player_OpenPackage(lua_State* L){

		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		_DWORD ObjPlayer =  (_DWORD)PE->ObjectPlayer;

		const char* v3 = lua_tostring(L, 2);
		if ( v3 ){

			std::string* S1 =  (std::string *)(ObjPlayer + 0x74);
			*S1 = v3;

			int v6 = *(_DWORD *)(ObjPlayer + 0x154);
			if ( v6 )
			{
		
				{
					int v11[8];
					int* v7 = (int *)(*(int (__fastcall **)(int *, int, int))(*(_DWORD *)v6 + 4))(v11, v6, ObjPlayer + 0x74);
					int v8 = *v7;		
				
					Sonicteam::SoX::RefCountObject* v9 = *(	Sonicteam::SoX::RefCountObject*  *)(ObjPlayer + 0xD0);
					*(_DWORD *)(ObjPlayer + 0xD0) = v8;
					if ( v9 )
						v9->Release();
				
				}
			}
		}
		return 0;
	}




	extern "C" Player_OpenInput(lua_State* L)
	{

		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		_DWORD ObjPlayer =  (_DWORD)PE->ObjectPlayer;



	

		int AllocatedInputMemDebug = (int)malloc06(0x70);
		int boostContainer[2];

		boostContainer[0]= 0;
		boostContainer[1]= 0;


		if ( AllocatedInputMemDebug )
		{
			int v12 = (int)(ObjPlayer + 8);
			if ( !ObjPlayer )
				v12 = 0;

			int AllocatedInputMemDebugT = BranchTo(0x822219F0,int,AllocatedInputMemDebug,(int)(ObjPlayer + 0x25),v12);
			BranchTo(0x821B8868,int,boostContainer,AllocatedInputMemDebugT);



			
			BranchTo(0x821B8A28,int,ObjPlayer,&boostContainer,0u);
			if ( boostContainer[1] ) BranchTo(0x821601B8,int,boostContainer[1]);
	
		}






		return 0;
	}

	extern "C" Player_OpenPostureControl(lua_State* L)
	{

		Player_NEWS* PE = (*reinterpret_cast<Player_NEWS**>(luaL_checkudata(L, 1, "Player")));
		_DWORD ObjPlayer =  (_DWORD)PE->ObjectPlayer;



		int AllocatedPostureDebugMem = (int)malloc06(0x140);
		int AllocatedPostureDebugMemT;
		int v40[2];
		int BoostAllocatedPostureDebugMem[2];
		BoostAllocatedPostureDebugMem[0] = 0;
		BoostAllocatedPostureDebugMem[1] = 0;
		int flag = 8;
		int DRefCountObj1;
		int* DRefCountObj;
		if ( AllocatedPostureDebugMem )
		{
			Sonicteam::SoX::RefCountObject* DRefCountObj0 = *(Sonicteam::SoX::RefCountObject**)(ObjPlayer + 0xCC);
			if ( DRefCountObj0 )
				DRefCountObj0->GetObject<int>(); //add ref
			int* v15 = BranchTo(0x823D0E88,int*,(int)v40, ObjPlayer);
			int flag = 8;
			DRefCountObj = (int *)(*(int (__fastcall **)(int *,int))(**(_DWORD **)v15 + 0x18))(&DRefCountObj1,*v15);

	
			AllocatedPostureDebugMemT = BranchTo(0x821F75E0,int,(int)AllocatedPostureDebugMem, DRefCountObj, &DRefCountObj0);
			BranchTo(0x821B9498,int,BoostAllocatedPostureDebugMem, AllocatedPostureDebugMem);
				if ( (flag & 8) != 0 && v40[1] )
					BranchTo(0x821601B8,int,v40[1]);
			

		}
		else
		{
			AllocatedPostureDebugMemT = 0;
		}



		BranchTo(0x821B95E8,int,ObjPlayer,BoostAllocatedPostureDebugMem,1u);
		*(_DWORD *)(ObjPlayer + 0xDC) = BoostAllocatedPostureDebugMem[0];
		int v28 = BoostAllocatedPostureDebugMem[1];
		if ( BoostAllocatedPostureDebugMem[1] )
			BranchTo(0x82160140,int,BoostAllocatedPostureDebugMem[1]);

		int v29 = *(_DWORD *)(ObjPlayer + 0xE0);
		if ( v29 )
			BranchTo(0x821601B8,int,v29);

		*(_DWORD *)(ObjPlayer + 0xE0) = v28;




		return 0;
	}

	extern "C" static int GetPlayerRawInput(lua_State* L)
	{
		int n = lua_gettop(L);  
		if (n <= 0) 

		{lua_pushnumber(L, 0); return 1;}


		UINT32 PlayerIndex = (UINT32)lua_tonumber(L,1);
		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		PlayerIndex = impl->GetRealControllerID(PlayerIndex);
		Sonicteam::Player::Input::IListenerInputStruc01* Inp2 = (Sonicteam::Player::Input::IListenerInputStruc01*)impl->GetPlayerInput(PlayerIndex);

		Sonicteam::Player::Input::IListenerInputStruc01 Inp;


		if (Inp2){
			memcpy(&Inp,Inp2,sizeof(Sonicteam::Player::Input::IListenerInputStruc01));
		}


		BranchTo(0x825D5C30,int,L); //lua_newtable


		//RtlReAllocateHeap NOT same :|

		BranchTo(0x825D5918,int,L,"Buttons");
		//BranchTo(0x825D5890,int,L,Inp.wLastButtons);
		lua_pushnumber(L, Inp.wLastButtons);
		BranchTo(0x825D5D98,int,L,-3);



		BranchTo(0x825D5918,int,L,"LStickX");
		BranchTo(0x825D5890,int,L,Inp.fX1);
		BranchTo(0x825D5D98,int,L,-3);


		BranchTo(0x825D5918,int,L,"LStickY");
		BranchTo(0x825D5890,int,L,Inp.fY1);
		BranchTo(0x825D5D98,int,L,-3);

		BranchTo(0x825D5918,int,L,"RStickX");
		BranchTo(0x825D5890,int,L,Inp.fX2);
		BranchTo(0x825D5D98,int,L,-3);

		BranchTo(0x825D5918,int,L,"RStickY");
		BranchTo(0x825D5890,int,L,Inp.fY2);
		BranchTo(0x825D5D98,int,L,-3);

		
		/*

		lua_pushstring(L, "Buttons");
		lua_pushnumber(L, Inp.wLastButtons);
		lua_settable(L, -3);

		lua_pushstring(L, "LStickX");
		lua_pushnumber(L, Inp.fX1);
		lua_settable(L, -3);

		lua_pushstring(L, "LStickY");
		lua_pushnumber(L, Inp.fY1);
		lua_settable(L, -3);

		lua_pushstring(L, "RStickX");
		lua_pushnumber(L, Inp.fX2);
		lua_settable(L, -3);

		lua_pushstring(L, "RStickY");
		lua_pushnumber(L, Inp.fY2);
		lua_settable(L, -3);

	
*/




		return 1;
	}


	extern "C" int GetPlayerInput(lua_State* L){


	
		int n = lua_gettop(L);  
		if (n <= 0) 
		{lua_pushnumber(L, 0); return 1;}


		UINT32 PlayerIndex = (UINT32)lua_tonumber(L,1);
		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		Sonicteam::GameImp* gameimp = *(Sonicteam::GameImp**)(impl->DocCurrentMode + 0x6C);		
		UINT32 ActorID = gameimp->PlayerGameplayerData[PlayerIndex].ActorID;
		if (ActorID == 0xFFFFFFFF) {lua_pushnumber(L,0);return 1;}
		UINT32 ObjPlayer =  BranchTo(0x821609D0,int,gameimp->GameActorManager.get(),&ActorID);



		if (!ObjPlayer) {lua_pushnumber(L,0);return 1;}
						


		std::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>* IPluginP = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>*>(ObjPlayer + 0x114);
		for (std::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>::iterator it  = IPluginP->begin();it!=IPluginP->end();it++){
			Sonicteam::Player::IPlugIn* plug = it->get();
			if (Sonicteam::Player::Input::IListener* listener = dynamic_cast<Sonicteam::Player::Input::IListener*>(plug)){
				lua_pushnumber(L,listener->ListenerGetResult());
				return 1;
			}
		}



		lua_pushnumber(L,0);
		return 1;
	}



	extern "C" int PrintNextFixed(std::string msg){
		
		
#ifdef ALTERNATIVE_UTIL_MSG
		AddMessage(msg);
#else

		int length = msg.length() + 1;
		wchar_t* wcharPtr = new wchar_t[length];
		std::memset(wcharPtr, 0, length * sizeof(wchar_t));
		std::mbstowcs(wcharPtr, msg.c_str(), length);
		int MCount = DebugMessages.size();
		if (MCount > 5){
			MCount--;
			std::vector<int>::iterator it = DebugMessages.begin();
			BranchTo(0x8262BA68,int,*it,1); //Destroy from mem
			std::advance(it, 0);
			DebugMessages.erase(it);

			//Re Order Messages

			int cc = 0;
			for (std::vector<int>::iterator it = DebugMessages.begin(); it != DebugMessages.end(); ++it) {
				ChangeMessagePositionY( *it,(cc * 28) );
				cc++;
			}
		}
		DebugMessages.push_back( SpawnMessage(wcharPtr,(MCount * 28)));

#endif



		return 0;
	}

	extern "C" int PrintNext(lua_State* L){

		int n = lua_gettop(L);  /* number of arguments */
		int i;
		lua_getglobal06(L, "tostring");
		for (i=1; i<=n; i++) {
			const char *s;
			lua_pushvalue(L, -1);  /* function to be called */
			lua_pushvalue(L, i);   /* value to print */
			lua_call06(L, 1, 1);
			s = lua_tostring(L, -1);  /* get result */
			if (s == NULL)
				return luaL_error(L, "`tostring' must return a string to `print'");
			
			std::string msg =  std::string(s);

			int length = msg.length() + 1;
			wchar_t* wcharPtr = new wchar_t[length];
			std::memset(wcharPtr, 0, length * sizeof(wchar_t));
			std::mbstowcs(wcharPtr, msg.c_str(), length);


			/*
			int MCount = DebugMessages.size();
			if (MCount > 2){
				MCount--;
				std::vector<int>::iterator it = DebugMessages.begin();
				BranchTo(0x8262BA68,int,*it,1); //Destroy from mem
				std::advance(it, 0);
				DebugMessages.erase(it);

				//Re Order Messages
				
				int cc = 0;
				for (std::vector<int>::iterator it = DebugMessages.begin(); it != DebugMessages.end(); ++it) {
					ChangeMessagePositionY( *it,(cc * 28) );
					cc++;
				}
			}

			DebugMessages.push_back( SpawnMessage(wcharPtr,(MCount * 28)));
			*/
			AddMessage(msg);
	
			//if (i>1) fputs("\t", stdout);
			//fputs(s, stdout);
			lua_pop(L, 1);  /* pop result */
		}
		fputs("\n", stdout);
		return 0;

	}


	//Size 4

	void GetCameraManActors(UINT32* pstack){
	
		pstack[0] = 0;
		pstack[1] = 0;
		pstack[2] = 0;
		pstack[3] = 0;

		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		UINT32 vft =  *(UINT32*)impl->DocCurrentMode;
	
		if (vft == 0x82033534){ //GameMode

			UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);
			UINT32 ActorManager = *(UINT32*)(gameimp+0x11C4);
			UINT32 ActorMangerActorsCount = *(UINT32*)(ActorManager+0x80000);
			UINT32 PIX = 0;
			for (int i = 0;i<ActorMangerActorsCount;i++)
			{
				UINT32 Actor = *(UINT32*)(ActorManager+0x40000+(i*4));
				UINT32 ActorVFT = *(UINT32*)Actor;
				if (ActorVFT == 0x82001F2C){ //Object_Player


					byte IsObjectPlayerHaveControll = *(byte*)(Actor + 0xC8);
					byte AIFLAG = *(byte*)(Actor + 0xCA);
						if (IsObjectPlayerHaveControll){
							pstack[PIX++] = Actor;
						}
			

				
				

	
					/*
					Sonicteam::Player::State::IMachine* PlayerMashine = *(Sonicteam::Player::State::IMachine**)(Actor+0xE4);
					if (PlayerMashine){

						Sonicteam::Player::State::IContext* context  =  PlayerMashine->GetMashineContext().get();
						Sonicteam::Player::State::CommonContext* PCC0 = dynamic_cast<Sonicteam::Player::State::CommonContext*>(context);
						Sonicteam::Player::State::FastContext* PCF0 = dynamic_cast<Sonicteam::Player::State::FastContext*>(context);
						bool flag = false;
						if (PCC0){
							Sonicteam::Player::Input::IListener* Listener =  PCC0->ListenerNormalInputPlugin.get();
							flag =  Listener->Listener5();
							
						}
						else if (PCF0){
							Sonicteam::Player::Input::IListener* Listener =  PCF0->ListenerNormalInputPlugin.get();
							flag = Listener->Listener5();
						}

						if (flag ) pstack[PIX++] = Actor;
					}
					*/


				}
			}
		}
	}

	void GetPlayerActors(UINT32* pstack,bool AI){
	
		pstack[0] = 0;
		pstack[1] = 0;
		pstack[2] = 0;
		pstack[3] = 0;

		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		UINT32 vft =  *(UINT32*)impl->DocCurrentMode;
	
		if (vft == 0x82033534){ //GameMode

			UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);
			UINT32 ActorManager = *(UINT32*)(gameimp+0x11C4);
			UINT32 ActorMangerActorsCount = *(UINT32*)(ActorManager+0x80000);
			UINT32 PIX = 0;
			for (int i = 0;i<ActorMangerActorsCount;i++)
			{
				UINT32 Actor = *(UINT32*)(ActorManager+0x40000+(i*4));
				UINT32 ActorVFT = *(UINT32*)Actor;
				if (ActorVFT == 0x82003564){ //Object_Player


					byte IsObjectPlayerHaveControll = *(byte*)(Actor + 0xC8);
					byte AIFLAG = *(byte*)(Actor + 0xCA);
					if (AI)
					{
						if (IsObjectPlayerHaveControll == 0 && AIFLAG){
							pstack[PIX++] = Actor;
						}
					

					}
					else{
						if (IsObjectPlayerHaveControll){
							pstack[PIX++] = Actor;
						}
					}


				}
			}
		}
	}

	UINT32 GetPlayerPosture(UINT32 ObjectPlayer){
		return *(UINT32 *)(ObjectPlayer + 0xDC);
	}
	XMVECTOR* GetPlayerPosition(UINT32 PlayerPosture){
		return (XMVECTOR*)(PlayerPosture + 0xB0);
	}



	extern "C" int GetPlayerPosition(lua_State* L){

		int n = lua_gettop(L);  
		if (n <= 0) 
		{lua_pushnumber(L, 0); return 1;}

		UINT32 PlayerIndex = (UINT32)lua_tonumber(L,1);

		UINT32 pstack[4] = {};
		GetPlayerActors((UINT32*)&pstack);

		XMFLOAT4 POS;
		if (pstack[PlayerIndex] != 0){
			UINT32 ObjectPlayer = pstack[PlayerIndex];
			UINT32 PlayerPosture = GetPlayerPosture(ObjectPlayer);
			XMVECTOR* PlayerPos = GetPlayerPosition(PlayerPosture);
			memcpy(&POS,PlayerPos,sizeof(XMFLOAT4));
		}

		
		BranchTo(0x825D5C30,int,L); //lua_newtable

		BranchTo(0x825D5918,int,L,"X");
		BranchTo(0x825D5890,int,L,POS.x);
		BranchTo(0x825D5D98,int,L,-3);

		BranchTo(0x825D5918,int,L,"Y");
		BranchTo(0x825D5890,int,L,POS.y);
		BranchTo(0x825D5D98,int,L,-3);

		BranchTo(0x825D5918,int,L,"Z");
		BranchTo(0x825D5890,int,L,POS.z);
		BranchTo(0x825D5D98,int,L,-3);
		




		return 1;

	}


	





	

	extern "C" int SetPlayerPosition(lua_State* L)
	{

		int n = lua_gettop(L);  
		if (n <= 0) return 0;
		UINT32 PlayerIndex = (UINT32)lua_tonumber(L,1);
		UINT32 pstack[4] = {};



		GetPlayerActors((UINT32*)&pstack);

		XMFLOAT4 POS;
		if (pstack[PlayerIndex] != 0){
			UINT32 ObjectPlayer = pstack[PlayerIndex];
			UINT32 PlayerPosture = GetPlayerPosture(ObjectPlayer);
			XMVECTOR* PlayerPos = GetPlayerPosition(PlayerPosture);
			PlayerPos->x = lua_tonumber(L,2);
			PlayerPos->y = lua_tonumber(L,3);
			PlayerPos->z = lua_tonumber(L,4);


		}


	





		return 0;
	}

	int STRLIB_GlobalInstall(lua_State* LS)
	{
		luaopen_string_06(LS);
		return 0;
	}



	



	extern "C" int MessageReceiver_GlobalInstall(lua_State* LS)
	{

		lua_register06(LS,"MessageReceiver",MessageReceiver__NEW);

		luaL_newmetatable06(LS,"MessageReceiver");

		//lua_pushstring06(LS, "__gc"); lua_pushcfunction06(LS, Vector__GC); lua_settable06(LS, -3);
		//	lua_pushstring06(LS, "__tostring"); lua_pushcfunction06(LS, Vector__tostring); lua_settable06(LS, -3);
		//	lua_pushstring06(LS, "__add"); lua_pushcfunction06(LS, Vector__add); lua_settable06(LS, -3);
		//	lua_pushstring06(LS, "__sub"); lua_pushcfunction06(LS, Vector__sub); lua_settable06(LS, -3);

		lua_pushvalue(LS, -1);  lua_pushstring06(LS, "__index"); lua_pushvalue(LS,-2);  lua_settable06(LS,-3);

		lua_pushstring06(LS, "SendMessage"); lua_pushcfunction06(LS, MessageReceiver__SendMessage); 	lua_settable06(LS, -3);

		//lua_pop(LS,1);
		return 1;

	}

	extern "C" int MessageReceiver__CreateMetatable(lua_State* L,int PTR){

		lua_newtable06(L);


		lua_pushstring06(L,"ptr");
		lua_pushlightuserdata(L,(void*)PTR);
		lua_settable06(L,-3);



		luaL_getmetatable06(L,"MessageReceiver");
		lua_setmetatable06(L, -2);



		return 0;

	}

	extern "C" int MessageReceiver__NEW(lua_State* L){

		int args = lua_gettop(L);

		int id =  0;

		if (lua_isuserdata(L,1)){
			id = (int)lua_touserdata(L,1);
		}
		else{
			id = lua_tonumber(L,1);
		}

	

		MessageReceiver__CreateMetatable(L,id);

		return 1;


	}

	extern "C" int MessageReceiver__SendMessage(lua_State* L){


		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::SoX::MessageReceiver* ptr =  (Sonicteam::SoX::MessageReceiver*)lua_touserdata(L,-1);

		UINT32 Messages[5] = {0};

		//RAW
		for (int i = 0;i<5;i++){
			Messages[i] = lua_tonumber(L,2+i);
		}



		Sonicteam::SoX::Message msg  = Sonicteam::SoX::Message(Messages[0],Messages[1],Messages[2],Messages[3],Messages[4]);



		ptr->OnMessageRecieved(&msg);



		return 0;


	}








	extern "C" int MainDisplayTask_GlobalInstall(lua_State* LS)
	{

		lua_register06(LS,"MainDisplayTask",MainDisplayTask__NEW);

		luaL_newmetatable06(LS,"MainDisplayTask");

		//lua_pushstring06(LS, "__gc"); lua_pushcfunction06(LS, Vector__GC); lua_settable06(LS, -3);
	//	lua_pushstring06(LS, "__tostring"); lua_pushcfunction06(LS, Vector__tostring); lua_settable06(LS, -3);
	//	lua_pushstring06(LS, "__add"); lua_pushcfunction06(LS, Vector__add); lua_settable06(LS, -3);
	//	lua_pushstring06(LS, "__sub"); lua_pushcfunction06(LS, Vector__sub); lua_settable06(LS, -3);

		lua_pushvalue(LS, -1);  lua_pushstring06(LS, "__index"); lua_pushvalue(LS,-2);  lua_settable06(LS,-3);

		lua_pushstring06(LS, "SendMessage"); lua_pushcfunction06(LS, MainDisplayTask__SendMessage); 	lua_settable06(LS, -3);

		//lua_pop(LS,1);
		return 1;
	}

	extern "C" int MainDisplayTask__CreateMetatable(lua_State* L,int ID,int MainDisplayTask){

		lua_newtable06(L);


		lua_pushstring06(L,"ptr");
		lua_pushlightuserdata(L,(void*)MainDisplayTask);
		lua_settable06(L,-3);


		lua_pushstring06(L,"id");
		lua_pushnumber(L,ID);
		lua_settable06(L,-3);



		luaL_getmetatable06(L,"MainDisplayTask");
		lua_setmetatable06(L, -2);



		return 0;

	}

	extern "C" int MainDisplayTask__NEW(lua_State* L){

		int args = lua_gettop(L);

		int id =  lua_tonumber(L,2);
		
		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		UINT32 vft =  *(UINT32*)impl->DocCurrentMode;

		int PTRX = 0;

		if (vft == 0x82033534){ //GameMode

			UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);

			 PTRX =  *(UINT32*)(0x10 * (id + 0x123) + gameimp);
		

		
		}	
		MainDisplayTask__CreateMetatable(L,id,PTRX);

		return 1;


	}

	extern "C" int MainDisplayTask__SendMessage(lua_State* L){


		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::SoX::Engine::Task* ptr =  (Sonicteam::SoX::Engine::Task*)lua_touserdata(L,-1);

		UINT32 Messages[5] = {0};

		//RAW
		for (int i = 0;i<5;i++){
			Messages[i] = lua_tonumber(L,2+i);
		}
		
		
		
		Sonicteam::SoX::Message msg  = Sonicteam::SoX::Message(Messages[0],Messages[1],Messages[2],Messages[3],Messages[4]);

	
		
		ptr->OnMessageRecieved(&msg);

	

		return 0;


	}


	extern "C" int GameImp_GlobalInstall(lua_State* LS)
	{

		lua_register06(LS,"GameImp",GameImp_NEW);

		luaL_newmetatable06(LS,"GameImp");

		//lua_pushstring06(LS, "__gc"); lua_pushcfunction06(LS, Vector__GC); lua_settable06(LS, -3);
		//	lua_pushstring06(LS, "__tostring"); lua_pushcfunction06(LS, Vector__tostring); lua_settable06(LS, -3);
		//	lua_pushstring06(LS, "__add"); lua_pushcfunction06(LS, Vector__add); lua_settable06(LS, -3);
		//	lua_pushstring06(LS, "__sub"); lua_pushcfunction06(LS, Vector__sub); lua_settable06(LS, -3);

		lua_pushvalue(LS, -1);  lua_pushstring06(LS, "__index"); lua_pushvalue(LS,-2);  lua_settable06(LS,-3);

	//	lua_pushstring06(LS, "SendMessage"); lua_pushcfunction06(LS, MainDisplayTask__SendMessage); 	lua_settable06(LS, -3);

		//lua_pop(LS,1);


		return 1;
	}


	extern "C" int GameImp__CreateMetatable(lua_State* L,int GameIMP){

		lua_newtable06(L);


		lua_pushstring06(L,"ptr");
		lua_pushlightuserdata(L,(void*)GameIMP);
		lua_settable06(L,-3);




		luaL_getmetatable06(L,"GameImp");
		lua_setmetatable06(L, -2);



		return 0;

	}

	extern "C" int GameImp_NEW(lua_State* LS)
	{


		int args = lua_gettop(LS);

		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		UINT32 vft =  *(UINT32*)impl->DocCurrentMode;




		int PTRX = 0;
		UINT32 gameimp  = 0;

		if (vft == 0x82033534){ //GameMode

			 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);


		}	
		GameImp__CreateMetatable(LS,gameimp);

		return 1;
	}

}