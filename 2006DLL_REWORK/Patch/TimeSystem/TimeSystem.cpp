#include "TimeSystem.h"
#include <Core/Debug.h>
#include <algorithm>
#include <SceneLightParam.h>

#include <Sox/Input/Listener.h>
#include <Sox/Input/Manager.h>
#include <System/Peripheral/ManagerImpXenon.h>
#include <KhronoControlInputListener.h>

#include <SceneOLSParam.h>
#include <SceneBloomParam.h>
#include <SceneKhronosParam.h>
#include <SceneParamManager.h>
#include <KhronoController.h>

#include <algorithm>

namespace TimeSystem{



	int sub_82270AE0_GetSceneLightMainParams(DWORD SceneLightParam,DWORD LightIstanceID){
		int v12 = *(int*)(SceneLightParam + 0x68); //CurrentDirectionalLight, std::vector,+0x4 ptr
		DWORD  v9 = 0x10 * LightIstanceID + v12;
		int* v10 = *(int **)(v9 + 4); //getting second vector +0x4 ptr
		int v11 = *v10; //getting first value from it
		return v11;
	}
	int sub_82270AE0_GetSceneLightSubParams(DWORD SceneLightParam,DWORD LightIstanceID){

		DWORD v8 = 0x10 * LightIstanceID;
		int v12 = *(int*)(SceneLightParam + 0x68);
		DWORD v14 = *(DWORD *)(v12 + v8 + 4);
		return  *(DWORD *)(v14 + 8);
	}



	// Helper function for linear interpolation
	inline float Lerp(float a, float b, float t) {
		return a + t * (b - a);
	}

