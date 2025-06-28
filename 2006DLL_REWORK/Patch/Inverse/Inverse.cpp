#include "Inverse.h"

namespace PostureInverseStick{


	void __fastcall Listener_Common_Input(int _this, Sonicteam::SoX::Input::Manager* PtrStructThatHasCameraObjAndKhronoController, double a3){


		short* t =  (short*)&PtrStructThatHasCameraObjAndKhronoController->Gamepad.sThumbLX;
		t[0] = -t[0];
		t[1] = -t[1];

		PtrStructThatHasCameraObjAndKhronoController->Gamepad.fX1 = -PtrStructThatHasCameraObjAndKhronoController->Gamepad.fX1;
	//	PtrStructThatHasCameraObjAndKhronoController->fY1 = -PtrStructThatHasCameraObjAndKhronoController->fY1;
		BranchTo(0x82222428,int,_this,PtrStructThatHasCameraObjAndKhronoController,a3);
	}

	int MangerProcessInput(Sonicteam::System::Peripheral::ManagerImpXenon* _this,
		Sonicteam::System::Peripheral::XINPUT_GAMEPAD_STATE* _buffer_,
		std::size_t _buffer_count)
	{
		XINPUT_STATE _buffer_state;
		if (_this->IsActive && _buffer_count) {
			for (std::size_t i = 0; i < _buffer_count; i++) {
				_buffer_[i].wButtons = 0;
				_buffer_[i].sThumbLX = 0;
				_buffer_[i].sThumbLY = 0;
				_buffer_[i].sThumbRX = 0;
				_buffer_[i].sThumbRY = 0;
				_buffer_[i].bActive = 1;
			}
		}

		if (_buffer_count ) {
			for (int i = 0; i < _buffer_count; i++) {
				int input_state = XInputGetState(i, &_buffer_state);	
				if (input_state == 0) {
					if (_buffer_[i].bActive) XInputGetCapabilities(i, 1u, &_this->GAMEPAD_CAPABILITIES[i]);
						int wbuttons = 0;
						int sThumbLX = 0;
						int sThumbLY = 0;
						int sThumbRX = 0;
						int sThumbRY = 0;

						for (int j = 0; j < 0xE; j++) {
							if ((Sonicteam::System::Peripheral::XINPUT_GAMEPAD_BUTTONS_REMAP_TABLE[j].From & _buffer_state.Gamepad.wButtons) != 0) {
								wbuttons |= Sonicteam::System::Peripheral::XINPUT_GAMEPAD_BUTTONS_REMAP_TABLE[j].To;
								if (wbuttons != 0){
		
								}
							}
						}

						if (_buffer_state.Gamepad.bLeftTrigger > 0x1Eu)
							wbuttons |= 0x4000u;
						if (_buffer_state.Gamepad.bRightTrigger > 0x1Eu)
							wbuttons |= 0x8000u;

						sThumbLX = _buffer_state.Gamepad.sThumbLX;
						if (_buffer_state.Gamepad.sThumbLX == 0xFFFF8000)
							sThumbLX = 0x8001;
						sThumbLY = _buffer_state.Gamepad.sThumbLY;
						if (_buffer_state.Gamepad.sThumbLY == 0xFFFF8000)
							sThumbLY = 0x8001;
						sThumbRX = _buffer_state.Gamepad.sThumbRX;
						if (_buffer_state.Gamepad.sThumbRX == 0xFFFF8000)
							sThumbRX = 0x8001;
						sThumbRY = _buffer_state.Gamepad.sThumbRY;
						if (_buffer_state.Gamepad.sThumbRY == 0xFFFF8000)
							sThumbRY = 0x8001;

						

						_buffer_[i].wButtons = wbuttons;
						_buffer_[i].sThumbLX = -sThumbLX;
						_buffer_[i].sThumbLY = -sThumbLY;
						_buffer_[i].sThumbRX = -sThumbRX;
						_buffer_[i].sThumbRY = -sThumbRY;
						_buffer_[i].bActive = input_state;
					
				}
			}
		}
		return 1;
	}



XMVECTOR RotationByAXIS(XMVECTOR VectorRot, XMVECTOR angledir, float angle) {
    // Define the rotation axis
    XMVECTOR rotationAxis = angledir;

    // Calculate the rotation delta
    XMVECTOR rotationDelta = XMQuaternionRotationAxis(rotationAxis, XMConvertToRadians(angle));

    // Calculate the new rotation
    return XMQuaternionMultiply(VectorRot, rotationDelta);
}


int __fastcall MainMenuTask_UI(Sonicteam::MainMenuTask *a1,float delta){



	struct test_udata {
	XMVECTOR Rotation;
    XMVECTOR Position;

}; test_udata* test_data[0xA];

test_data[0] = (test_udata*)0x82D37D00;
test_data[1] = (test_udata*)0x82D37D20;
test_data[2] = (test_udata*)0x82D37D40;

test_data[3] = (test_udata*)0x82D37D60;
test_data[4] = (test_udata*)0x82D37D80;
test_data[5] = (test_udata*)0x82D37DA0;



int SelectedIndex = a1->MMTuint0xB4;
if (SelectedIndex >= 5){
	SelectedIndex = 5;
}


Sonicteam::SoX::Input::Manager* inp_data = (Sonicteam::SoX::Input::Manager*)a1->GetCurrentDoc()->GetPlayerInput(a1->GetCurrentDoc()->GetRealControllerID(0));

switch (a1->MMCurrentCase) {
    case 0x9: {
        ATG::GAMEPAD* gc = ATG::Input::GetMergedInput();

        if (gc->fX2 != 0.0f) {
            test_data[SelectedIndex]->Rotation = RotationByAXIS(test_data[SelectedIndex]->Rotation, XMVectorSet(0, 0, 1, 0), gc->fX2 * 10.0f * delta);
            GetSelectedCharacterLUA(a1, reinterpret_cast<int>(&test_data[SelectedIndex]->Rotation), SelectedIndex);
        }

        if (gc->fY2 != 0.0f) {
            test_data[SelectedIndex]->Rotation = RotationByAXIS(test_data[SelectedIndex]->Rotation, XMVectorSet(0, 1, 0, 0), gc->fY2 * 10.0f * delta);
            GetSelectedCharacterLUA(a1, reinterpret_cast<int>(&test_data[SelectedIndex]->Rotation), SelectedIndex);
        }

        if ((gc->wLastButtons & (XINPUT_GAMEPAD_LEFT_SHOULDER | XINPUT_GAMEPAD_RIGHT_SHOULDER)) != 0) {
            float dir = (gc->wLastButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0 ? -1.0f : 1.0f;
            test_data[SelectedIndex]->Rotation = RotationByAXIS(test_data[SelectedIndex]->Rotation, XMVectorSet(1, 0, 0, 0), dir * 10.0f * delta);
            GetSelectedCharacterLUA(a1, reinterpret_cast<int>(&test_data[SelectedIndex]->Rotation), SelectedIndex);
        }
        break;
    }
}
	
	





	return BranchTo(0x824FFCF8,int,a1);

}



	void GlobalInstall()
	{

	// WRITE_DWORD(0x8200BD64,Listener_Common_Input);
	 WRITE_DWORD(0x820825F8,MangerProcessInput);

	 WRITE_DWORD(0x824FC810,0x616B0008);
	 WRITE_DWORD(0x824FC828,0x616B0004);



	// 	WRITE_DWORD(0x82039C10,MainMenuTask_UI);

	}

}