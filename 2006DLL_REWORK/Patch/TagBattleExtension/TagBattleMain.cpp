#include "TagBattleMain.h"
#include <xboxmath.h>
#include <map>
#include <sstream>
#include <string>
#include <set>
#include "AtgInput.h"

#include <CsdObject.h>
#include <Sox/StepableThread.h>
using namespace TagBattleMain;



#define LimitCHR 9


static PLAYERS_DATA P_DATA[5] = {
	{{0.12, 0.12, -0.01, 0.98}, {-255, -95, 300, 1}, 0,0, 0, 0, {0, 0}},
	{{0.1192633733, -0.09836506844, 0.01662381552, 0.9824031591}, {235, -95, 300, 1}, 0,0, 1, 0, {0, 0}},	
	{{0.1981271207, 0.1254334599, -0.01419138722, 0.966481328}, {-235, 95, 300, 1}, 0,0, 2, 0, {0, 0}},
	{{0.1998167932, -0.095509395, 0.0001695175597, 0.9696561694}, {235, 95, 300, 1}, 0,0, 0, 0, {0, 0}},
	{{0,0,0,1}, {-1280,-720,0,0}, 0,0, 1, 0, {0, 0}}
};
std::map<int, PLAYERS_DATA*> TagBattleMain::ordered_input;
std::vector<int> TagBattleMain::origin_index_order;


int InitINPUT(){
	ordered_input[1] = &P_DATA[0];
	ordered_input[2] = 0;
	ordered_input[3] = 0;
	ordered_input[4] = 0;
	return 0;
}


const char* CHRS[] = {
	"sonic",
	"shadow",
	"silver",
	"tails",
	"amy",
	"rouge",
	"knuckles",
	"omega",
	"blaze"
};

const char* CHRS_S[] = {
	"sonic_new",
	"shadow",
	"silver",
	"tails",
	"amy",
	"rouge",
	"knuckles",
	"omega",	
	"blaze"
};

const char* battle_missions[] = {
	":scripts/mission/4200/mission_4001.lua",
	":scripts/mission/4200/mission_4002.lua",
	":scripts/mission/4200/mission_4003.lua",
	":scripts/mission/4200/mission_4004.lua",
	":scripts/mission/4200/mission_4005.lua",
	":scripts/mission/4200/mission_4006.lua",
	":scripts/mission/4200/mission_4007.lua",
	":scripts/mission/4200/mission_4008.lua",
	":scripts/mission/4200/mission_4009.lua"
};

int GetSlot(PLAYERS_DATA* dt){

	for (std::map<int,PLAYERS_DATA*>::iterator it  = ordered_input.begin();it != ordered_input.end();it++){
		if (it->second == dt){
			return it->first;
		}
	}

}

int TakeSlot(PLAYERS_DATA* dt){
	for (std::map<int,PLAYERS_DATA*>::iterator it  = ordered_input.begin();it != ordered_input.end();it++){
		if ( it->second != 0 &&  (it->second == dt || it->second->CurrentController == dt->CurrentController)){
			return -1;
		}
	}



	for (std::map<int,PLAYERS_DATA*>::iterator it  = ordered_input.begin();it != ordered_input.end();it++){
		if (it->second == 0){
			it->second = dt;
			return it->first;
		}
	}

}
int FreeSlot(PLAYERS_DATA* dt){
	int slot =GetSlot(dt);
	ordered_input[slot] = 0;
	return slot;

}


int GetActivePlayers(){
	int ppl = 0;
	for (std::map<int,PLAYERS_DATA*>::iterator it  = ordered_input.begin();it != ordered_input.end();it++){
		if ( it->second != 0 && (it->second->selected & (1|2)) != 0 ){
			ppl++;
		}
	}
	return ppl;

}
int GetReadyPlayers(){
	int ppl = 0;
	for (std::map<int,PLAYERS_DATA*>::iterator it  = ordered_input.begin();it != ordered_input.end();it++){
		if ( it->second != 0 && (it->second->selected & 2) != 0 ){
			ppl++;
		}
	}
	return ppl;

}
PLAYERS_DATA* GetPrimaryPlayer(){

	for (std::map<int,PLAYERS_DATA*>::iterator it  = ordered_input.begin();it != ordered_input.end();it++){
		if (it->second != 0 &&(it->second->selected & (1|2)) != 0 ){
			return it->second;
		}
	}
	return &P_DATA[0];
}


void ProcessDButton(PLAYERS_DATA* plr,Sonicteam::Player::Input::IListenerInputStruc01* inp_data,int OriginButton,int out){

	//remove

	//HOLDING BUTTON
	if ((inp_data->wLastButtons & OriginButton) != 0){
			plr->input_data ^= out; 
	}

	else{
		plr->input_data &= ~(out);
	}

}


