#include <xtl.h>
#include <Hook/HookBase.h>
#include <DocMarathonImp.h>
#include <LuaSystemManager.h>
#include <LuaSystem.h>

#include <string>
#include <SceneTimeLight.h>




namespace TimeSystem{


	struct SceneTimeLightTableEX:public Sonicteam::SceneTimeLightTable{
		Sonicteam::SSLVECTOR LightMaxInfo0;
		Sonicteam::SSLVECTOR LightMaxInfo1;
	};


	class SceneTimeLightEX{
	public:
		SceneTimeLightEX(){

		}
		~SceneTimeLightEX(){

		}

		std::vector<SceneTimeLightTableEX> TLTable; // 0
		std::string TLFile; // wvo_table.lua //0x10

	};



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