	template<typename T>
	inline T Clamp(T value, T min, T max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

int __fastcall sub_82270AE0(int a1, double time_delta){

	// Update internal timer
	//*(float *)(a1 + 0x5C) = time_delta * 10;

	// Call original function (hook compatibility)
	BranchTo(0x82270AE0, int, a1, time_delta);

	// CUSTOM LIGHTING CODE
	std::string lightName = "Light";
	Sonicteam::DocMarathonImp* SDocMarathonState = *(Sonicteam::DocMarathonImp**)(a1 + 0x3C);
	int Scene_Params_ALL = (int)SDocMarathonState->DocGetSceneParamManager();

	Sonicteam::SceneParamManager* scene_manager =  SDocMarathonState->DocGetSceneParamManager();





	// Find light parameters
	DWORD SceneLightParam = (DWORD)BranchTo(0x825F7420, DWORD, Scene_Params_ALL, (int)&lightName); //Sonicteam::SceneLightParam vft = 8204B80C
	Sonicteam::SceneLightParam* SLPrm = (Sonicteam::SceneLightParam*)SceneLightParam;
	if (!SceneLightParam) return a1;
	Sonicteam::SceneOLSParam* ols_param =  (Sonicteam::SceneOLSParam*)scene_manager->SceneParams["OLS"].get();
	
	AddMessage("SceneLightParam : %x",SceneLightParam);

	// Get timer and normalize to 0-24 range
	float timer = *(float *)(a1 + 0x54);
	timer = fmod(timer, 24.0f);
	if (timer < 0) timer += 24.0f;

	// Get lighting state table
	std::vector<LightTableInfoEX>* lightTable = (std::vector<LightTableInfoEX>*)(0x82D36CD4);
	if (lightTable->empty()) return a1;

	// Find current and next lighting states
	LightTableInfoEX currentState;
	LightTableInfoEX nextState;
	float interpolationFactor = 0.0f;
	bool foundSegment = false;

	for (size_t i = 0; i < lightTable->size(); i++) {
		size_t nextIndex = (i + 1) % lightTable->size();
		currentState = (*lightTable)[i];
		nextState = (*lightTable)[nextIndex];

		float startTime = currentState.Time;
		float endTime = nextState.Time;

		// Handle time wrap-around (e.g., 23:00 → 01:00)
		if (endTime < startTime) endTime += 24.0f;

		// Adjust timer if crossing midnight
		float adjustedTimer = timer;
		if (adjustedTimer < startTime) adjustedTimer += 24.0f;

		// Check if in current segment
		if (adjustedTimer >= startTime && adjustedTimer < endTime) {
			interpolationFactor = (adjustedTimer - startTime) / (endTime - startTime);
			foundSegment = true;
			break;
		}
	}

	// Fallback for edge cases
	if (!foundSegment) {
		currentState = lightTable->front();
		nextState = lightTable->back();
		interpolationFactor = 1.0f;
	}

	// Clamp interpolation factor
	interpolationFactor = Clamp(interpolationFactor, 0.0f, 1.0f);

	// Get light instance parameters
	int lightInstanceID = 0;
	DWORD lightMainParams = sub_82270AE0_GetSceneLightMainParams(SceneLightParam, lightInstanceID);
	DWORD lightSubParams = sub_82270AE0_GetSceneLightSubParams(SceneLightParam, lightInstanceID);

	if (!lightMainParams || !lightSubParams) return a1;


	Sonicteam::SceneLightParam::CurrentDirectionalLight* cur_dir_light =  SLPrm->_CurrentDirectionalLight[0][0].get();


	

	// Extract light components
	XMFLOAT4* mainPosition = (XMFLOAT4*)(*(DWORD*)(lightMainParams + 0x20) + 0x10);
	XMFLOAT4* mainColor = (XMFLOAT4*)(*(DWORD*)(lightMainParams + 0x20));
	XMFLOAT4* subPosition = (XMFLOAT4*)(*(DWORD*)(lightSubParams + 0x20) + 0x10);
	XMFLOAT4* subColor = (XMFLOAT4*)(*(DWORD*)(lightSubParams + 0x20));

	// Get ambient parameters
	DWORD ambientParams = *(DWORD*)(SceneLightParam + 0x58);
	DWORD ambientInstance = *(DWORD*)(8 * lightInstanceID + ambientParams);
	XMFLOAT4* ambientColor = *(XMFLOAT4**)(ambientInstance + 0x10);

	// Interpolate lighting parameters
	if (mainPosition) {

		/*
		AddMessage("{%f,%f,%f,%f}, {%f,%f,%f,%f}",
			currentState.LightMaxInfo0.Position.x,
			currentState.LightMaxInfo0.Position.y,
			currentState.LightMaxInfo0.Position.z,
			currentState.LightMaxInfo0.Position.a,

			currentState.LightMaxInfo1.Position.x,
			currentState.LightMaxInfo1.Position.y,
			currentState.LightMaxInfo1.Position.z,
			currentState.LightMaxInfo1.Position.a
			
			);
		*/
		mainPosition->x = Lerp(currentState.LightMaxInfo0.Position.x, nextState.LightMaxInfo0.Position.x, interpolationFactor);
		mainPosition->y = Lerp(currentState.LightMaxInfo0.Position.y, nextState.LightMaxInfo0.Position.y, interpolationFactor);
		mainPosition->z = Lerp(currentState.LightMaxInfo0.Position.z, nextState.LightMaxInfo0.Position.z, interpolationFactor);
	}

	if (mainColor) {
		mainColor->x = Lerp(currentState.LightColor0.x, nextState.LightColor0.x, interpolationFactor);
		mainColor->y = Lerp(currentState.LightColor0.y, nextState.LightColor0.y, interpolationFactor);
		mainColor->z = Lerp(currentState.LightColor0.z, nextState.LightColor0.z, interpolationFactor);
	}

	if (subPosition) {
		subPosition->x = Lerp(currentState.LightMaxInfo1.Position.x, nextState.LightMaxInfo1.Position.x, interpolationFactor);
		subPosition->y = Lerp(currentState.LightMaxInfo1.Position.y, nextState.LightMaxInfo1.Position.y, interpolationFactor);
		subPosition->z = Lerp(currentState.LightMaxInfo1.Position.z, nextState.LightMaxInfo1.Position.z, interpolationFactor);
	}

	if (subColor) {
		subColor->x = Lerp(currentState.LightColor1.x, nextState.LightColor1.x, interpolationFactor);
		subColor->y = Lerp(currentState.LightColor1.y, nextState.LightColor1.y, interpolationFactor);
		subColor->z = Lerp(currentState.LightColor1.z, nextState.LightColor1.z, interpolationFactor);
	}

	if (ambientColor) {
		ambientColor->x = Lerp(currentState.AmbientColor.x, nextState.AmbientColor.x, interpolationFactor);
		ambientColor->y = Lerp(currentState.AmbientColor.y, nextState.AmbientColor.y, interpolationFactor);
		ambientColor->z = Lerp(currentState.AmbientColor.z, nextState.AmbientColor.z, interpolationFactor);
	}

	if (ols_param){
		ols_param->BMie.x = Lerp(currentState.BMie.x, nextState.BMie.x, interpolationFactor);
		ols_param->BMie.y = Lerp(currentState.BMie.y, nextState.BMie.y, interpolationFactor);
		ols_param->BMie.z = Lerp(currentState.BMie.z, nextState.BMie.z, interpolationFactor);
		ols_param->BMie.w = 1.0;

		ols_param->BRay.x = Lerp(currentState.BRay.x, nextState.BRay.x, interpolationFactor);
		ols_param->BRay.y = Lerp(currentState.BRay.y, nextState.BRay.y, interpolationFactor);
		ols_param->BRay.z = Lerp(currentState.BRay.z, nextState.BRay.z, interpolationFactor);
		ols_param->BRay.w = 1.0;

	}

	return a1;

}


#define lua_tovector3(svd,LS,cstr)  BranchTo(0X8226F0D0,int*,svd,LS,cstr)
#define lua_tovector4(svd,LS,cstr)  lua_ctovector4(svd,LS,cstr)



double __fastcall lua_tonumberfixedwithstep(lua_State* a1)
{
	double v2; // fp31

	v2 = 0.0;
	if ( lua_next(a1, 0xFFFFFFFE) )
	{
		if ( lua_isnumber(a1, 0xFFFFFFFF) )
			v2 = (float)lua_tonumber(a1, 0xFFFFFFFF);
		lua_settop(a1, 0xFFFFFFFE);
	}
	return v2;
}

extern "C" lua_ctovector4(float* buffer,lua_State* L,const char* LS){


	*buffer = 0.0;
	buffer[1] = 0.0;
	buffer[2] = 0.0;
	buffer[3] = 1.0;
	lua_pushstring(L, (char *)LS);
	lua_gettable(L, 0xFFFFFFFE);
	if ( lua_type(L, 0xFFFFFFFF) == 5 )
	{
		lua_pushnil(L);
		*buffer = lua_tonumberfixedwithstep(L);
		buffer[1] = lua_tonumberfixedwithstep(L);
		buffer[2] = lua_tonumberfixedwithstep(L);
		buffer[3] = lua_tonumberfixedwithstep(L);
		lua_settop(L, 0xFFFFFFFE);
	}
	lua_settop(L, 0xFFFFFFFE);
}



#define lua_toDirection3dsmax(svd,LS,cstr)  BranchTo(0x825F3890,int*,svd,LS,cstr)


extern "C" float lua06_tofloat(lua_State* LS,char* str){
	float result = 0;
	lua_pushstring(LS,str);
	lua_gettable(LS,-2);


	if (lua_isnumber(LS,-1)){
		result = (float)lua_tonumber(LS,-1);
	}


	lua_pop(LS,1);
	return result;
}



bool compareByTime(const LightTableInfoEX& a, const LightTableInfoEX& b)
{
	return a.Time < b.Time;
}

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
		DWORD IResourceV[2];

	
		BranchTo(0X82163620,int,(DWORD)&IResourceV, (DWORD)&IResourceV_16);
		lua_State* LS =  (lua_State*)*(DWORD*)(IResourceV[0]+0x4);
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
				test.Time = lua06_tofloat(LS,"Time");
				test.G = lua06_tofloat(LS,"G");
				test.Bloom_MinThr = lua06_tofloat(LS,"Bloom_MinThr");
				test.Bloom_MaxThr = lua06_tofloat(LS,"Bloom_MaxThr");
				test.Bloom_Scale = lua06_tofloat(LS,"Bloom_Scale");


				lua_tovector4((float*)&v499, LS, "SunColor");
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



				test.SunColor.x *= test.SunColor.a;
				test.SunColor.y *= test.SunColor.a;
				test.SunColor.z *= test.SunColor.a;
				test.SunColor.a = 1.0;


				test.AmbientColor.x *= test.AmbientColor.a;
				test.AmbientColor.y *= test.AmbientColor.a;
				test.AmbientColor.z *= test.AmbientColor.a;
				test.AmbientColor.a = 1.0;

				test.LightColor0.x *= test.LightColor0.a;
				test.LightColor0.y *= test.LightColor0.a;
				test.LightColor0.z *= test.LightColor0.a;
				test.LightColor0.a = 1.0;

				test.LightColor1.x *= test.LightColor1.a;
				test.LightColor1.y *= test.LightColor1.a;
				test.LightColor1.z *= test.LightColor1.a;
				test.LightColor1.a = 1.0;

				test.BRay.x *= test.BRay.a;
				test.BRay.y *= test.BRay.a;
				test.BRay.z *= test.BRay.a;
				test.BRay.a = 1.0;

				test.BMie.x *= test.BMie.a;
				test.BMie.y *= test.BMie.a;
				test.BMie.z *= test.BMie.a;
				test.BMie.a = 1.0;




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

		 std::sort(pro->begin(),pro->end(),compareByTime);

		//I guess ?????/

		



		//Free Resource From 06Memory
		if ( IResourceV[0] )
			BranchTo(0x82581E38,int,IResourceV[0] + 0x24);

		
	
		
		return (int)a1;


	}



