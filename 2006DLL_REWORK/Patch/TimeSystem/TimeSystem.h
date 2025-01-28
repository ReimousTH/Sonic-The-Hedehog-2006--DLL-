#include <xtl.h>
#include <Hook/HookBase.h>
#include <DocMarathonImp.h>
#include <LuaSystemManager.h>
#include <LuaSystem.h>




namespace TimeSystem{

	struct LTI_Vector4{
		float x;
		float y;
		float z;
		float a;
	};
	struct LTI_Vector3{
		float x;
		float y;
		float z;

	};




	/*struct LightDirecton{
		LTI_Vector3 Position;
		LTI_Vector3 Target;
	};
	};*/
	struct LightDirecton{
		LTI_Vector4 Position;
	};




	struct LightTableInfo{
		float Time;
		LTI_Vector4 SunColor;
		LTI_Vector4 AmbientColor;
		LTI_Vector4 LightColor0;
		LTI_Vector4 LightColor1;
		LTI_Vector4 BRay;
		LTI_Vector4 BMie;
		float G;
		float Bloom_MinThr;
		float Bloom_MaxThr;
		float Bloom_Scale;
	};
	struct LightTableInfoEX:LightTableInfo{
		LightDirecton LightMaxInfo0;
		LightDirecton LightMaxInfo1;
	};

	struct LightTableInfoEXSort{
		float value;
		LightTableInfoEX* LightInfo;

	};


	void GlobalInstall();
}