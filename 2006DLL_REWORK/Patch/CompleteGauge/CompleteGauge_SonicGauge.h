#pragma once


#include <Hook/HookBase.h>
#include <Player/Gauge/SonicGauge.h>


namespace CompleteGauge{
	
	


	class SonicGaugeExtended:public Sonicteam::Player::SonicGauge{

	public:
		float c_current_gauge_maturity;
		float c_current_gauge_maturity_add;
		float c_current_gauge_fatigue;
		float c_gauge_fatigue_green; 
		float c_gauge_fatigue_red; 
		float c_gauge_fatigue_blue; 
		float c_gauge_fatigue_white; 
		float c_gauge_fatigue_sky; 
		float c_gauge_fatigue_yellow; 
		float c_gauge_fatigue_purple; 
		float c_gauge_fatigue_super; 


		bool HomingSmash;
		bool HomingSmash_Release;


		int c_current_gauge_level;
		bool IsFull;

		void ResetMaturityAdd(){
				c_current_gauge_maturity_add = 0;
		}
		float GetMaturityToAdd(){
			return c_current_gauge_maturity_add;
		}




	};

	
	void GlobalInstall_SonicGauge();
	
}