void ProcessButton(PLAYERS_DATA* plr, Sonicteam::Player::Input::IListenerInputStruc01* inp_data, int OriginButton, int StartFrameID, int HOLDFrameID, int ReleaseFrameID) {

	// if BUTTON IS HELD
	if ((inp_data->wLastButtons & OriginButton) != 0) {
		// Check if the button was previously released
		if ((plr->input_data & (StartFrameID | HOLDFrameID) ) != 0) {
			plr->input_data |= HOLDFrameID; // A_HOLD_FRAME;
			plr->input_data &= ~StartFrameID;
		}
		else {
			plr->input_data |= StartFrameID; // A_START_FRAME
		}
	}
	// Check if the button was released
	else if ((inp_data->wLastButtons & OriginButton) == 0 && (plr->input_data & (HOLDFrameID | StartFrameID)) != 0) {
		plr->input_data &= ~(HOLDFrameID | StartFrameID);
		plr->input_data |= ReleaseFrameID; // CONFIRM_BUTTON
	}
	// Check if the button was previously held
	else if ((inp_data->wLastButtons & OriginButton) == 0) {
		plr->input_data &= ~(StartFrameID | HOLDFrameID | ReleaseFrameID);
	}
}

void ProcessFourInputs(Sonicteam::MainMenuTask *a1,float delta){
	for (int i = 0;i<4;i++){
		PLAYERS_DATA* plr =  &P_DATA[i];
		//HELP A BIT
		if (true){

			

			Sonicteam::Player::Input::IListenerInputStruc01* inp_data = (Sonicteam::Player::Input::IListenerInputStruc01*)a1->GetCurrentDoc()->GetPlayerInput(a1->GetCurrentDoc()->GetRealControllerID(i));
			std::stringstream ss;

			//plr->CurrentController = a1->GetCurrentDoc()->GetPlayerInput(a1->GetCurrentDoc()->GetRealControllerID(i));

			ProcessButton(plr,inp_data,SO_GAMEPAD_RAW_BUTTON_A,0x10,0x1000,0x2000);	
			ProcessButton(plr,inp_data,SO_GAMEPAD_RAW_BUTTON_B,0x20,0x4000,0x8000);	

		//	ProcessButton(plr,inp_data,SO_GAMEPAD_RAW_BUTTON_DPAD_LEFT,0x4,0x10000,0x20000);	
		//	ProcessButton(plr,inp_data,SO_GAMEPAD_RAW_BUTTON_DPAD_RIGHT,0x8,0x40000,0x80000);	

		//	ProcessButton(plr,inp_data,SO_GAMEPAD_RAW_BUTTON_DPAD_LEFT,0x4,0x100000,0x200000);	
		//	ProcessButton(plr,inp_data,SO_GAMEPAD_RAW_BUTTON_DPAD_RIGHT,0x8,0x400000,0x800000);	

			if (inp_data->wLastButtons != 0){

				std::stringstream sss;
				//sss << std::hex << inp_data->wLastButtons;
				//DebugLogV2::PrintNextFixed(sss.str());
			}
	



			if (i == 0){

				std::stringstream ss;
				ss << std::hex<< plr->input_data;
				//	DebugLogV2::PrintNextFixed(ss.str());
			}

			if ( (inp_data->wLastButtons & (SO_GAMEPAD_RAW_BUTTON_DPAD_LEFT | SO_GAMEPAD_RAW_BUTTON_DPAD_RIGHT)) != 0){
				inp_data->fX1 = (inp_data->wLastButtons & SO_GAMEPAD_RAW_BUTTON_DPAD_LEFT) != 0 ? -1.0 : 1.0;
			}
			if ( (inp_data->wLastButtons & (SO_GAMEPAD_RAW_BUTTON_DPAD_DOWN | SO_GAMEPAD_RAW_BUTTON_DPAD_UP)) != 0){
				inp_data->fY1 = (inp_data->wLastButtons & SO_GAMEPAD_RAW_BUTTON_DPAD_UP) != 0 ? -1.0 : 1.0;
			}


			//StickOnly
			if (abs(inp_data->fX1) > 0.1){
				plr->hold_time += delta;
				if (plr->hold_time <= delta){
					if (inp_data->fX1 < 0.0){
						plr->input_data |= 4;
					}
					else{
						plr->input_data |=8;
					}

				}
				else if (plr->hold_time > 0.167){
					plr->hold_time = 0.0f;
				}
				else{
					plr->input_data &= ~(4 | 8);
				}
			}

			else {
				plr->hold_time = 0.0f;
				plr->input_data &= ~(4 | 8);
			}




			//StickOnly
			if (abs(inp_data->fY1) > 0.1){
				plr->hold_time_Y += delta;
				if (plr->hold_time_Y <= delta){
					if (inp_data->fY1 < 0.0){
						plr->input_data |= 1;
					}
					else{
						plr->input_data |= 2;
					}

				}
				else if (plr->hold_time_Y > 0.167){
					plr->hold_time_Y = 0.0f;
				}
				else{
					plr->input_data &= ~(1 | 2);
				}
			}

			else {
				plr->hold_time_Y = 0.0f;
				plr->input_data &= ~(1 | 2);
			}





		}
	}
}
void ResetFourInputs(Sonicteam::MainMenuTask *a1,float delta){
	for (int i = 0;i<4;i++){
		PLAYERS_DATA* plr =  &P_DATA[i];
		plr->input_data = 0;

	}
}
void ResetFourInputsComplete(Sonicteam::MainMenuTask *a1,float delta){
	for (int i = 0;i<4;i++){
		PLAYERS_DATA* plr =  &P_DATA[i];
		plr->input_data = 0;
		plr->selected = 0;
		ordered_input[i+1] = 0;

	}
}

