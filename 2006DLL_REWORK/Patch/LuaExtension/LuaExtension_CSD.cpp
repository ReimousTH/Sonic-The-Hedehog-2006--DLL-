#include "LuaExtension_CSD.h"

#define CSD_META_STR "CsdObjectMeta"
#define CSD_META_CONSTRUCT_FUNC_NAME "CsdObject"


#define CPROJECT_META_STR "CProjectMeta"
#define CPROJECT_META_CONSTRUCT_FUNC_NAME "CProject"


#define CSCENE_META_STR "CSceneMeta"
#define CSCENE_META_CONSTRUCT_FUNC_NAME "CScene"



#define CNODE_META_STR "CNodeMeta"
#define CNODE_META_CONSTRUCT_FUNC_NAME "CNode"




#include <CsdObject.h>


namespace DebugLogV2{




	extern "C" CSD__CREATEMETATABLE(lua_State* L,void* arg1){

		lua_getglobal06(L, "Memory");
		lua_pushlightuserdata(L, arg1);
		lua_pcall06(L, 1, 1, 0);

		luaL_getmetatable06(L, CSD_META_STR);
		lua_setmetatable06(L, -2);


		return 1;
	}

	extern "C" CSD__NEW(lua_State* L){
		return CSD__CREATEMETATABLE(L,(void*)Misc::GetNumber(L,1));
	}


	extern "C" CSD__GETCPROJECT(lua_State*L){

		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Sonicteam::CsdObject* csd_obj = 	(Sonicteam::CsdObject*)Misc::GetNumber(L,-1);
		return CPROJECT__CREATEMETATABLE(L,(void*)csd_obj->FCProject);
	}


	extern "C" CPROJECT__CREATEMETATABLE(lua_State* L,void* arg1){

		lua_getglobal06(L, "Memory");
		lua_pushlightuserdata(L, arg1);
		lua_pcall06(L, 1, 1, 0);

		luaL_getmetatable06(L, CPROJECT_META_STR);
		lua_setmetatable06(L, -2);


		return 1;
	}

	extern "C" CPROJECT__NEW(lua_State* L){
		return CSD__CREATEMETATABLE(L,(void*)Misc::GetNumber(L,1));
	}


	extern "C" CPROJECT__GETCSCENE(lua_State*L){

		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CProject* CProject = 	(Chao::CSD::CProject*)Misc::GetNumber(L,-1);



		const char* SceneName = lua_tostring(L,2);
		std::map<const char*,RCOBJREF(Chao::CSD::CScene),STD_MAP_CONST_CHAR_PTR_COMPARATOR>::iterator it =  CProject->CProjectScene.find(SceneName);
		if (it != CProject->CProjectScene.end()){

			Chao::CSD::RCObject<Chao::CSD::CScene>* rc =  it->second.get();
			Chao::CSD::CScene* cscene = rc->get();
			return CSCENE__CREATEMETATABLE(L,(void*)cscene);
		}
		else{ 
			RCOBJREF(Chao::CSD::CScene) _CScene;
			((void (__fastcall *)(RCOBJREF(Chao::CSD::CScene)*,Chao::CSD::CProject*,const char*,const char*,size_t))0x825CEB58)(&_CScene,CProject,SceneName,"",0);		
			
			//CProject->CProjectScene[_CScene.get()->get()->CSName] = _CScene;
			
			return CSCENE__CREATEMETATABLE(L,(void*)_CScene.get()->get());
		}


		return CSCENE__CREATEMETATABLE(L,(void*)0);
	}





	extern "C" CSCENE__CREATEMETATABLE(lua_State* L,void* arg1){

		lua_getglobal06(L, "Memory");
		lua_pushlightuserdata(L, arg1);
		lua_pcall06(L, 1, 1, 0);

		luaL_getmetatable06(L, CSCENE_META_STR);
		lua_setmetatable06(L, -2);


		return 1;
	}

	extern "C" CSCENE__NEW(lua_State* L){
		return CSCENE__CREATEMETATABLE(L,(void*)Misc::GetNumber(L,1));
	}





