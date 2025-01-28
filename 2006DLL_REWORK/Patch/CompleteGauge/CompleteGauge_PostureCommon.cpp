#pragma once
#include "CompleteGauge_PostureCommon.h"


#include "Player/State/CommonObject.h"
#include "Player/State/SonicContext.h"
#include "Player/Gauge/SonicGauge.h"
#include "Player/State/CommonContext.h"


namespace CompleteGauge{
	
	bool pass;
	int __fastcall PostureControlCmn(int a1, int *a2, int *a3)
	{

		

		XMFLOAT4 PostRotationInterpolate= *(XMFLOAT4*)(a1 + 0xA0);  // always (0.0.0.1) but if change it sees it restore back to normal rotation with lerp

		XMFLOAT4 Position = *(XMFLOAT4*)(a1 + 0xB0); //Position
		XMFLOAT4 Rotation = *(XMFLOAT4*)(a1 + 0xC0); //Rotation

		//0xD0 (nothing)




		//0xC0 = Rotation(Quaternion)

		BranchTo(0x82200538,int,a1,a2,a3);
		DWORD v18 =   BranchTo(0x821FE130,int,a1);
		DWORD* v19 = (DWORD *)BranchTo(0x821FDC28,int,a1, v18);
		// DWORD v22 =  BranchTo(0x821F1578,int,a1,v19);
		DWORD* v22 = (DWORD *)(a1 + 0xF0); // PostureExportFlag
		//Edge(nvm Up-Down axis)


		DWORD Edge =  *(DWORD*)(a1 + 0x388);


		//a1 + 0xB0 (Vector) Chr Position
		//a1 + 0x260 (Class)
		// Use them both for Grab


	
	
		if ((*v22 & CC_HEADONWALL) && (*v22 & CC_WALLBRUSHING) && (*v22 & CC_FALL)) {
			// Your code here
		


			DWORD t =  *(DWORD*)(a1 + 0x120 + 0x24);
			DWORD z  = *(DWORD*)(t + 0x14); // Count of corners that character touches

			if (z > 0){
				*(DWORD*)(Edge + 0x30) = 0x80;
				*(byte*)(Edge + 0x4C) = 0x0;
				*v22 = *v22 |  0x80;
			}

		}

		return 0;

	}

	void GlobalInstall_CommonPosture()
	{
		WRITE_DWORD(0x82009050,PostureControlCmn);
		
	}
	
}