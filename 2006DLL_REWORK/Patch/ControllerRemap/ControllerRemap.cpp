#include "ControllerRemap.h"
#include <Core/Debug.h>
#include <Hook/HookNew.h>
#include <System/Peripheral/ManagerImpXenon.h>
#include <ZLua/ZLua.h>

#include <cmath> // for std::abs

#define XENON_GAMEPAD_LS_RS				-3
#define XENON_GAMEPAD_LS_X_PLUS			XENON_GAMEPAD_LS_RS-1
#define XENON_GAMEPAD_LS_X_MINUS		XENON_GAMEPAD_LS_RS-2

#define XENON_GAMEPAD_LS_Y_PLUS			XENON_GAMEPAD_LS_RS-3
#define XENON_GAMEPAD_LS_Y_MINUS		XENON_GAMEPAD_LS_RS-4

#define XENON_GAMEPAD_RS_X_PLUS			XENON_GAMEPAD_LS_RS-5
#define XENON_GAMEPAD_RS_X_MINUS		XENON_GAMEPAD_LS_RS-6

#define XENON_GAMEPAD_RS_Y_PLUS			XENON_GAMEPAD_LS_RS-7
#define XENON_GAMEPAD_RS_Y_MINUS		XENON_GAMEPAD_LS_RS-8



namespace ControllerRemap{


	struct CRTStick{
		int XPLUS;
		int XMINUS;

		int YPLUS;
		int YMINUS;
	};
	struct ControllerRemapTable{
		std::map<size_t,size_t> Buttons;
		size_t XINPUT_LEFT_TRIGGER;
		size_t XINPUT_RIGHT_TRIGGER;
		size_t XINPUT_GAMEPAD_TRIGGER_THRESHOLD_L;
		CRTStick LS;
		CRTStick RS;
	};

	std::vector<ControllerRemapTable> _players_map;


	float GetStickNormalized(SHORT thumbX, short THUMB_DEADZONE) {
		if (abs(thumbX) < THUMB_DEADZONE) {
			return 0.0f;
		}
		float normalizedX;
		if (thumbX > 0) {
			normalizedX = (float)(thumbX - THUMB_DEADZONE) / (32767 - THUMB_DEADZONE);
		} else {
			normalizedX = (float)(thumbX + THUMB_DEADZONE) / (32767 - THUMB_DEADZONE);
		}

		return normalizedX;
	}


	
	// Helper function to clamp values to valid stick range
	short ClampStickValue(int value) {
		if (value > 32767) return 32767;
		if (value < -32768) return -32768;
		return static_cast<short>(value);
	}

	// Maps stick movements to other stick axes
	

	#pragma optimize( "", off )
	extern "C" void StickToStick(short* sThumbs_out[4], const short sThumbs_raw[4], 
		const CRTStick& LS, const CRTStick& RS) 
	{
		for (int axis = 0; axis < 4; axis++) {
			short rawValue = sThumbs_raw[axis];
			if (rawValue == 0) continue;  // Skip zero values

			int ax = axis / 2;
			int axis_idx = axis % 2;
			CRTStick CStick = (ax == 0) ? LS : RS;

			// Get target mapping
			int target;
			if (axis_idx == 0) {  // X-axis
				target = rawValue >= 0 ? CStick.XPLUS : CStick.XMINUS;
			} else {  // Y-axis
				target = rawValue >= 0 ? CStick.YPLUS : CStick.YMINUS;
			}

			// Compute output index
			int out_idx = (-(target - XENON_GAMEPAD_LS_RS - 1) / 2) - 1;

			// Compute sign using modulo
			int sign_flag = (-(target - XENON_GAMEPAD_LS_RS - 1) % 2);


			if (rawValue == SHRT_MIN) {
				rawValue = SHRT_MAX; 
			} else {
				rawValue = static_cast<short>(std::abs(rawValue)); // Safe if not SHRT_MIN
			}



		    short finalValue =  rawValue;
			if (sign_flag == 1){
				finalValue = -finalValue;
			}
	

		
			// Apply inversion using abs() as requested
	
	



			*sThumbs_out[out_idx] = finalValue;
			//AddMessage("Axis %d: raw %d -> idx %d val %d, val2 %d (sign %d)",
			//	axis, rawValue, out_idx, finalValue,finalValue, sign_flag);
		}
	}
	
	
	void StickToButton(int& wbuttons, short& sThumb, short rawValue, 
		unsigned short deadzone, unsigned short max_radius,
		int PLUS, int MINUS) 
	{
		// Apply deadzone
		short processedValue = rawValue;
		if (abs(rawValue) < deadzone) {
			processedValue = 0;
		} else if (rawValue > 0) {
			processedValue = rawValue - deadzone;
		} else {
			processedValue = rawValue + deadzone;
		}

		// Calculate normalized value using max_radius (0x5E0E)
		float normalized = static_cast<float>(processedValue) / max_radius;

		// Track if we've mapped to a button
		bool mapped = false;

		// Handle positive axis mapping
		if (PLUS > 0 && normalized > 0.5f) {
			wbuttons |= PLUS;
			mapped = true;
		}
		// Handle negative axis mapping
		if (MINUS > 0 && normalized < -0.5f) {
			wbuttons |= MINUS;
			mapped = true;
		}

		// Only disable analog output for the MAPPED direction
		if (mapped) {
			sThumb = 0;  // Disable entire axis output
		} else {
			sThumb = rawValue;  // Preserve analog value
		}
	}