int FixSelectedIndex(PLAYERS_DATA* plr) {
	int index = plr->SelectionIndex;
	std::set<int> used_indices;



	// Collect all used indices
	for (std::map<int,PLAYERS_DATA*>::iterator it = ordered_input.begin(); it != ordered_input.end(); ++it) {
		if ( it->second != 0&& (it->second->selected & (1 | 2)) != 0 && it->second != plr) {
			used_indices.insert(it->second->SelectionIndex);
		}
	}

	// Find the first available index
	int new_index = index;
	while (used_indices.find(new_index) != used_indices.end()) {
		if ((plr->input_data & 8) != 0) {
			new_index = (new_index + 1) % LimitCHR;
			std::stringstream ss;
			ss << std::hex<< new_index;
	
			
			
		} else if ((plr->input_data & 4) != 0) {
			new_index = (new_index - 1 + LimitCHR + LimitCHR) % LimitCHR;
		}
		else
		{
			new_index = (new_index + 1) % LimitCHR;
		}
	}

	// Update the index for the current player
	plr->SelectionIndex = new_index;

	return plr->SelectionIndex;
}

XMVECTOR RotationByAXIS(XMVECTOR VectorRot,XMVECTOR angledir,float angle){

	// Define the rotation axis
	XMVECTOR rotationAxis = XMVectorSet(0.0f, 0.0f, 1.0f,0); // For example, the Z-axis

	rotationAxis = angledir;
	// Calculate the rotation delta
	XMVECTOR rotationDelta = XMQuaternionRotationAxis(rotationAxis, XMConvertToRadians(angle));

	// Calculate the new rotation

	return XMQuaternionMultiply(VectorRot, rotationDelta);


}

