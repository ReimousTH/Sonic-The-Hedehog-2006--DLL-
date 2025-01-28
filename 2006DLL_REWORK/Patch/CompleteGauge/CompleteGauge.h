#pragma once

#include <Hook/HookBase.h>

#include "CompleteGauge_SonicContext.h"
#include "CompleteGauge_SonicGauge.h"
#include "CompleteGauge_SonicSound.h"
#include "CompleteGauge_SonicEffect.h"
#include "CompleteGauge_SonicStates.h"
#include "CompleteGauge_SonicModel.h"
#include "CompleteGauge_PostureCommon.h"



extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}


namespace CompleteGauge{

	namespace Gems{
		enum Gems{
			Green,
			Red,
			Blue,
			White,
			Sky,
			yellow,
			purple,
			super
		};

	}


#define IContext_TypeDescriptor 0x82B17464
#define ICommonContext_TypeDescriptor 0x82B17464
#define SonicContext_TypeDescriptor 0x82B177A8


	


	void GlobalInstall();

}