	void ProcessStickMappings(
		short* output[4], 
		const short raw[4],
		int& buttons,
		const CRTStick& LS,
		const CRTStick& RS,
		unsigned short deadzone,
		unsigned short max_radius
		) {
			// Axis indices: 0=LX, 1=LY, 2=RX, 3=RY
			 int LX = 0, LY = 1, RX = 2, RY = 3;
			int temp[4] = {0};

			// First pass: Stick-to-stick mapping
			for (int axis = 0; axis < 4; axis++) {
				const short val = raw[axis];
				if (val == 0) continue;

				// Get direction mappings
				const bool positive = val > 0;
				const int map_val = positive ? abs(val) : -abs(val);
				const int mapping = positive ? 
					(axis == LX ? LS.XPLUS : 
					axis == LY ? LS.YPLUS :
					axis == RX ? RS.XPLUS : RS.YPLUS) :
					(axis == LX ? LS.XMINUS : 
					axis == LY ? LS.YMINUS :
					axis == RX ? RS.XMINUS : RS.YMINUS);

				if (!mapping) {
					temp[axis] += val;  // No mapping - preserve value
					continue;
				}

				// Apply stick-to-stick mapping
				const int target = mapping >> 16;  // High word stores axis index
				const int operation = mapping & 0xFFFF;  // Low word stores operation
				temp[target] += (operation == 1) ? map_val : -map_val;
			}

			// Second pass: Stick-to-button mapping and deadzone handling
			for (int axis = 0; axis < 4; axis++) {
				const short val = raw[axis];

				// Apply deadzone
				if (abs(val) < deadzone) {
					*output[axis] = 0;
					continue;
				}

				// Calculate normalized value
				float normalized = (val > 0) ? 
					static_cast<float>(val - deadzone) / (32767 - deadzone) :
				static_cast<float>(val + deadzone) / (32767 - deadzone);

				// Get button mappings
				const int plus_map = (axis == LX || axis == RX) ? 
					(axis == LX ? LS.XPLUS : RS.XPLUS) : 
					(axis == LY ? LS.YPLUS : RS.YPLUS);
				const int minus_map = (axis == LX || axis == RX) ? 
					(axis == LX ? LS.XMINUS : RS.XMINUS) : 
					(axis == LY ? LS.YMINUS : RS.YMINUS);

				// Apply stick-to-button mapping
				bool mapped = false;
				if (plus_map > 0 && normalized > 0.5f) {
					buttons |= plus_map;
					mapped = true;
				}
				if (minus_map > 0 && normalized < -0.5f) {
					buttons |= minus_map;
					mapped = true;
				}

				// Set final output
				*output[axis] = mapped ? 0 : ClampStickValue(temp[axis]);
			}
	}

