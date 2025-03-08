#include "TailsGauge.h"

#include <Player/IDynamicLink.h>
#include <Player/IPlugIn.h>
#include <boost/shared_ptr.hpp>
#include <Player/State/TailsContext.h>
#include <Player/Gauge/GaugeStandard.h>

#include <Core/Debug.h>
#include <Hook/HookNew.h>

namespace TailsGauge{



	class TailsContextEx:public Sonicteam::Player::State::TailsContext{
	public:
		boost::shared_ptr<Sonicteam::Player::IGauge> c_tails_gauge_module;
	};

	void ContextTailsOnLink(Sonicteam::Player::IDynamicLink* _this,DynContainer(Sonicteam::Player::IPlugIn) plugin){
		TailsContextEx* _this_2 = (TailsContextEx*)dynamic_cast<Sonicteam::Player::State::TailsContext*>(_this);

		if (plugin->PluginName == "gauge" && _this_2){
			_this_2->c_tails_gauge_module =  boost::dynamic_pointer_cast<Sonicteam::Player::IGauge>(plugin);
		}
		BranchTo(0x8220F330,int,_this,&plugin);
	}

	void* ContextDestroy(TailsContextEx* ex,int flag){

		ex->c_tails_gauge_module.reset(); //reset_pointer
		return BranchTo(0x82206FD8,void*,ex,flag);
	}
	

	void TailsContextOnUpdate(TailsContextEx* _this,float delta){

		BranchTo(0x8221A7D8,int,_this,delta);
		if ( Sonicteam::Player::GaugeStandard* gauge =  dynamic_cast<Sonicteam::Player::GaugeStandard*>(_this->c_tails_gauge_module.get())){
			gauge->GaugeValue = _this->TCuint0x230 * gauge->c_s_max;
		}
	}

	HOOKV3(0x8221A908,void*,TailsContextConstructor,(TailsContextEx*),(_this),TailsContextEx* _this){
		memset((void*)&_this->c_tails_gauge_module,0,0x8);
		return _this;
	}


	void GlobalInstall()
	{

		INSTALL_HOOKV3EX(TailsContextConstructor,1,false,12);
		WRITE_DWORD(0x8200B7F8,ContextTailsOnLink);
		WRITE_DWORD(0x821B5FDC,0x38600258);
		WRITE_DWORD(0x8200B860,ContextDestroy);
		WRITE_DWORD(0x8200B868,TailsContextOnUpdate);

	}

}