int SelectedIndex = 0;
int __fastcall MainMenuTask_UI(Sonicteam::MainMenuTask *a1,float delta){


	Sonicteam::CsdObject* TAG_CSD_OBJECT = *(Sonicteam::CsdObject**)(a1->CSD_tag_character + 0x70);


	
	/*
	float pitch = 0.0f;
	float yaw = 10.0;
	float roll = 0.0f;

	ATG::GAMEPAD* gc =  ATG::Input::GetMergedInput();
	if ((gc->wPressedButtons & (XINPUT_GAMEPAD_LEFT_THUMB | XINPUT_GAMEPAD_RIGHT_THUMB)) != 0){
		int dir = (gc->wPressedButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0 ? -1 : 1;
		SelectedIndex = ((SelectedIndex + dir) % 4 + 4) % 4;
		GetSelectedCharacterLUA(a1, (int)&P_DATA[SelectedIndex].Rotation, a1->MMP1SelectedIndex);


	}

	if (gc->fX2 != 0.0){

		P_DATA[SelectedIndex].Rotation = RotationByAXIS(P_DATA[SelectedIndex].Rotation,XMVectorSet(0,0,1,0),gc->fX2*10.0*delta);
		GetSelectedCharacterLUA(a1, (int)&P_DATA[SelectedIndex].Rotation, a1->MMP1SelectedIndex);

	}
	if (gc->fY2 != 0.0){

		P_DATA[SelectedIndex].Rotation = RotationByAXIS(P_DATA[SelectedIndex].Rotation,XMVectorSet(0,1,0,0),gc->fY2*10.0*delta);
		GetSelectedCharacterLUA(a1, (int)&P_DATA[SelectedIndex].Rotation, a1->MMP1SelectedIndex);

	}
	if ((gc->wLastButtons & (XINPUT_GAMEPAD_LEFT_SHOULDER | XINPUT_GAMEPAD_RIGHT_SHOULDER)) != 0){
		float dir = (gc->wLastButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0 ? -1.0 : 1.0;
		P_DATA[SelectedIndex].Rotation = RotationByAXIS(P_DATA[SelectedIndex].Rotation,XMVectorSet(1,0,0,0),dir*10.0*delta);
		GetSelectedCharacterLUA(a1, (int)&P_DATA[SelectedIndex].Rotation, a1->MMP1SelectedIndex);
	}


		
	*/

	ProcessFourInputs(a1,delta);


	Sonicteam::SoX::RefCountObject* obj;
	int v78;

	if (a1->MMCurrentCase == 0x33){
		a1->MMCurrentCase = 0x200;
	}

	if (a1->MMCurrentCase == 0x16){
		a1->MMCurrentCase = 0x100;
	}
	if (a1->MMCurrentCase == 0x21){
		a1->MMCurrentCase = 0x121;
	}





	switch (a1->MMCurrentCase)
	{
	case 0x200:


		if (!BranchTo(0X824FE3B8,int,a1,a1->MMP1Input,8,&a1->MMTuint0xE0)){
			a1->MMP1Input = 0;
			return 0;
		}


		a1->MMHudOption->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B055,5,0,0,a1->MMTuint0xE0));
		a1->MMP1Input = 0;



		break;
		//new multiplayer
	case 0x99:

		a1->MMHudMainMenu->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B053,a1->MMTuint0xA0,63));
		a1->MMHudMainMenu->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B053,5,a1->MMTuint0xA0 | 0x70000));
		a1->MMCurrentCase = 0x100;
		break;

	//NEW MULTIPLAYER
	case 0x100:
		if ( !a1->MMTuint0x270 )
			return 0;
		{
			a1->MMHudMainMenu->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B053,0x64,a1->MMTuint0xA8 | 0x20000)); //mst (
		}
		a1->MMCurrentCase = 0x101;
		a1->MMP1Input = 0;
		
		ResetFourInputs(a1,delta);


		BranchTo(0x824FDBC0,int,a1->MMTuint0x9C,(int*)&a1->MMTextCard_msg_tag_c);
		a1->MMTuint0x270 = 0;
		return 0;
	//New Multiplayer(LOOP)
	case  0x101:

		//X
		if ((a1->MMP1Input & 0x10)  != 0){

			int s[] = {a1->MMHudMainMenu->HMMCSDSpriteMainMenu};
			
			if (a1->MMTuint0xA8 == 0){
				BranchTo(0x824CE9D0,int,(int*)s,"main_menu","tagstory_select"); //PLAY
			}
			else{

				a1->MMHudMainMenu->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B053,0x65,a1->MMTuint0xA8 | 0x20000));
				a1->MMHudMainMenu->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B053,0xD,a1->MMTuint0xA8 | 0x20000));



				PLAYERS_DATA* plr = &P_DATA[0];
				FixSelectedIndex(plr);
				v78 = a1->CSD_tag_character;
				int CSD_OBJECT = *(_DWORD *)(v78 + 0x70);
				Sonicteam::SoX::RefCountObject* csd = (Sonicteam::SoX::RefCountObject*)CSD_OBJECT;
				csd->GetObject<int>();
				csd->GetObject<int>();
				csd->GetObject<int>();
				csd->GetObject<int>();
				std::stringstream p_tug; p_tug << 1 << "p_tug";
				std::stringstream p_crusor; p_crusor << 1 << "p_cursor";
				std::stringstream p_name; p_name << 1 << "p_name";
				std::stringstream p_name_anime; p_name_anime << CHRS[plr->SelectionIndex];
				std::stringstream p_controller; p_controller << "controller_" << 1 << "p";
				std::stringstream p_controller_anim; p_controller_anim << "controller" << a1->GetCurrentDoc()->GetRealControllerID(0) + 1;

				CellLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_tug.str().c_str(),"DefaultAnim");
				CellLoadSpriteLoop(&CSD_OBJECT,(char*)p_crusor.str().c_str(),"loop");
				CellLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_name.str().c_str(),(char*)p_name_anime.str().c_str());
				CellLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_controller.str().c_str(),(char*)p_controller_anim.str().c_str());
				plr->selected |= 1;

				BranchTo(0x824FD1F0,int,a1,&P_DATA[0].Rotation,plr->SelectionIndex);

			
				P_DATA[0].CurrentController =  a1->GetCurrentDoc()->GetRealControllerID(0);
				P_DATA[0].selected = 1;

				TakeSlot(plr);
			
		
				P_DATA[0].LastSelectionIndex = P_DATA[0].SelectionIndex;
				GetSelectedCharacterLUA(a1, (int)&P_DATA[0].Rotation, P_DATA[0].SelectionIndex);
				BranchTo(0x824FDBC0,int,	
					a1->MMTuint0x9C,
					(int)&a1->MMTextCard_msg_characterselect_c);
				a1->MMP1Input = 0;
				a1->MMTuint0x270 = 0;
				a1->MMP1Input = 0;


			}
	



			if (a1->MMTuint0xA8 == 0) a1->MMHudMainMenu->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B053,0x66,a1->MMTuint0xAC | 0x20000)); //show select3 arrow
			a1->MMHudMainMenu->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B053,0x65,a1->MMTuint0xA8 | 0x20000)); //hide select2 arrow
			BranchTo(0x824FD460,int,a1,"main_deside");
			a1->MMCurrentCase = a1->MMTuint0xA8 == 0 ? 0x102 : 0x103;

			a1->MMP1Input = 0;
			return 0;

		}


		if (!BranchTo(0X824FE3B8,int,a1,a1->MMP1Input,2,&a1->MMTuint0xA8)){
			a1->MMP1Input = 0;
			return 0;
		}
		a1->MMP1Input = 0;
		{
			a1->MMHudMainMenu->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B053,0xB,a1->MMTuint0xA8 |  0x20000)); //mst 
			BranchTo(0x824FDBC0,int,a1->MMTuint0x9C,&a1->MMTextCard_msg_tag_c + a1->MMTuint0xA8);
		}

		return 0;
	case 0x102:
		//Back
		if ((a1->MMP1Input & 0x20) != 0){
			int s[] = {a1->MMHudMainMenu->HMMCSDSpriteMainMenu};
			BranchTo(0x824CE670,int,(int*)s,"main_menu","tagstory_select"); //PLAY

			a1->MMHudMainMenu->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B053,0x4E,a1->MMTuint0xA8 | 0x20000)); //mst (
			a1->MMHudMainMenu->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B053,0xB,a1->MMTuint0xA8 |  0x20000)); //mst (
			a1->MMHudMainMenu->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B053,0x67,a1->MMTuint0xAC | 0x20000)); //Select3 Arrow Show(Position)

			BranchTo(0x824FD460,int,a1,"main_deside");
			a1->MMCurrentCase = 0x101;
			return 0;
		}
		//select
		if ((a1->MMP1Input & 0x10) != 0){

			int s[] = {a1->MMHudMainMenu->HMMCSDSpriteMainMenu};
			BranchTo(0x824CE670,int,(int*)s,"main_menu","tagstory_select"); //PLAY

			a1->MMHudMainMenu->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B053,0x67,a1->MMTuint0xAC | 0x20000)); //Select3 Arrow Show(Position)
			a1->MMCurrentCase = 0x103; // TAG STORY, TAG TRIAL
			BranchTo(0x824FD460,int,a1,"main_deside");
			return 0;

		}


		if (!BranchTo(0X824FE3B8,int,a1,a1->MMP1Input,2,&a1->MMTuint0xAC)){
			a1->MMP1Input = 0;
			return 0;
		}
		a1->MMP1Input = 0;
		a1->MMHudMainMenu->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B053,0x66,a1->MMTuint0xAC | 0x20000)); //Select3 Arrow Show(Position)

		return 0;
		//new Battle Screen
	case 0x103:
		{


		bool is_can_leave = true;
		for (int i = 0;i<4;i++){
			PLAYERS_DATA* plr =  &P_DATA[i];
			if (plr->selected != 0 ){
				is_can_leave = false;
			}
		}
		if (is_can_leave) a1->MMCurrentCase = 0x99;

		int CSD_OBJECT = *(_DWORD *)(a1->CSD_tag_character + 0x70);
		Sonicteam::SoX::RefCountObject* csd = (Sonicteam::SoX::RefCountObject*)CSD_OBJECT;

		

		if (GetReadyPlayers()  == GetActivePlayers() && GetActivePlayers() > 0){
			PLAYERS_DATA* plr =  GetPrimaryPlayer();
			if ((plr->input_data & 0x10) != 0){
				
				a1->MMCurrentCase = 0x104;
				break;;
			}
		}

	

		//P_DATA[0].input_data = a1->MMP1Input;



		for (int i = 0;i<4;i++){
			PLAYERS_DATA* plr =  &P_DATA[i];
			//HELP A BIT
			{


			if ((plr->input_data & 0x10) !=0){

				//CONFIRM
				if ((plr->selected & 1) != 0){
					int ID = GetSlot(plr);
					int CSD_OBJECT = *(_DWORD *)(a1->CSD_tag_character + 0x70);
					Sonicteam::SoX::RefCountObject* csd = (Sonicteam::SoX::RefCountObject*)CSD_OBJECT;
					csd->GetObject<int>();
					csd->GetObject<int>();
					std::stringstream p_crusor; p_crusor << ID << "p_cursor";
					std::stringstream p_name; p_name << ID << "p_name";
					std::stringstream p_name_anime; p_name_anime << CHRS[plr->SelectionIndex] << "_select";
				
				
					CellLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_name.str().c_str(),(char*)p_name_anime.str().c_str());
					CellUnLoadSpriteLoop(&CSD_OBJECT,(char*)p_crusor.str().c_str(),"loop");
					plr->selected &= ~1;
					plr->selected |= 2;


					int ObjPlayer = *(int*)(((int)(a1)) + 4 * (plr->SelectionIndex + 0xA7));
					if (ObjPlayer){
						Sonicteam::Player::State::IMachine* Mashine = *(Sonicteam::Player::State::IMachine**)(ObjPlayer + 0xE4);
						Mashine->ChangeMashineState(1);

					}
					plr->CurrentController = a1->GetCurrentDoc()->GetRealControllerID(i);
			
			




					BranchTo(0x824FD460,int,a1,"main_deside");
				}
				else if (plr->selected == 0){
					plr->CurrentController = a1->GetCurrentDoc()->GetRealControllerID(i);
					int ID = TakeSlot(plr);
					if (ID != -1){
						FixSelectedIndex(plr);
						v78 = a1->CSD_tag_character;
						int CSD_OBJECT = *(_DWORD *)(v78 + 0x70);
						Sonicteam::SoX::RefCountObject* csd = (Sonicteam::SoX::RefCountObject*)CSD_OBJECT;
						csd->GetObject<int>(); //ref add
						csd->GetObject<int>(); //ref add
						csd->GetObject<int>(); //ref add
						csd->GetObject<int>(); //ref add
						std::stringstream p_tug; p_tug << ID << "p_tug";
						std::stringstream p_crusor; p_crusor << ID << "p_cursor";
						std::stringstream p_name; p_name << ID << "p_name";
						std::stringstream p_name_anime; p_name_anime << CHRS[plr->SelectionIndex];
						std::stringstream p_controller; p_controller << "controller_" << ID << "p";
						std::stringstream p_controller_anim; p_controller_anim << "controller" << a1->GetCurrentDoc()->GetRealControllerID(i) + 1;

						CellLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_tug.str().c_str(),"DefaultAnim");
						CellLoadSpriteLoop(&CSD_OBJECT,(char*)p_crusor.str().c_str(),"loop");
						CellLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_name.str().c_str(),(char*)p_name_anime.str().c_str());
						CellLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_controller.str().c_str(),(char*)p_controller_anim.str().c_str());
						plr->selected |= 1;

						plr->LastSelectionIndex = plr->SelectionIndex;
			
						BranchTo(0x824FD1F0,int,a1,&plr->Rotation,plr->SelectionIndex);

						
					}
					else{
						plr->CurrentController = -1;
					}
				
				}

			}
			else if ((plr->input_data & 0x20) != 0){
				//CONFIFM
				if ((plr->selected & 2) != 0){
					plr->selected &= ~2;
					plr->selected |= 1;
					int ID = GetSlot(plr);
					int CSD_OBJECT = *(_DWORD *)(a1->CSD_tag_character + 0x70);
					Sonicteam::SoX::RefCountObject* csd = (Sonicteam::SoX::RefCountObject*)CSD_OBJECT;
					csd->GetObject<int>();
					csd->GetObject<int>();
					std::stringstream p_crusor; p_crusor << ID << "p_cursor";
					CellLoadSpriteLoop(&CSD_OBJECT,(char*)p_crusor.str().c_str(),"loop");

					int ObjPlayer = *(int*)(((int)(a1)) + 4 * (plr->SelectionIndex + 0xA7));
					if (ObjPlayer){
						Sonicteam::Player::State::IMachine* Mashine = *(Sonicteam::Player::State::IMachine**)(ObjPlayer + 0xE4);
						Mashine->ChangeMashineState(0);

					}


				}
				//leave(slot)
				else if (plr->selected & 1 != 0){
					int ID = FreeSlot(plr);
					int CSD_OBJECT = *(_DWORD *)(a1->CSD_tag_character + 0x70);
					Sonicteam::SoX::RefCountObject* csd = (Sonicteam::SoX::RefCountObject*)CSD_OBJECT;
					csd->GetObject<int>();
					csd->GetObject<int>();
					csd->GetObject<int>();
					csd->GetObject<int>();
					std::stringstream p_tug; p_tug << ID << "p_tug";
					std::stringstream p_crusor; p_crusor << ID << "p_cursor";
					std::stringstream p_name; p_name << ID << "p_name";
					std::stringstream p_controller; p_controller << "controller_" << ID << "p";
					std::stringstream p_controller_anim; p_controller_anim << "controller" << a1->GetCurrentDoc()->GetRealControllerID(i) + 1;

					CellUnLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_tug.str().c_str(),"DefaultAnim");
					CellUnLoadSpriteLoop(&CSD_OBJECT,(char*)p_crusor.str().c_str(),"loop");
					CellUnLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_name.str().c_str(),"sonic");
					CellUnLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_controller.str().c_str(),(char*)p_controller_anim.str().c_str());
				
					
			
					plr->LastSelectionIndex = plr->SelectionIndex;
					BranchTo(0x824FD1F0,int,a1,&P_DATA[4].Rotation,plr->LastSelectionIndex);

					plr->selected &= ~1;
		
				}
			}
	
		
			if ( (plr->selected & 1) != 0 && BranchTo(0x824FE310,int,a1,plr->input_data,LimitCHR,&plr->SelectionIndex) == 1){
				int ID = GetSlot(plr);
				//DebugLogV2::PrintNextFixed("asg");
				v78 = a1->CSD_tag_character;
				int CSD_OBJECT = *(_DWORD *)(v78 + 0x70);
				Sonicteam::CsdObject* csd = (Sonicteam::CsdObject*)CSD_OBJECT;
				csd->CsdLink0x4();
				csd->GetObject<int>();

				std::stringstream p_name; p_name << ID << "p_name";
				FixSelectedIndex(plr);

				BranchTo(0x824FD1F0,int,a1,&P_DATA[4].Rotation,plr->LastSelectionIndex);
				BranchTo(0x824FD1F0,int,a1,&P_DATA[ID-1].Rotation,plr->SelectionIndex);

	
				int ObjPlayer = *(int*)(((int)(a1)) + 4 * (plr->SelectionIndex + 0xA7));
				if (ObjPlayer){
					Sonicteam::Player::State::IMachine* Mashine = *(Sonicteam::Player::State::IMachine**)(ObjPlayer + 0xE4);
					Mashine->ChangeMashineState(0);

				}


				CellLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_name.str().c_str(),(char*)CHRS[plr->SelectionIndex]);



				plr->LastSelectionIndex = plr->SelectionIndex;

			}
			
			}
			}
		}
		//CHECK
		break;


		case 0x104:
			{
				
				TagBattleMain::EmptyXNCPCSD* csd = (TagBattleMain::EmptyXNCPCSD*) a1->CSD_tag_character;
				csd->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B010,0xA,a1->MMTuint0xDC));
			//	TAG_CSD_OBJECT->MarathonPlaySceneAnimation("2p_tug","DefaultAnim");
			//	TAG_CSD_OBJECT->FCProject->
				
				
			}

			a1->MMCurrentCase = 0x105;
			break;
			//Select ACTS
		case 0x105:
			{
				TagBattleMain::EmptyXNCPCSD* csd = (TagBattleMain::EmptyXNCPCSD*) a1->CSD_tag_character;
				PLAYERS_DATA* plr =  	GetPrimaryPlayer();
				if ((plr->input_data & 0x20) != 0){
					a1->MMCurrentCase = 0x103; //BACk
					csd->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B010,0xB,a1->MMTuint0xDC));

				} 
				if ((plr->input_data & 0x10) != 0){
					a1->MMCurrentCase = 0x106;


				}
				if (BranchTo(0x824FE3B8,int,a1,plr->input_data,9,&a1->MMTuint0xDC) == 1){
					csd->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B010,0xC,a1->MMTuint0xDC));

				}


			}
			break;
			//loading (prep) (battle)
		case 0x106:
			{
				//Fade (not working because mb 0x270 it what it was)
				//+ return to exactly battle menu

				char FadeTaskParams[0x28];
				BranchTo(0x82170B68,int,FadeTaskParams); //Create
				*(int*)&FadeTaskParams[7] = 1;
				BranchTo(0x824CAC88,int,malloc06(0x64),a1,FadeTaskParams);
				a1->MMTuint0x50 = 5; // easier replace battle behaviour
				a1->MMTuint0x270 = 0;
				BranchTo(0x8216ECD8,int,FadeTaskParams); //Destory

				//


				int CSD_OBJECT = *(_DWORD *)(a1->CSD_tag_character + 0x70);
				TagBattleMain::EmptyXNCPCSD* csd_E = (TagBattleMain::EmptyXNCPCSD*) a1->CSD_tag_character;
				Sonicteam::SoX::RefCountObject* csd = (Sonicteam::SoX::RefCountObject*)CSD_OBJECT;
				csd_E->OnMessageRecieved(&Sonicteam::SoX::Message(0x1B010,0xB,a1->MMTuint0xDC));
				CellUnLoadSpriteWithAnim(&CSD_OBJECT,(char*)"stage_window","in");

				for (int i = 0;i<4;i++){
					PLAYERS_DATA* plr =  &P_DATA[i];

					int ID = GetSlot(plr);
				
		

					csd->GetObject<int>();
					csd->GetObject<int>();
					csd->GetObject<int>();
					csd->GetObject<int>();
					std::stringstream p_tug; p_tug << ID << "p_tug";
					std::stringstream p_crusor; p_crusor << ID << "p_cursor";
					std::stringstream p_name; p_name << ID << "p_name";
					std::stringstream p_controller; p_controller << "controller_" << ID << "p";
					std::stringstream p_controller_anim; p_controller_anim << "controller" << a1->GetCurrentDoc()->GetRealControllerID(i) + 1;

					CellUnLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_tug.str().c_str(),"DefaultAnim");
					CellUnLoadSpriteLoop(&CSD_OBJECT,(char*)p_crusor.str().c_str(),"loop");
					CellUnLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_name.str().c_str(),"sonic");
					CellUnLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_controller.str().c_str(),(char*)p_controller_anim.str().c_str());

					BranchTo(0x824FD1F0,int,a1,&P_DATA[4].Rotation,plr->LastSelectionIndex);

					
		
				}



			BranchTo(0x821EDDB0,int,a1->GetCurrentDoc()->DocGetGameRuleContext(),GetReadyPlayers()); // Set Player
			BranchTo(0x826314F8,int,a1->GetCurrentDoc()->DocGetGameRuleContext(),2); // battle flag i guess 
			int i = 0;


			origin_index_order.clear();
			for (int i = 0;i<4;i++){
				origin_index_order.push_back(a1->GetCurrentDoc()->GetRealControllerID(i));
			}


			//map characters with ID
			for (std::map<int, PLAYERS_DATA*>::iterator it = ordered_input.begin(); it != ordered_input.end(); it++) {
				if (it->second != 0) {
			

					int* ControllersIDS = (int*)((int)a1->GetCurrentDoc() + 0x55C2C);
					int GID = ControllersIDS[i];
					if (GID != it->second->CurrentController) {
						int* FGID = std::find(&ControllersIDS[0], &ControllersIDS[4], it->second->CurrentController);
						int index = std::distance(&ControllersIDS[0], FGID);
						std::swap(ControllersIDS[index], ControllersIDS[i]);
						
					}
					BranchTo(0x82186138, int, a1->GetCurrentDoc()->DocGetGameRuleContext(), i, &std::string(CHRS_S[it->second->SelectionIndex]));
					i++;
				}
			}

			//ResetFourInputsComplete(a1,delta);
			
			a1->GetCurrentDoc()->DocLoadScene(battle_missions[a1->MMTuint0xDC]);
			a1->GetCurrentDoc()->DocSet0x55C54(0x21);
			a1->GetCurrentDoc()->DocCurrentApplication->ApplicationConsoleMessages(0xC,0,6);
			a1->GetCurrentDoc()->DocCurrentApplication->ApplicationConsoleMessages(0xB,0,0);
			a1->MMCurrentCase = 0x3F;


			}


			break;
			//From Stage To Battle Transistion
		case 0x121:

			{

		
			for (std::map<int,PLAYERS_DATA*>::iterator it = ordered_input.begin();it!= ordered_input.end();it++){
				if (it->second  == 0) continue;;


			
			
				//restore-origin-input-data
				for (int i = 0;i<4;i++){
					a1->GetCurrentDoc()->SetRealControllerID(i,origin_index_order[i]);
				}


				int ID = it->first;
				PLAYERS_DATA* plr = it->second;
				int CSD_OBJECT = *(_DWORD *)(a1->CSD_tag_character + 0x70);
				Sonicteam::SoX::RefCountObject* csd = (Sonicteam::SoX::RefCountObject*)CSD_OBJECT;
				csd->GetObject<int>();
				csd->GetObject<int>();
				csd->GetObject<int>();
				csd->GetObject<int>();
				std::stringstream p_tug; p_tug << ID << "p_tug";
				std::stringstream p_crusor; p_crusor << ID << "p_cursor";
				std::stringstream p_name; p_name << ID << "p_name";
				std::stringstream p_name_anime; p_name_anime << CHRS[plr->SelectionIndex];
				std::stringstream p_controller; p_controller << "controller_" << ID << "p";
				std::stringstream p_controller_anim; p_controller_anim << "controller" << plr->CurrentController + 1;

				CellLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_tug.str().c_str(),"DefaultAnim");

				if ((plr->selected & 2) == 0) CellLoadSpriteLoop(&CSD_OBJECT,(char*)p_crusor.str().c_str(),"loop");

				CellLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_name.str().c_str(),(char*)p_name_anime.str().c_str());
				CellLoadSpriteWithAnim(&CSD_OBJECT,(char*)p_controller.str().c_str(),(char*)p_controller_anim.str().c_str());
				BranchTo(0x824FD1F0,int,a1,&P_DATA[ID-1].Rotation,plr->SelectionIndex);
		
			}
		
		
			}
		

			a1->MMCurrentCase = 0x103;

			break;


	}



	return BranchTo(0x824FFCF8,int,a1);

}


extern "C" int RenderNeParticlePost_GetNumCHR(lua_State* L){


	Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
	int ppls = *(int*)(impl->DocGetGameRuleContext() + 0xC);

	lua_pushnumber(L,ppls);
	return 1;
}




void TagBattleMain::GlobalInstall()
{

	
//	ExFileSystem::PushArc("DLL\\cache_3P.arc");
//	ExFileSystem::PushArc_pkg("DLL\\scripts_4P.arc");
//	ExFileSystem::PushArc("DLL\\sprites_4P.arc");
//	ExFileSystem::PushArc("DLL\\sprite_online.arc");

	


	
	InitINPUT();
	WRITE_DWORD(0x8204C5BC,RenderNeParticlePost_GetNumCHR);
	WRITE_DWORD(0x82039C10,MainMenuTask_UI);


	Sonicteam::GameImp* im = 0;
	


//	Sonicteam::GameImp* impl = 0;
	
//	impl->GamePropLibary->ObjCommonProp->str1 = "";
//	impl->GamePropLibary->KingdomvalleyProp->str1 = "";

	GlobalInstall_InBattle();
	GlobalInstall_ONLINE();

}