	int MangerProcessInput(Sonicteam::System::Peripheral::ManagerImpXenon* _this,Sonicteam::System::Peripheral::XINPUT_GAMEPAD_STATE* _buffer_,int _buffer_count)
	{
		XINPUT_STATE _buffer_state;
		if (_this->IsActive && _buffer_count){
			for (int i = 0;i<_buffer_count;i++){
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
					short sThumbLX = 0;
					short sThumbLY = 0;
					short sThumbRX = 0;
					short sThumbRY = 0;


					//KeyMapRemap
		

					for (std::map<size_t,size_t>::iterator it =_players_map[i].Buttons.begin();it != _players_map[i].Buttons.end();it++ ){
						if ((it->first & _buffer_state.Gamepad.wButtons) != 0){
							wbuttons |= it->second;	
						}
						//AddMessage("%x,%x,%x",wbuttons,it->first,it->second);
					}

					/*
					//06 Remap
					for (int j = 0; j < 0xE; j++) {
						if ((Sonicteam::System::Peripheral::XINPUT_GAMEPAD_BUTTONS_REMAP_TABLE[j].From & _buffer_state.Gamepad.wButtons) != 0) {
							wbuttons |= Sonicteam::System::Peripheral::XINPUT_GAMEPAD_BUTTONS_REMAP_TABLE[j].To;
						}
					}
					*/

					ControllerRemapTable* table = &_players_map[i];
				
					
					//LEFT_TRIGGER (REMAP TO OTHER TRIGGER IF)
					if (table->XINPUT_LEFT_TRIGGER == 0x4000u) {
						if (_buffer_state.Gamepad.bLeftTrigger > table->XINPUT_GAMEPAD_TRIGGER_THRESHOLD_L) {
							wbuttons |= 0x4000u;
						}
					} 
					else if (table->XINPUT_LEFT_TRIGGER == 0x8000u) {
						if (_buffer_state.Gamepad.bRightTrigger > table->XINPUT_GAMEPAD_TRIGGER_THRESHOLD_L) {
							wbuttons |= 0x4000u;
						}
					}
					else{
						if (_buffer_state.Gamepad.bLeftTrigger > table->XINPUT_GAMEPAD_TRIGGER_THRESHOLD_L) {
							wbuttons |= table->XINPUT_LEFT_TRIGGER;
						}
					}


					//RIGHT_TRIGGER (REMAP TO OTHER TRIGGER IF)
					if (table->XINPUT_RIGHT_TRIGGER == 0x8000u) {
						if (_buffer_state.Gamepad.bRightTrigger > table->XINPUT_GAMEPAD_TRIGGER_THRESHOLD_L) {
							wbuttons |= 0x8000u;
						}
					} 
					else if (table->XINPUT_RIGHT_TRIGGER == 0x4000u) {
						if (_buffer_state.Gamepad.bLeftTrigger > table->XINPUT_GAMEPAD_TRIGGER_THRESHOLD_L) {
							wbuttons |= 0x8000u;
						}
					}
					else{
						if (_buffer_state.Gamepad.bRightTrigger > table->XINPUT_GAMEPAD_TRIGGER_THRESHOLD_L) {
							wbuttons |= table->XINPUT_RIGHT_TRIGGER;
						}
					}



	
				

					short* sThumbs_out[] = {&sThumbLX, &sThumbLY, &sThumbRX, &sThumbRY};
					short sThumbs_raw[] = {
						_buffer_state.Gamepad.sThumbLX,
						_buffer_state.Gamepad.sThumbLY,
						_buffer_state.Gamepad.sThumbRX,
						_buffer_state.Gamepad.sThumbRY
					};


					// 1. First apply stick-to-stick mappings
					StickToStick(sThumbs_out, sThumbs_raw, table->LS, table->RS);


					/*
					StickToButton(wbuttons,sThumbLX,sThumbLX,XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,0x5E0E,table->LS.XPLUS,table->LS.XMINUS); //LSX
					StickToButton(wbuttons,sThumbLY,sThumbLY,XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,0x5E0E,table->LS.YPLUS,table->LS.YMINUS); //LSY
			
					StickToButton(wbuttons,sThumbRX,sThumbRY,XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,0x5E0E,table->RS.XPLUS,table->RS.XMINUS); //RSX
					StickToButton(wbuttons,sThumbRY,sThumbRY,XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,0x5E0E,table->RS.YPLUS,table->RS.YMINUS); //RSY
					*/

	
			
					if (sThumbLX == 0xFFFF8000)
						sThumbLX = 0x8001;

					if (sThumbLY == 0xFFFF8000)
						sThumbLY = 0x8001;
			
					if (sThumbRX == 0xFFFF8000)
						sThumbRX = 0x8001;
			
					if (sThumbRY == 0xFFFF8000)
						sThumbRY = 0x8001;




					//mapped to button (LSX+)
					_buffer_[i].wButtons = wbuttons;
			

					_buffer_[i].sThumbLX = sThumbLX;
					_buffer_[i].sThumbLY = -sThumbLY;
					_buffer_[i].sThumbRX = sThumbRX;
					_buffer_[i].sThumbRY = -sThumbRY;
				

					/*
					_buffer_[i].sThumbLX = sThumbLX;
					_buffer_[i].sThumbLY = -sThumbLY;
					_buffer_[i].sThumbRX = sThumbRX;
					_buffer_[i].sThumbRY = -sThumbRY
					*/


					_buffer_[i].bActive = input_state;

				}
			}
		}
		return 1;
	}



	CRTStick lua_getstick(lua_State* L, const char* str, int idx) {
		CRTStick _stick = {0}; // Initialize all fields to 0

		lua_pushstring(L, str);
		lua_gettable(L, idx - 1);  // Get the stick table

		if (lua_istable(L, -1)) {
			// Get XPLUS (index 0)
			lua_rawgeti(L, -1, 1);  // Lua arrays are 1-based
			if (lua_isnumber(L, -1)) _stick.XPLUS = lua_tonumber(L, -1);
			lua_pop(L, 1);

			// Get XMINUS (index 1)
			lua_rawgeti(L, -1, 2);
			if (lua_isnumber(L, -1)) _stick.XMINUS = lua_tonumber(L, -1);
			lua_pop(L, 1);

			// Get YPLUS (index 2)
			lua_rawgeti(L, -1, 3);
			if (lua_isnumber(L, -1)) _stick.YPLUS = lua_tonumber(L, -1);
			lua_pop(L, 1);

			// Get YMINUS (index 3)
			lua_rawgeti(L, -1, 4);
			if (lua_isnumber(L, -1)) _stick.YMINUS = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}

		lua_pop(L, 1);  // Pop the stick table
		return _stick;
	}

	void GlobalInstall()
	{

		_players_map.clear();
		ZLua lua_file = ZLua("game:\\common\\KeyMap.lua");
		lua_file.DoFile(true);

		lua_State* L = lua_file.L;


	


		lua_getglobal(L, "p_gamepad");

		if (!lua_istable(L, -1)) {
			lua_pop(L, 1);
			return;
		}

		
		for (int player_idx = 1; player_idx <= 4; player_idx++) {
			ControllerRemapTable _map;
			lua_pushnumber(L, player_idx);
			lua_gettable(L, -2);

			lua_pushstring(L, "Buttons");
			lua_gettable(L, -2);
			for (unsigned int button_mask = 0x0001; button_mask <= 0x8000; button_mask <<= 1) {
				lua_pushnumber(L, button_mask);
				lua_gettable(L, -2);
				if (lua_isnumber(L, -1)) {
					_map.Buttons[button_mask] = (size_t)lua_tonumber(L, -1);
				}
				lua_pop(L, 1);
			}
			lua_pop(L, 1);



			_map.LS =  lua_getstick(L,"XINPUT_LS",-1);
			_map.RS =  lua_getstick(L,"XINPUT_RS",-1);
	

			lua_pushstring(L, "XINPUT_LEFT_TRIGGER");
			lua_gettable(L, -2);
			_map.XINPUT_LEFT_TRIGGER = lua_tonumber(L, -1);
			lua_pop(L, 1);

			lua_pushstring(L, "XINPUT_RIGHT_TRIGGER");
			lua_gettable(L, -2);
			_map.XINPUT_RIGHT_TRIGGER = lua_tonumber(L, -1);
			lua_pop(L, 1);


			lua_pushstring(L, "XINPUT_GAMEPAD_TRIGGER_THRESHOLD");
			lua_gettable(L, -2);
			_map.XINPUT_GAMEPAD_TRIGGER_THRESHOLD_L = lua_tonumber(L, -1);
			lua_pop(L, 1);



			lua_pop(L, 1);
			_players_map.push_back(_map);
		}
		lua_pop(L, 1);

	
		WRITE_DWORD(0x820825F8,MangerProcessInput);

	}

}