	extern "C" CSCENE_GetCNode(lua_State* L){

		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);
		const char* Name = lua_tostring(L,2);
		RCOBJREF(Chao::CSD::CNode) _CNode;
		((void (__fastcall *)(RCOBJREF(Chao::CSD::CNode)*,Chao::CSD::CScene*,const char*))0x825CB970)(&_CNode,CScene,Name);	
		return 	CNODE__CREATEMETATABLE(L,(void*)_CNode.get()->get());
	}


	extern "C" CSCENE_UpdateMotionFlag(lua_State* L){

		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);

		CScene->MotionRepeatMode = 0;
		CScene->CCSFlag3 = 0;

		return 0;
	}



	extern "C" CSCENE_IsMotionEnd(lua_State* L){

		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);
		lua_pushboolean(L,CScene->IsMotionEnd);
		return 1;
	}


	extern "C" CSCENE_SetMotionRepeatMode(lua_State* L){

		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);
		CScene->MotionRepeatMode = lua_tonumber06(L,2);
		return 0;
	}

	extern "C" CSCENE_GetMotionRepeatMode(lua_State* L){

		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);
		lua_pushnumber(L,CScene->MotionRepeatMode);
		return 0;
	}


	extern "C" CSCENE_GetMotionIndex(lua_State* L){

		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);
		
		return 1;
	}

	extern "C" CSCENE_Render(lua_State* L){
		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);
		CScene->OnUpdateFirst(1.0/60.0);
	//	CScene->OnUpdateSecond(1.0/60.0);
		return 0;

	}

	extern "C" CSCENE_Reload(lua_State* L){
		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);
		size_t raw_1 =  (size_t)CScene->CMTransform3->CObjectRaw;
		*(size_t*)(raw_1 + 0x38) = 0; //force_to_re_render all CSD nodes

		return 0;

	}

	extern "C" CSCENE_StopMotion(lua_State* L){

		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);


		CScene->MotionIndex = -1;
		CScene->MotionKeyFramePre = 0;
		CScene->MotionKeyFramePost = 0;
		CScene->MotionKeyFrameRate = 0.0;
		CScene->IsMotionEnd = 0;
		CScene->MotionRepeatMode = 0;
		CScene->CCSFlag3 = 0;
		CScene->CSMotionData->DestroyObject(0); // yep
		CScene->CSMotionData = 0;



		return 0;
	}



	extern "C" CSCENE_SetMotion(lua_State* L){

		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);

		CScene->IsMotionEnd = 1;
		BranchTo(0x825CA448,int,CScene,(size_t)lua_tostring(L,2));
		CScene->MotionKeyFramePre = 0;
		CScene->MotionKeyFramePost = 0;
		CScene->MotionKeyFrameRate = 1.0;
		CScene->IsMotionEnd = 0;
		CScene->MotionRepeatMode = 0;
		CScene->CCSFlag3 = 0;


		return 0;
	}

	extern "C" CSCENE_SetMotionIndex(lua_State* L){


		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);
		BranchTo(0x825CA368,int,CScene,(size_t)lua_tonumber06(L,2));
		return 0;
	}



	extern "C" CSCENE_GetKeyFrame(lua_State* L){
		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);
		lua_pushnumber(L,	CScene->MotionKeyFramePost);
		return 1;
	}

	extern "C" CSCENE_SetKeyFrame(lua_State* L){


		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);

		CScene->MotionKeyFramePre = lua_tonumber06(L,2);
		CScene->MotionKeyFramePost = lua_tonumber06(L,2);
		return 0;
	}


	extern "C" CSCENE_GetKeyFrameRate(lua_State* L){
		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);
		lua_pushnumber(L,	CScene->MotionKeyFrameRate);
		return 1;
	}


	extern "C" CSCENE_SetKeyFrameRate(lua_State* L){


		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);
		CScene->MotionKeyFrameRate = lua_tonumber06(L,2);
		return 0;
	}

	extern "C" CSCENE_GetKeyFrameStart(lua_State* L){
		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);
		lua_pushnumber(L,CScene->MotionKeyFrameStart);
		return 1;
	}


	extern "C" CSCENE_SetKeyFrameStart(lua_State* L){


		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);
		CScene->MotionKeyFrameStart = lua_tonumber06(L,2);
		return 0;
	}

	extern "C" CSCENE_GetKeyFrameEnd(lua_State* L){
		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);
		lua_pushnumber(L,	CScene->MotionKeyFrameEnd);
		return 1;
	}

	extern "C" CSCENE_SetKeyFrameEnd(lua_State* L){


		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CScene* CScene = (Chao::CSD::CScene*)Misc::GetNumber(L,-1);
		CScene->MotionKeyFrameEnd = lua_tonumber06(L,2);
		return 0;
	}



	extern "C" CNODE__CREATEMETATABLE(lua_State* L,void* arg1){

		lua_getglobal06(L, "Memory");
		lua_pushlightuserdata(L, arg1);
		lua_pcall06(L, 1, 1, 0);

		luaL_getmetatable06(L, CNODE_META_STR);
		lua_setmetatable06(L, -2);


		return 1;
	}

	extern "C" CNODE__NEW(lua_State* L){
		return CNODE__CREATEMETATABLE(L,(void*)Misc::GetNumber(L,1));
	}





	extern "C" CNODE_SetPatternIndex(lua_State* L){


		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CNode* CNode = (Chao::CSD::CNode*)Misc::GetNumber(L,-1);
		size_t index = lua_tonumber(L,2);
		BranchTo(0x825CED98,int,CNode,index); //because method also crop image :)
		return 0;
	}

	extern "C" CNODE_GetPatternIndex(lua_State* L){

		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CNode* CNode = (Chao::CSD::CNode*)Misc::GetNumber(L,-1);
		return 1; // ?
	}




	extern "C" CNODE_SetText(lua_State* L){


		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CNode* CNode = (Chao::CSD::CNode*)Misc::GetNumber(L,-1);
		const char* _str = lua_tostring(L,2);
		BranchTo(0x825CF700,int,CNode,_str); //because method also crop image :)
		return 0;
	}

	extern "C" CNODE_GetText(lua_State* L){


		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		Chao::CSD::CNode* CNode = (Chao::CSD::CNode*)Misc::GetNumber(L,-1);


		const char* _str =  CNode->Text.get()->get();



		lua_pushstring06(L,_str);

		return 1;
	}




	int CSD_GlobalInstall(lua_State* L)
	{
		if(L == 0) return 0;





		lua_register06(L, CSD_META_CONSTRUCT_FUNC_NAME, CSD__NEW);
		luaL_newmetatable06(L, CSD_META_STR);
		lua_pushstring06(L,"__index"); lua_pushvalue(L,-2); lua_settable06(L,-3); 
		lua_newtable06(L);lua_pushstring06(L, "__index");luaL_getmetatable06(L, "MemoryMeta");lua_settable06(L, -3); lua_setmetatable06(L, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })
		lua_pushstring06(L, "GetCProject"); lua_pushcfunction06(L, CSD__GETCPROJECT); 	lua_settable06(L, -3);
		lua_pop(L,1);



		lua_register06(L, CPROJECT_META_CONSTRUCT_FUNC_NAME, CPROJECT__NEW);
		luaL_newmetatable06(L, CPROJECT_META_STR);
		lua_pushstring06(L,"__index"); lua_pushvalue(L,-2); lua_settable06(L,-3); 
		lua_newtable06(L);lua_pushstring06(L, "__index");luaL_getmetatable06(L, "MemoryMeta");lua_settable06(L, -3); lua_setmetatable06(L, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })
		lua_pushstring06(L, "GetCScene"); lua_pushcfunction06(L, CPROJECT__GETCSCENE); 	lua_settable06(L, -3);
		lua_pop(L,1);



		lua_register06(L, CSCENE_META_CONSTRUCT_FUNC_NAME, CSCENE__NEW);
		luaL_newmetatable06(L, CSCENE_META_STR);
		lua_pushstring06(L,"__index"); lua_pushvalue(L,-2); lua_settable06(L,-3); 
		lua_newtable06(L);lua_pushstring06(L, "__index");luaL_getmetatable06(L, "MemoryMeta");lua_settable06(L, -3); lua_setmetatable06(L, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })
		
	

		lua_pushstring06(L, "GetCNode"); lua_pushcfunction06(L, CSCENE_GetCNode); 	lua_settable06(L, -3);


		lua_pushstring06(L, "GetMotionIndex"); lua_pushcfunction06(L, CSCENE_GetMotionIndex); 	lua_settable06(L, -3);
		lua_pushstring06(L, "GetMotionRepeatFlag"); lua_pushcfunction06(L, CSCENE_GetMotionRepeatMode); 	lua_settable06(L, -3);
		lua_pushstring06(L, "GetKeyFrame"); lua_pushcfunction06(L, CSCENE_GetKeyFrame); 	lua_settable06(L, -3);
		lua_pushstring06(L, "GetKeyFrameRate"); lua_pushcfunction06(L, CSCENE_GetKeyFrameRate); 	lua_settable06(L, -3);
		lua_pushstring06(L, "GetKeyFrameStart"); lua_pushcfunction06(L, CSCENE_GetKeyFrameStart); 	lua_settable06(L, -3);
		lua_pushstring06(L, "GetKeyFrameEnd"); lua_pushcfunction06(L, CSCENE_GetKeyFrameEnd); 	lua_settable06(L, -3);





		lua_pushstring06(L, "UpdateMotionFlag"); lua_pushcfunction06(L, CSCENE_UpdateMotionFlag); 	lua_settable06(L, -3);
		lua_pushstring06(L, "IsMotionEnd"); lua_pushcfunction06(L, CSCENE_IsMotionEnd); 	lua_settable06(L, -3);
		lua_pushstring06(L, "StopMotion"); lua_pushcfunction06(L, CSCENE_StopMotion); 	lua_settable06(L, -3);
		lua_pushstring06(L, "Render"); lua_pushcfunction06(L, CSCENE_Render); 	lua_settable06(L, -3);
		lua_pushstring06(L, "Reload"); lua_pushcfunction06(L, CSCENE_Reload); 	lua_settable06(L, -3);

	


		
		lua_pushstring06(L, "SetMotion"); lua_pushcfunction06(L, CSCENE_SetMotion); 	lua_settable06(L, -3);
		lua_pushstring06(L, "SetMotionIndex"); lua_pushcfunction06(L, CSCENE_SetMotionIndex); 	lua_settable06(L, -3);
		lua_pushstring06(L, "SetMotionRepeatMode"); lua_pushcfunction06(L, CSCENE_SetMotionRepeatMode); 	lua_settable06(L, -3);
		lua_pushstring06(L, "SetKeyFrame"); lua_pushcfunction06(L, CSCENE_SetKeyFrame); 	lua_settable06(L, -3);
		lua_pushstring06(L, "SetKeyFrameRate"); lua_pushcfunction06(L, CSCENE_SetKeyFrameRate); 	lua_settable06(L, -3);
		lua_pushstring06(L, "SetKeyFrameStart"); lua_pushcfunction06(L, CSCENE_SetKeyFrameStart); 	lua_settable06(L, -3);
		lua_pushstring06(L, "SetKeyFrameEnd"); lua_pushcfunction06(L, CSCENE_SetKeyFrameEnd); 	lua_settable06(L, -3);
		lua_pop(L,1);



		lua_register06(L, CNODE_META_CONSTRUCT_FUNC_NAME, CNODE__NEW);
		luaL_newmetatable06(L, CNODE_META_STR);
		lua_pushstring06(L,"__index"); lua_pushvalue(L,-2); lua_settable06(L,-3); 
		lua_newtable06(L);lua_pushstring06(L, "__index");luaL_getmetatable06(L, "MemoryMeta");lua_settable06(L, -3); lua_setmetatable06(L, -2); //setmetatable(PlayerMeta, { __index = MemoryMeta })
		
		lua_pushstring06(L, "SetPatternIndex"); lua_pushcfunction06(L, CNODE_SetPatternIndex); 	lua_settable06(L, -3);
		lua_pushstring06(L, "GetPatternIndex"); lua_pushcfunction06(L, CNODE_GetPatternIndex); 	lua_settable06(L, -3);
		lua_pushstring06(L, "SetText"); lua_pushcfunction06(L, CNODE_SetText); 	lua_settable06(L, -3);
		lua_pushstring06(L, "GetText"); lua_pushcfunction06(L, CNODE_GetText); 	lua_settable06(L, -3);


		
		
		lua_pop(L,1);





		return 0;
	}

}