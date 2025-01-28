#include "TagBattleMain_InBattle.h"
#include <xboxmath.h>
#include <map>
#include <sstream>
#include <string>
#include <set>
#include "AtgInput.h"


using namespace TagBattleMain;

int GetPPL(){
	return GoalActors.size();
}
int IncrementPlayer(int ActorID){

	if (std::find(GoalActors.begin(), GoalActors.end(), ActorID) == GoalActors.end()) {
		GoalActors.push_back(ActorID);
	}

	return GetPPL();
}
int PlayerActorIDIndex(int ActorIDS){

	Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
	UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);

	return 0;	
}
int PlayerIndexToActorID(int ActorIDS){

	Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
	UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);

	int ActorID =  *(int*)  ((gameimp  + 0xE40) + (ActorIDS * 0x4C));
	
	return ActorID;	
}

int GetPlayerIDPlace(int ID){



	int UID = PlayerIndexToActorID(ID);

	for (int i = 0;i<GoalActors.size();i++){
		if (GoalActors[i] == UID){
			return i+1;
		}
	}
	return -1;

}







int __fastcall GoalRing_TriggerTouch(int a1, double a2){

	DWORD v7;
	unsigned int* v9;
	DWORD* result;
	if ( !*(BYTE *)(a1 + 0x1CC) )
	{
		v7 = *(DWORD *)(a1 + 0x1C4);

		for ( unsigned int* i = *(unsigned int **)(a1 + 0x1C0); ; ++i )
		{
			v9 = *(unsigned int **)(a1 + 0x1C4);
			if ( v9 == i )
				break;

			result = (_DWORD *)BranchTo(0x82160658,int,a1, *i);  // Dont know what is I	T but 
			if (result){

			
				Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
				int ppls = *(int*)(impl->DocGetGameRuleContext() + 0xC);

				int ObjPlayer = (int)result;
				if ( ObjPlayer ){
					Sonicteam::Player::State::IMachine* Mashine = *(Sonicteam::Player::State::IMachine**)(ObjPlayer + 0xE4);
					Mashine->ChangeMashineState(0x16);
				}


				if (IncrementPlayer(*i) >= ppls){
				
	
					goto NORMAL;
				}


			}

		}

		return 0;
	}

NORMAL:
	return BranchTo(0x822A4690,int,a1,a2);

}



int TagBattleMain::GoalCount;

std::vector<int> TagBattleMain::GoalActors;


void __fastcall BattleResultTaskOnMessage(int result, double delta){

	

	Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
	int PlayersGamemodeCount = *(int*)(impl->DocGetGameRuleContext() + 0xC);


	int HudBattleResult = *(int *)(result + 0x50);
	int CSD_BATTLE_RESULT = *(_DWORD *)(HudBattleResult + 0x54);
	Sonicteam::SoX::RefCountObject* CSD_REF = (Sonicteam::SoX::RefCountObject*)CSD_BATTLE_RESULT;

	float* _timer_ = (float* )(result + 0x54);
	if (*_timer_ > 0.0){
	//	*_timer_ = *_timer_ - delta;
	}


	switch ( *(_DWORD *)(result + 0x4C) ){

		case  0:
			{
				std::stringstream rank_ppl; rank_ppl << "rank_" << PlayersGamemodeCount << "p";
				std::stringstream icon_ppl; icon_ppl << "icon_" << PlayersGamemodeCount << "p";

				CSD_REF->GetObject<int>(); CellLoadSpriteWithAnim(&CSD_BATTLE_RESULT,"plate","title_plate");
				CSD_REF->GetObject<int>(); CellLoadSpriteWithAnim(&CSD_BATTLE_RESULT,"congratulations","DefaultAnim");
				CSD_REF->GetObject<int>(); CellLoadSpriteWithAnim(&CSD_BATTLE_RESULT,"p_icon",(char*)icon_ppl.str().c_str());
				CSD_REF->GetObject<int>(); CellLoadSpriteWithAnim(&CSD_BATTLE_RESULT,"rank",(char*)rank_ppl.str().c_str());

				for (int i = 0;i<PlayersGamemodeCount;i++){
					int Place = GetPlayerIDPlace(i);
					if (Place == -1) continue;
					std::stringstream ss; ss << i+1 << "p";
					CSD_REF->GetObject<int>(); CellLoadSpriteWithSubAndSpriteIndex(&CSD_BATTLE_RESULT,"rank",(char*)ss.str().c_str(),Place-1);

				}
					GoalActors.clear();
			//	*_timer_ = 1.0;



			}

			break;
		default:
			
			break;


	}

	BranchTo(0x824C9660,int,result,delta);

}


void __fastcall EngineGlobalEventsActions(int* a1, double a2){

	if (a1[2] == 4 && a1[1] >=2 && a1[1] <= 3 ){
		GoalActors.clear();
	}

	BranchTo(0x82185D30,int,a1,a2);
}


void TagBattleMain::GlobalInstall_InBattle()
{
	WRITE_DWORD(0x826651E4,0x60000000);
	WRITE_DWORD(0x826651E8,0x60000000);
	WRITE_DWORD(0x82648D70,0x60000000);

	WRITE_DWORD(0x82035A2C,BattleResultTaskOnMessage);
	WRITE_DWORD(0x820126A8,GoalRing_TriggerTouch);
	WRITE_DWORD(0x82001B3C,EngineGlobalEventsActions);
}
