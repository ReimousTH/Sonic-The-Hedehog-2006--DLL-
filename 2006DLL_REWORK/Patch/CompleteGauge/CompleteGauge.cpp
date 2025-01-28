#include "CompleteGauge.h"

namespace CompleteGauge{


	void GlobalInstall(){

		CompleteGauge::GlobalInstall_CommonPosture();
		CompleteGauge::GlobalInstall_SonicSound();
		CompleteGauge::GlobalInstall_SonicEffect();
		CompleteGauge::GlobalInstall_SonicContext();
		CompleteGauge::GlobalInstall_SonicStates();
		CompleteGauge::GlobalInstall_SonicGauge();
		CompleteGauge::GlobalInstall_SonicModel();

	}

	
}



