#pragma once
#include "CompleteGauge_SonicModel.h"


namespace CompleteGauge{
	


	
	void Sonic_Model_Standard_WeaponsFlags(int _this,int flags){


		DWORD _fake_boost_container[2] = {0};
		//BranchTo(0x822387C8,int,_this - 0x28,4,0.0);
	//	std::stringstream sus;
	//	sus << std::hex << flags;
	//	DebugLogV2::PrintNextFixed(sus.str());

		//SpeedGem
		if ((flags & 0x400) != 0){
		
		
			BranchTo(0x822387C8,int,_this - 0x28,4,0.0);

		}
		else{
	      BranchTo(0x822387C8,int,_this - 0x28,5,0.0);
		}
	
			


	

	}


	void GlobalInstall_SonicModel()
	{
	
	//	WRITE_DWORD(0x8200CF98,Sonic_Model_OnUnknownFlagsC8);
	//	WRITE_DWORD(0x8200CF9C,Sonic_Model_OnUnknownFlagsC8);
	//	WRITE_DWORD(0x8200CFA0,Sonic_Model_OnUnknownFlagsC8);
	//	WRITE_DWORD(0x8200CFA4,Sonic_Model_OnUnknownFlagsC8);
	//	WRITE_DWORD(0x8200CFA8,Sonic_Model_OnUnknownFlagsC8);
	//	WRITE_DWORD(0x8200CFAC,Sonic_Model_OnUnknownFlagsC8);
	//	WRITE_DWORD(0x8200CFB0,Sonic_Model_OnUnknownFlagsC8);


	//	WRITE_DWORD(0x822377B8,0x4E800020);
		WRITE_DWORD(0X822383C4,0X60000000);
		WRITE_DWORD(0x822371A8,0x4E800020);
		
		WRITE_DWORD(0x8200CFB4 ,Sonic_Model_Standard_WeaponsFlags);
		WRITE_DWORD(0x82237804,0x60000000);
		
	}
	
}