	void __fastcall EngineDocOnUpdate(int a1, double a2){

		BranchTo(0x825EA610,int,a1,a2);
	}


	

	int __fastcall KhronoControlInputListenerUpdate(Sonicteam::KhronoControlInputListener* a1,SOXLISTENER_ARGSC){

	

			//DebugLogRestore::log.push_back(new std::string(ss.str().c_str()));
			int Components  = *(int*)((int)a1+0x1C);
			int Khrono_Component  = *(int*)(Components+0x8);
			Sonicteam::KhronoController* controller  = *(Sonicteam::KhronoController**)(Components+0x8);


			Sonicteam::SoX::Input::Manager* manager = mgr;

		
		
			
			double tick = delta;
			if (controller->khrono_flag_0x60 == 0){ //manual_mode, 1- default
				tick = 0.0;
			}
			else{

				
				if (manager->Gamepad.wLastButtons & XENON_GAMEPAD_Y && DebugOptions::GetEnableDevStuff() && DebugOptions::GetYKhronoTimeACC()){
					tick *= 20.0;
				}

			}

			
			controller->khrono_time_pass += tick;
			//a1->func0x18(tick);


		
		

		return 0;

	}


	int sub_8217D5C0(int a1){

		TimeSystem::SceneTimeLightEX* light = (TimeSystem::SceneTimeLightEX*)&Sonicteam::SceneTimeLight::getInstance();
		light->TLTable.clear();
		return BranchTo(0x8217D5C0,int,a1);
	}

void TimeSystem::GlobalInstall()
{




	
	WRITE_DWORD(0x82001678,sub_8217D5C0);
	WRITE_DWORD(0x8201079C,sub_82270AE0);

	INSTALL_HOOK(sub_8226FEB0);

	//WRITE_DWORD(0x82000950,EngineDocOnUpdate);
	//WRITE_DWORD(0x8204ACC8,EngineDocOnUpdate);

	//WRITE_NOP(0x825EA65C,1); why


	WRITE_WORD(0x8227059E,sizeof(LightTableInfoEX));
	WRITE_WORD(0x822705D2,sizeof(LightTableInfoEX));

	WRITE_DWORD(0x820107AC,KhronoControlInputListenerUpdate);

	//For auto-manual mode, why even need this
	WRITE_DWORD(0x82270BA8,0x480001C0);
	WRITE_DWORD(0x82270D64,0x60000000);



	//0x78 CURRENT SIZE

}


}