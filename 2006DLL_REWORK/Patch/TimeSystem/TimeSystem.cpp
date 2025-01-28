#include "TimeSystem.h"

namespace TimeSystem{


int __fastcall sub_82270AE0(int a1, double time_delta){

	*(float *)(a1 + 0x5C) = time_delta;
	BranchTo(0x82270AE0,int,a1,time_delta);
	//CUSTOM CODE TIME
	std::string s1 = "Light";
	Sonicteam::DocMarathonImp * SDocMarathonState = *(Sonicteam::DocMarathonImp **)(a1 + 0x3C);
	int Scene_Params_ALL =  SDocMarathonState->DocGetSceneParamManager();
	
	DWORD SceneLightParam =  (DWORD)BranchTo(0x825F7420,DWORD,Scene_Params_ALL,(int)&s1); // MarahonFindPluginByName
	if (SceneLightParam){


		int v12 = *(int*)(SceneLightParam + 0x68);


		int LightIstanceID = 0; //Should Be cycled
		int v8 = 0x10 * LightIstanceID;
		DWORD  v9 = 0x10 * LightIstanceID + v12;
		int* v10 = *(int **)(v9 + 4);
		int v11 = *v10;
		DWORD v14 = *(DWORD *)(v12 + v8 + 4);
		DWORD v15 = *(DWORD *)(v14 + 8);




		float timer = *(float *)(a1 + 0x54);

	

		DWORD v11_s = *(int*)(	BranchTo(0x82270AE0,DWORD,SceneLightParam,LightIstanceID) + 0x20);
		XMFLOAT4* trg = (XMFLOAT4*)(v11_s + 0x10);
		XMFLOAT4* MainColor = (XMFLOAT4*)(v11_s);


		DWORD v11_s_2 = *(int*)(BranchTo(0x82270AE0,DWORD,SceneLightParam,LightIstanceID) + 0x20);
		XMFLOAT4* trg_s = (XMFLOAT4*)(v11_s_2 + 0x10);
		XMFLOAT4* SubColor = (XMFLOAT4*)(v11_s_2);


		//	XMFLOAT4* trg_s = (XMFLOAT4*)(sub_82270AE0_GetSceneLightParams(SceneLightParam,0) + 0x10);


		LightTableInfoEX Current;
		LightTableInfoEX Next;
		std::vector<LightTableInfoEX>* pro =  (std::vector<LightTableInfoEX>*)(0x82D36CD4);
		if (pro->size() > 0 ){
			for (int i = 0;i<pro->size();i++){
				Current = (*pro)[i];
				Next =(*pro)[i+1];
				if (Current.Time <= timer &&	timer <= Next.Time){
					break;
				}
			}
		}

		float TM = (timer -  Current.Time) / (Next.Time - Current.Time);


		DWORD v5 = *(DWORD*)(SceneLightParam +0x58);
		DWORD AmbientSceneParam = *(DWORD *)(8 * LightIstanceID + v5);
		XMFLOAT4* SAmbientSceneParam = *(XMFLOAT4**)(AmbientSceneParam + 0x10);





		if (pro->size() > 0)
		{
			if (trg){

				trg->x = ((1.0 - TM) *  Current.LightMaxInfo0.Position.x) + (Next.LightMaxInfo0.Position.x * TM);
				trg->y = ((1.0 - TM) *  Current.LightMaxInfo0.Position.y) + (Next.LightMaxInfo0.Position.y * TM);
				trg->z = ((1.0 - TM) *  Current.LightMaxInfo0.Position.z) + (Next.LightMaxInfo0.Position.z * TM);

			}

			if (MainColor){
				MainColor->x = ((1.0 - TM) *  Current.LightColor0.x) + (Next.LightColor0.x * TM);
				MainColor->y = ((1.0 - TM) *  Current.LightColor0.y) + (Next.LightColor0.y * TM);
				MainColor->z = ((1.0 - TM) *  Current.LightColor0.z) + (Next.LightColor0.z * TM);
			}


			if (trg_s){
				trg_s->x = ((1.0 - TM) *  Current.LightMaxInfo1.Position.x) + (Next.LightMaxInfo1.Position.x * TM);
				trg_s->y = ((1.0 - TM) *  Current.LightMaxInfo1.Position.y) + (Next.LightMaxInfo1.Position.y * TM);
				trg_s->z = ((1.0 - TM) *  Current.LightMaxInfo1.Position.z) + (Next.LightMaxInfo1.Position.z * TM);
			}
			if (SubColor){
				SubColor->x = ((1.0 - TM) *  Current.LightColor1.x) + (Next.LightColor1.x * TM);
				SubColor->y = ((1.0 - TM) *  Current.LightColor1.y) + (Next.LightColor1.y * TM);
				SubColor->z = ((1.0 - TM) *  Current.LightColor1.z) + (Next.LightColor1.z * TM);
			}

			if (SAmbientSceneParam){
				SAmbientSceneParam->x = ((1.0 - TM) *  Current.AmbientColor.x) + (Next.AmbientColor.x * TM);
				SAmbientSceneParam->y = ((1.0 - TM) *  Current.AmbientColor.y) + (Next.AmbientColor.y * TM);
				SAmbientSceneParam->z = ((1.0 - TM) *  Current.AmbientColor.z) + (Next.AmbientColor.z * TM);
			}

		}


	}





	return a1;
}


#define lua_tovector4(svd,LS,cstr)  BranchTo(0X8226F0D0,int*,svd,LS,cstr)
#define lua_toDirection3dsmax(svd,LS,cstr)  BranchTo(0x825F3890,int*,svd,LS,cstr)

HOOK_EXTERN_C(int,__fastcall,sub_8226FEB0,0x8226FEB0,DWORD *a1, DWORD *a2){


	
	float v50[32]; // [sp+90h] [-150h] BYREF
	char v51[16]; // [sp+110h] [-D0h] BYREF

	char v53[16]; // [sp+130h] [-B0h] BYREF
	char v54[16]; // [sp+140h] [-A0h] BYREF
	char v55[16]; // [sp+150h] [-90h] BYREF

	DWORD v4 = a1[2];
	DWORD v5 = a1[1];
		DWORD IResource[2];
		byte v49[0x1c];
		int SunColorV4[4];
		int AmbientColorV4[4];
		int LightColor0V4[4];
		int LightColor1V4[4];
		int BrayV4[4];
		int BmieV4[4];
		std::string* t =   (std::string *)(a1 + 4);
		t->assign(*(std::string*)a2);
		std::string IResourceV_16(t->c_str());



		Sonicteam::LuaSystem* IResourceV;
		Sonicteam::LuaSystem::LoadInitResource(IResourceV,IResourceV_16);


		lua_State* LS =  IResourceV->Lua_State;
		lua_getglobal(LS,"TimeLightTable");
		lua_pushnil(LS); //For Deep Lua Searc
		float v52[32];
		memset(&v52,0,32*4);
		float v499[4];
		LightTableInfoEX test;
		memset(&test,0,sizeof(LightTableInfoEX));
		std::vector<LightTableInfoEX>* pro = (std::vector<LightTableInfoEX>*)a1;
		if (pro->size() > 0){
			pro->clear();
		}
		while (lua_next(LS,-2) != 0){
			if (lua_istable(LS,-1)){
				
				lua_pushstring06(LS,"Time");
				test.Time = lua_tonumber06(LS,-2);

				lua_pushstring06(LS,"G");
				test.G = lua_tonumber06(LS,-2);


				lua_pushstring06(LS,"Bloom_MinThr");
				test.Bloom_MinThr = lua_tonumber06(LS,-2);


				lua_pushstring06(LS,"Bloom_MaxThr");
				test.Bloom_MaxThr = lua_tonumber06(LS,-2);

				lua_pushstring06(LS,"Bloom_Scale");
				test.Bloom_Scale = lua_tonumber06(LS,-2);

				lua_pushstring06(LS,"SunColor");
				lua_tovector4((float*)&v499, LS, -2);
				memcpy(&test.SunColor,&v499,16);

		
				lua_tovector4((float *)v499, LS, "AmbientColor");
				memcpy(&test.AmbientColor,&v499,16);

			
				lua_tovector4((float *)&v499, LS, "LightColor0");
				memcpy(&test.LightColor0,&v499,16);


			
		     	lua_tovector4((float *)&v499, LS, "LightColor1");
			    memcpy(&test.LightColor1,&v499,16);


			
				lua_tovector4((float *)&v499, LS, "BRay");
				memcpy(&test.BRay,&v499,16);

				lua_tovector4((float *)&v499, LS, "BMie");
				memcpy(&test.BMie,&v499,16);




				lua_toDirection3dsmax((float*)&v499,LS,"LightColor0_Direction_3dsmax");
				memcpy(&test.LightMaxInfo0.Position,&v499,16);
				lua_toDirection3dsmax((float*)&v499,LS,"LightColor1_Direction_3dsmax");
				memcpy(&test.LightMaxInfo1.Position,&v499,16);


				//ShowXenonMessage(L"MSG",test.LightMaxInfo0.Position.x,0);
				/*
				lua_pushstring(LS,"LightColor0_Direction_3dsmax");
				lua_gettable(LS,-2);
				lua_tovector4((float *)&v499, LS, "Position");
				memcpy(&test.LightMaxInfo0.Position,&v499,12);
				lua_tovector4((float *)v499, LS, "Target");
				memcpy(&test.LightMaxInfo0.Target,&v499,12);
				lua_pop(LS,1); // Back to TimeLightTable

				lua_pushstring(LS,"LightColor1_Direction_3dsmax");
				lua_gettable(LS,-2);
				lua_tovector4((float *)&v499, LS, "Position");
				memcpy(&test.LightMaxInfo1.Position,&v499,12);
				lua_tovector4((float *)v499, LS, "Target");
				memcpy(&test.LightMaxInfo1.Target,&v499,12);
				lua_pop(LS,1); // Back to TimeLightTable
				*/

				pro->push_back(test);

	

				


		
				
			}


			lua_pop(LS,1);

		}

		 lua_pop(LS, 1); // pop table




		//Free Resource From 06Memory
		if (IResourceV) IResourceV->Release();
	


		
	
		
		return (int)a1;


	}



	void __fastcall EngineDocOnUpdate(int a1, double a2){

		BranchTo(0x825EA610,int,a1,a2);
	}



void TimeSystem::GlobalInstall()
{




	
	WRITE_DWORD(0x8201079C,sub_82270AE0);

	INSTALL_HOOK(sub_8226FEB0);

	//WRITE_DWORD(0x82000950,EngineDocOnUpdate);
	//WRITE_DWORD(0x8204ACC8,EngineDocOnUpdate);

	WRITE_NOP(0x825EA65C,1);


	WRITE_WORD(0x8227059E,sizeof(LightTableInfoEX));
	WRITE_WORD(0x822705D2,sizeof(LightTableInfoEX));
	//0x78 CURRENT SIZE

}


}