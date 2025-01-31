#include "TagBattleOnline.h"
#include <xboxmath.h>
#include <Patch/LuaExtension/DebugLog.h>

#include <map>
#include <sstream>
#include <string>
#include <set>

#include <CsdObject.h>
#include <Sox/StepableThread.h>


#define CUTSCENE_MODE false

using namespace TagBattleMain;






struct SMDATA_PPL_CHANGE_SCENE {
	DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL(1, IPPROTO_TCP);
	XUID sender_xuid;
	char scene_name[128];
};
struct SMDATA_PPL_CHANGE_SCENE_REPLICATE {
	DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL(2, IPPROTO_TCP);
	XUID sender_xuid;
	char scene_name[128];
};

struct SMDATA_PPL_CHANGE_TRANSFORM {
	DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL(3, IPPROTO_UDP);
	XMMATRIX Transform;
	XMVECTOR Position;
	XMVECTOR Rotation;
	XUID sender_xuid;
};

struct SMDATA_PPL_CHANGE_TRANSFORM_REPLICATE {
	DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL(4, IPPROTO_UDP);
	XMMATRIX Transform;
	XMVECTOR Position;
	XMVECTOR Rotation;
	XUID sender_xuid;
};


struct SMDATA_PPL_CHANGE_CBEHAVIOUR {
	DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL(5, IPPROTO_UDP);
	XMFLOAT4 StickFixedRotationMb;
	int AnimationID;
	int AnimationState;

	//Context
	int ContextFlags;
	int ExportWeaponRequestFlag;	
	int ExportPostureRequestFlag;	
	int UnknownFlags01;	
	unsigned long long UnknownFlags0xC8;

	//Sonic-context
	unsigned int gem_color;
	XUID sender_xuid;
};

struct SMDATA_PPL_CHANGE_CBEHAVIOUR_REPLICATE {
	DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL(6, IPPROTO_UDP);
	XMFLOAT4 StickFixedRotationMb;
	int AnimationID;
	int AnimationState;

	//Context
	int ContextFlags;
	int ExportWeaponRequestFlag;	
	int ExportPostureRequestFlag;	
	int UnknownFlags01;	
	unsigned long long UnknownFlags0xC8;
	
	//Sonic Context
	unsigned int gem_color;
	XUID sender_xuid;

};

struct SMDATA_PPL_CHANGE_RINGS {
	DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL(7, IPPROTO_UDP);
	int RingsCount;

	XUID sender_xuid;
};

struct SMDATA_PPL_CHANGE_RINGS_REPLICATE {
	DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL(8, IPPROTO_UDP);
	int RingsCount;

	XUID sender_xuid;
};

struct SMDATA_PPL_CHANGE_CHR {
	DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL(9, IPPROTO_UDP);
	char sender_character[64];
	XUID sender_xuid;
};

struct SMDATA_PPL_CHANGE_CHR_REPLICATE {
	DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL(10, IPPROTO_UDP);
	char sender_characer[64];
	XUID sender_xuid;
};













struct OBJPlayerSpawnData{
	const char *player_name;
	int player_index;
	int player_controller_id;
	int player_flag0;
	int Unk0x10;
	int Unk0x14;
	int Unk0x18;
	int Unk0x1C;
	XMVECTOR player_position;
	XMVECTOR player_rotation;
	int player_unk_flag1;
	void *player_based_on;
	char player_unk_flag2;

public: OBJPlayerSpawnData();


};
OBJPlayerSpawnData::OBJPlayerSpawnData()
{
	this->player_index = -1;
	this->player_name = 0;
	this->player_controller_id = 0;
	this->Unk0x10 = 1;
	this->player_flag0 = -1;
	this->player_unk_flag1 = 0;
	this->player_based_on = 0;
	this->player_unk_flag2 = 1;

	this->player_position = XMVectorZero();
	this->player_rotation = XMVectorZero();

}





Socket _socket;
int LocalPlayer;
Sonicteam::CsdObject* Player_TAG;
Sonicteam::SoX::RefCountObject* Player_TAG_DRAWABLE;
Sonicteam::SoX::IResource* Player_TAG_SHADER;
bool GameIMP_LOADED_SCENE = false;
bool GameIMP_LOADED_CUTSCENE = false;

struct PPL_DATA{
	XMVECTOR Position_FRAME;
	XMVECTOR Rotation_Posture;;
	XMMATRIX RFTransformMatrix0x70_FRAME;
	XMFLOAT4 StickFixedRotationMb;

	std::string player_pkg;

	int Context_Animation_ID;
	int Context_Animation_STATE;
	unsigned int RingsCount;
	unsigned int LivesCount;
	unsigned int ContextFlags;
	unsigned int ExportPostureRequestFlag;
	unsigned int ExportWeaponRequestFlag;
	unsigned int UnknownFlags01;
	unsigned int UnknownFlags0xC8;
	unsigned int gem_color;
	bool local;

	Sonicteam::CsdObject* CSD;
	Sonicteam::SoX::RefCountObject* CSD_SHADER;
	Sonicteam::SoX::RefCountObject* TechniqueCSD3D;
	Sonicteam::SoX::RefCountObject* CsdObjectDrawable;

	std::string scene;
	int object_player;
	PPL_DATA(){
		memset(this,0,sizeof(PPL_DATA));
		scene = "";
		object_player = 0;
		player_pkg = "sonic_new.lua";
		
	}

	

};

std::map<XUID,PPL_DATA> Players_DATA;
std::map<std::string,const char*> players_chr_remap;

// Define your structures


static int GetLocalPlayer(){
	Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
	UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);

	int LocalPlayer = 0;
	if (gameimp == 0 || *(UINT32*)gameimp != 0x82001AEC) return 0;

	UINT32 vft =  *(UINT32*)impl->DocCurrentMode;
	if (vft == 0x82033534){ //GameMode
		UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);
		int ActorID =  *(int*)(gameimp + 0xE40);	
		UINT32 ActorManager = *(UINT32*)(gameimp+0x11C4);
		if (ActorManager) return BranchTo(0x821609D0,int,ActorManager,&ActorID);
//		UINT32 ActorMangerActorsCount = *(UINT32*)(ActorManager+0x80000);
		
		
	}
	return LocalPlayer;
}

static int IsLocalPlayer(int ObjPlayer){
	return GetLocalPlayer() == ObjPlayer;
}

static int IsNetworkPlayer(int ObjPlayer){
	for (std::map<XUID,PPL_DATA>::iterator it = Players_DATA.begin();it != Players_DATA.end();it++){
		if (it->second.object_player == 0 ) continue;
		if (it->second.object_player == ObjPlayer ){
			return  true;

		}
	}
	return false;
}


static const char* ObjectPlayerToLuaChr(int ObjectPlayer){

	Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
	UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);

	int LocalPlayer = 0;
	if (gameimp == 0 || *(UINT32*)gameimp != 0x82001AEC) return 0;
	int index = 0;

	UINT32 vft =  *(UINT32*)impl->DocCurrentMode;
	if (vft == 0x82033534){ //GameMode

		UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);
		UINT32 ActorManager = *(UINT32*)(gameimp+0x11C4);
		UINT32 ActorMangerActorsCount = *(UINT32*)(ActorManager+0x80000);
		UINT32 PIX = 0;
		for (int i = 0;i<ActorMangerActorsCount;i++)
		{
			UINT32 Actor = *(UINT32*)(ActorManager+0x40000+(i*4));
			UINT32 ActorVFT = *(UINT32*)Actor;
			if (ActorVFT == 0x82003564){ //Object_Player


				byte IsObjectPlayerHaveControll = *(byte*)(Actor + 0xC8);
				byte AIFLAG = *(byte*)(Actor + 0xCA);

				if (AIFLAG || IsNetworkPlayer(Actor) ){
					continue;
				}
				else if (Actor == ObjectPlayer){
					break;
				}
				index ++;
				
			}
		}
		
		if (index < 16){
			std::string* chr_lua =  (std::string*)((gameimp + 0x30) + (0xF0 * index));
			PushXenonMessage(L"MSG",chr_lua->c_str());
			

		}
	}
	return "";
}







static const char* GetLocalPKGPlayerName(){
	std::string* str =  (std::string*)(GetLocalPlayer() + 0x74);
	return str->c_str();
}



static int SpawnDummyCharacter(const char* name = "sonic_new.lua",XUID xuid = 0){
	
    // Spawn dummy character
    OBJPlayerSpawnData data;
	data.player_index = xuid;
	if (data.player_name == ""){
		    data.player_name = "sonic_new.lua";
	}
	else{
		    data.player_name = name;
	}

    data.player_unk_flag1 = 0xFFFFFFFF;
    data.player_unk_flag2 = 0xFFFFFFFF;
    data.player_controller_id = 0xFFFFFFFF;
	
    Sonicteam::DocMarathonImp* impl = *(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
    UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);
    UINT32 NameActorPlayers = *(UINT32*)(gameimp + 0x1428);
    int Player = (int)BranchTo(0x8219FAE0, int, NameActorPlayers, &data);

	*(short*)(Player + 0xC8) = 0x0001;;

    Sonicteam::Player::State::IMachine* machine = *(Sonicteam::Player::State::IMachine**)(Player + 0xE4);
    machine->ChangeMashineState(0x1E);
    Sonicteam::Player::State::ICommonContext* contxt = dynamic_cast<Sonicteam::Player::State::ICommonContext*>(machine->GetMashineContext().get());
    contxt->AnimationState = 0x10;
    contxt->AnimationState = -1;
    return Player;
}

void MSG_HANDLE_SERVER_XUI_JOIN(Socket* _sock, SOCKET sock, XUID xuid){


	PPL_DATA data = PPL_DATA();
	data.scene = "";
	Players_DATA[xuid] = data;
	std::stringstream stream; stream << "[Server] ADD XUID " << std::hex << xuid;
	DebugLogV2::PrintNextFixed(stream.str());
}


void MSG_HANDLE_CLIENT_XUI_JOIN(Socket* _sock, SOCKET sock, XUID xuid){
	
	PPL_DATA data = PPL_DATA();
	data.scene = "";
	Players_DATA[xuid] = data;
	std::stringstream stream; stream << "[Client] ADD XUID " << std::hex << xuid;
	DebugLogV2::PrintNextFixed(stream.str());
}

//TRY SPAWN ONLY WHEN LEVEL LOADED, AND THEN SAVE FOR LATER
void DestroyObjectPlayerByXUID(XUID xuid){

//	DebugLogV2::PrintNextFixed("DestroyObjectPlayerByXUID");

	if (CUTSCENE_MODE){
		if (GameIMP_LOADED_CUTSCENE == false && GameIMP_LOADED_SCENE ==false  )
			return;
	}
	else{
		if (GameIMP_LOADED_SCENE == false) return;
	}


	Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
	UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);
	if (gameimp != 0 && *(UINT32*)gameimp == 0x82001AEC && Players_DATA[xuid].object_player != 0){
		BranchTo(0x82195298,int,Players_DATA[xuid].object_player,1);
		Players_DATA[xuid].object_player = 0;
	}
}
void SpawnObjectPlayerByXUID(XUID xuid){

	DebugLogV2::PrintNextFixed("SpawnObjectPlayerByXUID");
	
	if (CUTSCENE_MODE){
		if (GameIMP_LOADED_CUTSCENE == false && GameIMP_LOADED_SCENE ==false  )
			return;
	}
	else{
		if (GameIMP_LOADED_SCENE == false) return;
	}


	DebugLogV2::PrintNextFixed("SpawnObjectPlayerByXUIDX");


	Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
	UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);
	if (gameimp != 0 && *(UINT32*)gameimp != 0x82001AEC ) return;

	if ( Players_DATA[xuid].object_player == 0){
		Players_DATA[xuid].object_player = SpawnDummyCharacter(Players_DATA[xuid].player_pkg.c_str(),xuid);
	}
	
}

static void SWAPObjectPlayer(int Player,const char* name = "sonic_new.lua"){
	BranchTo(0x82195D20,int,Player,name);
	*(short*)(Player + 0xC8) = 0x0001;;
	Sonicteam::Player::State::IMachine* machine = *(Sonicteam::Player::State::IMachine**)(Player + 0xE4);
	machine->ChangeMashineState(0x1E);
	Sonicteam::Player::State::ICommonContext* contxt = dynamic_cast<Sonicteam::Player::State::ICommonContext*>(machine->GetMashineContext().get());
	contxt->AnimationState = 0x10;
	contxt->AnimationState = -1;

}

void SWAPObjectPlayerByXUID(XUID xuid,const char* name = "sonic_new.lua"){

	//	DebugLogV2::PrintNextFixed("SpawnObjectPlayerByXUID");
	if (CUTSCENE_MODE){
		if (GameIMP_LOADED_CUTSCENE == false && GameIMP_LOADED_SCENE ==false  )
			return;
	}
	else{
		if (GameIMP_LOADED_SCENE == false) return;
	}

	Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
	UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);
	if (gameimp != 0 && *(UINT32*)gameimp != 0x82001AEC ) return;
	SWAPObjectPlayer(Players_DATA[xuid].object_player,name);
	

}


void DestroyLabelByXUID(XUID xuid){

	
	if (CUTSCENE_MODE){
		if (GameIMP_LOADED_CUTSCENE == false && GameIMP_LOADED_SCENE ==false  )
			return;
	}
	else{
		if (GameIMP_LOADED_SCENE == false) return;
	}


	Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
	boost::shared_ptr<unsigned int> GraphicBufferGuess =  (impl->DocDoculistAction01(7));
	if (GraphicBufferGuess.get()){

		// Use find to get an iterator to the element
		std::map<XUID,PPL_DATA>::iterator it = Players_DATA.find(xuid);
		PPL_DATA* d = &it->second; // Pointer to the PPL_DATA object
		


		if (d->CsdObjectDrawable) {
			
			d->CsdObjectDrawable->Release();
			d->CsdObjectDrawable = 0;
			d->CSD = 0;
			d->CSD_SHADER = 0;
			d->TechniqueCSD3D = 0;
		
		
		
		
		}
//		if (d->CSD) {d->CSD->DestroyObject(1); d->CSD = 0;}
//		if (d->TechniqueCSD3D){d->TechniqueCSD3D->DestroyObject(1);d->TechniqueCSD3D = 0;}
//		if (d->CSD_SHADER){d->CSD_SHADER->DestroyObject(1);d->CSD_SHADER = 0;}

	}

}
void SpawnPlayerLabelByXUID(XUID xuid){


	if (CUTSCENE_MODE){
		if (GameIMP_LOADED_CUTSCENE == false && GameIMP_LOADED_SCENE ==false  )
			return;
	}
	else{
		if (GameIMP_LOADED_SCENE == false) return;
	}


	Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
	boost::shared_ptr<unsigned int> GraphicBufferGuess =  (impl->DocDoculistAction01(7));
	if (GraphicBufferGuess.get()){ //WorldImp


	// Use find to get an iterator to the element
	std::map<XUID,PPL_DATA>::iterator it = Players_DATA.find(xuid);
	PPL_DATA* d = &it->second; // Pointer to the PPL_DATA object
	if (d->CSD != 0 ) return;

   
	BranchTo(0x82371620,int,&d->CSD_SHADER,&std::string("shader/primitive/csd3D.fx"));
	

	int v18 = (int)d->CSD_SHADER;
	(int *)(*(int (__fastcall **)(int *, int, int))(*(_DWORD *)v18 + 0x18))((int*)&d->TechniqueCSD3D, (int)d->CSD_SHADER, (int)"TechniqueCSD3D");


	BranchTo(0x82617570,int,&d->CSD,&std::string("sprite/enemy_powergage/enemy_powergage"));

	

	d->CsdObjectDrawable =  (Sonicteam::SoX::RefCountObject*)BranchTo(0x82616C68,int,malloc06(0xA0),impl->DocGetMyGraphicDevice(),&d->CSD);
	d->CsdObjectDrawable->GetObject<int>();
	BranchTo(0x82616EB0,int,d->CsdObjectDrawable,&Players_DATA[xuid].TechniqueCSD3D);


	
	
	(*(void (__fastcall **)(_DWORD, int *))(*GraphicBufferGuess + 0x14))((_DWORD)GraphicBufferGuess.get(), (int*)&d->CsdObjectDrawable);
	Players_DATA[xuid].CSD->MarathonPlaySceneAnimation("enemy_powergage","DefaultAnim");

	if (Players_DATA[xuid].object_player){

		std::string* chr_name = (std::string*)(Players_DATA[xuid].object_player + 0x1D8);
		int ID = BranchTo(0x824CFCB8,int,chr_name);
		Players_DATA[xuid].CSD->MarathonSetSceneNodeSpriteIndex("enemy_powergage","LivesCast",ID);

	}

	int CsdObjectDrawable_INT = (int)d->CsdObjectDrawable;
	int CsdObject_INT = (int)d->CSD;
	*(_BYTE *)(CsdObjectDrawable_INT + 0x74) = 1;
	*(XMVECTOR *)(CsdObjectDrawable_INT + 0x80) = XMVectorZero();
	Players_DATA[xuid].CSD->SetFlag0x20(0x401); //SetRenderFlag




	Players_DATA[xuid].CSD->MarathonSetSceneNodeText("enemy_powergage","Nick",_socket.XUIDToName(xuid).c_str());

	//std::string test =  _socket.XUIDToName(xuid);
	//DebugLogV2::PrintNextFixed(test);

	//Back when i map all font data >:)

	
	
}
	

}



static void CommonMessages(Socket* socket, SOCKET client_socket, SocketMessage* message){
	
	bool replicate = false;
	if (message->ID == SMDATA_PPL_CHANGE_SCENE::GetID() || message->ID == SMDATA_PPL_CHANGE_SCENE_REPLICATE::GetID()){
		SMDATA_PPL_CHANGE_SCENE* _data_ = EXTRACT_SOCKET_MESSAGE_FROM_MESSAGE_PTR(message, SMDATA_PPL_CHANGE_SCENE);
		if (_data_->sender_xuid == socket->GetXUID(0)) return;
		if (Players_DATA.find(_data_->sender_xuid) == Players_DATA.end()) return;



		Players_DATA[_data_->sender_xuid].scene = _data_->scene_name;

		if (Players_DATA[_data_->sender_xuid].scene == ""){
			DestroyObjectPlayerByXUID(_data_->sender_xuid);
			DestroyLabelByXUID(_data_->sender_xuid);
		}
		else{
			SpawnObjectPlayerByXUID(_data_->sender_xuid);
			SpawnPlayerLabelByXUID(_data_->sender_xuid);
		}



		std::stringstream stream; stream << "[Server] XUID :  " << std::hex << _data_->sender_xuid << " goto " << _data_->scene_name;
		DebugLogV2::PrintNextFixed(stream.str());
		replicate = true;
	}

	

	bool replicate_transform = false;
	if (message->ID == SMDATA_PPL_CHANGE_TRANSFORM::GetID() || message->ID == SMDATA_PPL_CHANGE_TRANSFORM_REPLICATE::GetID()){
		SMDATA_PPL_CHANGE_TRANSFORM* _data_ = EXTRACT_SOCKET_MESSAGE_FROM_MESSAGE_PTR(message, SMDATA_PPL_CHANGE_TRANSFORM);
		if (_data_->sender_xuid == socket->GetXUID(0)) return;

		replicate_transform = true;
		//	std::stringstream ss; ss << "SMDATA_PPL_CHANGE_TRANSFORM X : "  << _data_->Position.x << " Y:" << _data_->Position.y << " Z:" <<  _data_->Position.z; 
		//	DebugLogV2::PrintNextFixed(ss.str());
		Players_DATA[_data_->sender_xuid].RFTransformMatrix0x70_FRAME = _data_->Transform;
		Players_DATA[_data_->sender_xuid].Position_FRAME = _data_->Position;
		Players_DATA[_data_->sender_xuid].Rotation_Posture = _data_->Rotation;
	}

	bool replicate_anim = false;

	if (message->ID == SMDATA_PPL_CHANGE_CBEHAVIOUR::GetID() || message->ID == SMDATA_PPL_CHANGE_CBEHAVIOUR_REPLICATE::GetID()){
		SMDATA_PPL_CHANGE_CBEHAVIOUR* _data_ = EXTRACT_SOCKET_MESSAGE_FROM_MESSAGE_PTR(message, SMDATA_PPL_CHANGE_CBEHAVIOUR);
		if (_data_->sender_xuid == socket->GetXUID(0)) return;
		replicate_anim = true;

		//	 std::stringstream hero; hero << _data_->AnimationID << "-" << _data_->AnimationState;
		//	DebugLogV2::PrintNextFixed(hero.str());

		Players_DATA[_data_->sender_xuid].Context_Animation_ID = _data_->AnimationID;
		Players_DATA[_data_->sender_xuid].Context_Animation_STATE = _data_->AnimationState;


		Players_DATA[_data_->sender_xuid].ContextFlags = _data_->ContextFlags;
		Players_DATA[_data_->sender_xuid].ExportPostureRequestFlag = _data_->ExportPostureRequestFlag;
		Players_DATA[_data_->sender_xuid].ExportWeaponRequestFlag = _data_->ExportWeaponRequestFlag;
		Players_DATA[_data_->sender_xuid].UnknownFlags01 = _data_->UnknownFlags01;
		Players_DATA[_data_->sender_xuid].UnknownFlags0xC8 = _data_->UnknownFlags0xC8;
		Players_DATA[_data_->sender_xuid].StickFixedRotationMb = _data_->StickFixedRotationMb;
		Players_DATA[_data_->sender_xuid].gem_color = _data_->gem_color;


	}
	


	bool replicate_rings = false;

	if (message->ID == SMDATA_PPL_CHANGE_RINGS::GetID() || message->ID == SMDATA_PPL_CHANGE_RINGS::GetID()){
		SMDATA_PPL_CHANGE_RINGS* _data_ = EXTRACT_SOCKET_MESSAGE_FROM_MESSAGE_PTR(message, SMDATA_PPL_CHANGE_RINGS);
		if (_data_->sender_xuid == socket->GetXUID(0)) return;
		replicate_rings = true;
		Players_DATA[_data_->sender_xuid].RingsCount = _data_->RingsCount;
	
		std::stringstream ss; ss << _data_->RingsCount;
		if (Players_DATA[_data_->sender_xuid].CSD){
			Players_DATA[_data_->sender_xuid].CSD->MarathonSetSceneNodeText("enemy_powergage","RingCastTex",ss.str().c_str());
			Players_DATA[_data_->sender_xuid].CSD->MarathonSetSceneNodeText("enemy_powergage","LivesCastTex",ss.str().c_str());
		}


	}
	


	bool replicate_skins = false;

		if (message->ID == SMDATA_PPL_CHANGE_CHR::GetID() || message->ID == SMDATA_PPL_CHANGE_CHR_REPLICATE::GetID()){
		SMDATA_PPL_CHANGE_CHR* _data_ = EXTRACT_SOCKET_MESSAGE_FROM_MESSAGE_PTR(message, SMDATA_PPL_CHANGE_CHR);
		if (_data_->sender_xuid == socket->GetXUID(0)) return;
		replicate_skins = true;
		//	std::stringstream ss; ss << "SMDATA_PPL_CHANGE_TRANSFORM X : "  << _data_->Position.x << " Y:" << _data_->Position.y << " Z:" <<  _data_->Position.z; 
		//	DebugLogV2::PrintNextFixed(ss.str());

	
		std::string after = _data_->sender_character;
		if (int pos = after.find("player/") != std::string::npos){
			after.erase(pos-1,7);

		}
		
		if (Players_DATA[_data_->sender_xuid].object_player && Players_DATA[_data_->sender_xuid].player_pkg != after ){
			DebugLogV2::PrintNextFixed(after);
			SWAPObjectPlayerByXUID(_data_->sender_xuid,after.c_str());
			std::string* chr_name = (std::string*)(Players_DATA[_data_->sender_xuid].object_player + 0x1D8);
			int ID = BranchTo(0x824CFCB8,int,chr_name);
			if (Players_DATA[_data_->sender_xuid].CSD){
				Players_DATA[_data_->sender_xuid].CSD->MarathonSetSceneNodeSpriteIndex("enemy_powergage","LivesCast",ID);
			}
		}
		Players_DATA[_data_->sender_xuid].player_pkg = after;
		


	}



	if (_socket.IsServer()){



			for (std::map<XUID,PPL_DATA>::iterator it = Players_DATA.begin();it != Players_DATA.end();it++){


				//Send Cross Over
				if (replicate){
					SMDATA_PPL_CHANGE_SCENE_REPLICATE _data_;
					_data_.sender_xuid = it->first;
					memcpy(&_data_.scene_name,it->second.scene.c_str(),it->second.scene.length()+1);
					SocketMessage msg  =  DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_data_);	
					socket->SendTCPMessageToClients(&msg);

				}

				if (replicate_skins){
					SMDATA_PPL_CHANGE_CHR _data_;
					_data_.sender_xuid = it->first;
					memcpy(&_data_.sender_character,it->second.player_pkg.c_str(),it->second.player_pkg.size()+1);
					SocketMessage msg  =  DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_data_);
					socket->SendUDPMessageToClients(&msg);
				}
				if (replicate_rings){



					SMDATA_PPL_CHANGE_RINGS _data_;
					_data_.sender_xuid = it->first;
					_data_.RingsCount = it->second.RingsCount;
				
					SocketMessage msg  =  DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_data_);
					socket->SendUDPMessageToClients(&msg);



				}
				if (replicate_anim){



					SMDATA_PPL_CHANGE_CBEHAVIOUR_REPLICATE _data_;
					_data_.sender_xuid = it->first;
					_data_.AnimationID = it->second.Context_Animation_ID;
					_data_.AnimationState = it->second.Context_Animation_STATE;

					_data_.ContextFlags = it->second.ContextFlags;
					_data_.ExportPostureRequestFlag = it->second.ExportPostureRequestFlag;
					_data_.ExportWeaponRequestFlag = it->second.ExportWeaponRequestFlag;
					_data_.UnknownFlags01 = it->second.UnknownFlags01;
					_data_.UnknownFlags0xC8 = it->second.UnknownFlags0xC8;
					_data_.StickFixedRotationMb = it->second.StickFixedRotationMb;
					_data_.gem_color = it->second.gem_color;



					SocketMessage msg  =  DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_data_);
					socket->SendUDPMessageToClients(&msg);



				}
				if (replicate_transform){


					SMDATA_PPL_CHANGE_TRANSFORM_REPLICATE _data_;
					_data_.sender_xuid = it->first; // Get the XUID
					_data_.Position = it->second.Position_FRAME; // Get the position
					_data_.Transform = it->second.RFTransformMatrix0x70_FRAME; // Get the transform matrix
					_data_.Rotation = it->second.Rotation_Posture;


					//	std::stringstream ss; ss << "SMDATA_PPL_CHANGE_TRANSFORM X : "  << _data_->Position.x << " Y:" << _data_->Position.y << " Z:" <<  _data_->Position.z; 
					//	DebugLogV2::PrintNextFixed(ss.str());

					SocketMessage msg = DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_data_);

					// Uncomment the line below to send the message
					socket->SendUDPMessageToClients(&msg); // Ensure socket is defined and initialized
				}
			}
	}











}

// Function to handle client messages
static void ClientMessages(Socket* socket, SOCKET client_socket, SocketMessage* message) {
	
	
	




CommonMessages(socket,client_socket,message);



}

// Function to handle server messages
static void ServerMessages(Socket* socket, SOCKET client_socket, SocketMessage* message) {


	CommonMessages(socket,client_socket,message);





}
// Function to handle a new client connection
static void ServerClientConnected(Socket* socket, SOCKET client_socket) {
 
	
}
static void ServerClientLost(Socket* socket, SOCKET client_socket) {

}
float trigger_menu = 0.0f;



extern "C" int EngineDocOnUpdateHE(Sonicteam::DocMarathonImp* a1, double a2) {



	for (std::map<XUID,PPL_DATA>::iterator it = Players_DATA.begin();it != Players_DATA.end();it++){

		if (it->first == _socket.GetXUID(0)){continue; };
		PPL_DATA* data =  &it->second;
		if (data->CSD){
			data->CSD->CsdLink0x8(a2);

		}
		if (Sonicteam::SoX::RefCountObject* drawable =  data->CsdObjectDrawable){

		//	std::stringstream cantjust; cantjust <<  "XUID : " << it->first << std::hex << " X : " << data->Position_FRAME.x << " Y : " << data->Position_FRAME.y << " Z : " << data->Position_FRAME.z;
		//	DebugLogV2::PrintNextFixed(cantjust.str());
			int CsdObjectDrawable_INT = (int)data->CsdObjectDrawable;
			*(_BYTE *)(CsdObjectDrawable_INT + 0x74) = 1;
			*(XMVECTOR *)(CsdObjectDrawable_INT + 0x80) = data->Position_FRAME + XMVectorSet(0,140,0,0);		
		}

	}

	

	if (_socket.IsClient()){

		if (_socket.GetConnectStatus() != 1) {
			_socket.InitClient();
		}
		else if (_socket.GetConnectStatus() == 1) {
			if (_socket.IsWorks()) _socket.UpdateClient(1.0);
		}
			
	}
	else if (_socket.IsServer()){
		_socket.UpdateServer(1.0);
	}




	

	if (ATG::GAMEPAD* gc = ATG::Input::GetMergedInput()) {


		if (gc->wLastButtons & XINPUT_GAMEPAD_BACK) {
				trigger_menu += a2;
	
		}
	
		if (trigger_menu > 1.0f){
			trigger_menu = 0;

			LPCWSTR g_pwstrButtonsX[4] = { L"Host Server",L"Join Server",L"Disable Socket",L"Test" };
			MESSAGEBOX_RESULT result;
			XOVERLAPPED m_Overlapped; 
			XShowMessageBoxUI(0,L"Server",L"Menu",4,g_pwstrButtonsX,4,XMB_ALERTICON,&result,&m_Overlapped);

			while (!( XHasOverlappedIoCompleted( &m_Overlapped ) )){				}

			if (result.dwButtonPressed == 1 || gc->wLastButtons & XINPUT_GAMEPAD_DPAD_UP){

				_socket = Socket();
				
	
				_socket.InitSockets();
				_socket.SetAddress(Socket::IP_ADDR, htons(1001));
				_socket.SetBind();
				_socket.SetNonBlockingMode();
				_socket.SetAddress(Socket::IP_ADDR, htons(1000));
				_socket.InitClient();
			//	_socket.MSG_HANDLE_CLIENT_JOIN_SERVER = ServerClientConnected;
				_socket.MSG_HANDLE_CLIENT_MESSAGES = ClientMessages;
				_socket.MSG_HANDLE_CLIENT_XUI_JOIN = MSG_HANDLE_CLIENT_XUI_JOIN;


				PPL_DATA data =   PPL_DATA();

				data.local = true;
				Players_DATA[_socket.GetXUID(0)] =data; 
			
				DebugLogV2::PrintNextFixed("Connect");

			
		


			


			}
			else if (result.dwButtonPressed == 0 ||  gc->wLastButtons & XINPUT_GAMEPAD_DPAD_DOWN){

		
				_socket = Socket();

				_socket.InitSockets();
				_socket.SetAddress(Socket::IP_ADDR, htons(1000)); 
				_socket.SetBind();
				_socket.SetNonBlockingMode();
				_socket.InitServer();
				_socket.MSG_HANDLE_SERVER_CLIENT_JOIN = ServerClientConnected;
				_socket.MSG_HANDLE_SERVER_MESSAGES = ServerMessages;
				_socket.MSG_HANDLE_SERVER_CLIENT_LOST_CONNECTION = ServerClientLost;
				_socket.MSG_HANDLE_SERVER_XUI_JOIN = MSG_HANDLE_SERVER_XUI_JOIN;

				PPL_DATA data =   PPL_DATA();
				data.local = true;
				Players_DATA[_socket.GetXUID(0)] =data; 

				DebugLogV2::PrintNextFixed("Host");

			//	SpawnPlayerLabelByXUID(_socket.GetXUID(0));



				
			
				
		
	
	

			}
			else if (result.dwButtonPressed == 3){

				
			//	SpawnPlayerLabelByXUID(_socket.GetXUID(0));

	
		
				DebugLogV2::PrintNextFixed("SpawnLabel");

			}
			else {
				
			}




		}

	

	


	}
	return BranchTo(0x825EA610, int, a1, a2);
}

int RemoveCHR(int a1,int a2){

	if (a2 == 1){
		for (std::map<XUID,PPL_DATA>::iterator it = Players_DATA.begin();it != Players_DATA.end();it++){
			if (it->second.object_player == a1){
				it->second.object_player = 0;
				break;
			}
		}

	}


	
	return BranchTo(0x82195298,int,a1,a2);

}
int __fastcall sub_8229A5D0(int a1, double a2){

	int v4; // r30
	double v6; // fp13
	float *v7; // r11
	int v12; // r3
	__int64 *v20; // r3
	__int64 *v29; // r30
	__int64 *v30; // r3
	int *v31; // r3
	int v32; // r5
	int result; // r3
	_DWORD *v34; // r3
	int v35; // r29
	__int64 *v36; // r30
	int *v37; // r3
	__int64 *v38; // r3
	int v39; // r30
	_DWORD *v40; // r3
	Sonicteam::SoX::RefCountObject* v41; // [sp+50h] [-230h] BYREF
	float v42; // [sp+54h] [-22Ch] BYREF
	int v43; // [sp+58h] [-228h] BYREF
	int v44; // [sp+5Ch] [-224h] BYREF
	int v45; // [sp+60h] [-220h] BYREF
	float v46; // [sp+64h] [-21Ch] BYREF
	float v47[4]; // [sp+70h] [-210h] BYREF
	char v48[16]; // [sp+80h] [-200h] BYREF
	char v49[4]; // [sp+90h] [-1F0h] BYREF
	int v50; // [sp+94h] [-1ECh]
	char v51[16]; // [sp+A0h] [-1E0h] BYREF
	int v52[4]; // [sp+B0h] [-1D0h] BYREF
	char v53; // [sp+C0h] [-1C0h] BYREF
	char v54[16]; // [sp+D0h] [-1B0h] BYREF
	char v55[16]; // [sp+E0h] [-1A0h] BYREF
	char v56[16]; // [sp+F0h] [-190h] BYREF
	char v57[16]; // [sp+100h] [-180h] BYREF
	char v58[16]; // [sp+110h] [-170h] BYREF
	char v59[16]; // [sp+120h] [-160h] BYREF
	char v60[64]; // [sp+130h] [-150h] BYREF
	char v61[64]; // [sp+170h] [-110h] BYREF
	char v62[64]; // [sp+1B0h] [-D0h] BYREF
	char v63[72]; // [sp+1F0h] [-90h] BYREF


	/*

	if ( (*(_DWORD *)(a1 + 0x184) & 1) != 0  && _socket.IsWorks())     // Touch Ring
	{
		v34 = (_DWORD *)BranchTo(0x823D0E88,int,(int)v49, a1);
		v35 = (*(int (__fastcall **)(_DWORD))(*(_DWORD *)*v34 + 0xC))(*v34);
		if ( v50 )
			sub_821601B8(v50);

		BranchTo(0x825D2350,int,v57,0x82B39EE0)

		v36 = (__int64 *)((int)v63, (int)v57);
		v37 = BranchTo(0x82277768,int*,&v41, a1);
		v38 = (__int64 *)BranchTo(0x82594160,int,(int)v60, (_DWORD *)*v37);
		BranchTo(0x82168C48,int,(int)v62, v38, v36);
		if ( v41 )
			v41->LoseObject();
		v39 = *(_DWORD *)(a1 + 0x180);
		v40 = (_DWORD *)(*(int (__fastcall **)(int))(*(_DWORD *)v35 + 0x24))(v35);
		sub_825FE5B0(&v43, v40, *(_DWORD *)(v39 + 0x3C), (int)v62);
		if ( v43 )
			sub_82631328(v43);
		sub_822791E8(&v45, a1, *(_DWORD *)(*(_DWORD *)(a1 + 0x180) + 0x38));
		if ( v45 )
			RefCountObjectRemoveReference(v45);
		sub_825813A0(a1);
		result = sub_8227CB18(a1);
	}
	else{
		BranchTo(0x8229A5D0 ,int,a1,a2);
	}
	*/
	return 0;
	
}

int __fastcall MainModeOnMessageRecieved(Sonicteam::SoX::MessageReceiver* _this,Sonicteam::SoX::Message* msg){


	int a1 = (int)_this;
	a1 = a1 -0x20;
	int extra_flag = *(int *)(a1 + 0x50) ;

	switch ( msg->MessageInfo )
	{
	case 0x1C001:
		std::stringstream ss;
		ss << "[MainMode][MessageReceiver]0x1C001-" << extra_flag;
		DebugLogV2::PrintNextFixed(ss.str());
		break;
	}



	return BranchTo(0x824A6758 ,int,_this,msg);
}


int __fastcall GameImpOnMessageRecieved(Sonicteam::SoX::MessageReceiver* _this,Sonicteam::SoX::Message* msg){


	int a1 = (int)_this;
	int extra_flag = *(int *)(a1 + 0x50) ;

	std::stringstream ss;
	ss << "[MainMode][MessageReceiver]" << msg->MessageInfo << "-" << msg->MessageInfo2;
	DebugLogV2::PrintNextFixed(ss.str());

	switch ( msg->MessageInfo )
	{
	case 0x1C001:

		break;
	}



	return BranchTo(0x8217BB68 ,int,_this,msg);
}


static void ClearPPLPlayers(){
	for (std::map<XUID,PPL_DATA>::iterator it =		Players_DATA.begin();it !=Players_DATA.end();it++){
		if (it->first == _socket.GetXUID(0)) continue;
		if (it->second.local == true) continue;
		if (it->second.object_player != 0) DestroyObjectPlayerByXUID(it->first);
		if (it->second.CSD != 0) DestroyLabelByXUID(it->first);
	}

}
static void SpawnPPLPlayers(){
	for (std::map<XUID,PPL_DATA>::iterator it =		Players_DATA.begin();it !=Players_DATA.end();it++){
		if (it->first == _socket.GetXUID(0)) continue;
		if (it->second.scene == "") continue;
		if (it->second.local == true) continue;
		if (it->second.object_player == 0) SpawnObjectPlayerByXUID(it->first);
		if (it->second.CSD == 0) SpawnPlayerLabelByXUID(it->first);
	}

}



	#define  GAMEIMP_ON_LEVEL 1
	#define  GAMEIMP_ON_CUTSCENE 2
	#define  GAMEIMP_ON_CGI_CUTSCENE 3
	#define  GAMEIMP_ON_RESULT 4
	#define  GAMEIMP_ON_CUTSCENE_END 5
	#define  GAMEIMP_DESTROY_ALL 9

	#define GAMEIMP_MY_SWAP_TERRAIN 1
	#define GAMEIMP_MY_RESPAWN 2
	#define GAMEIMP_MY_CUTSCENE 4
	#define GAMEIMP_MY_CUTSCENE_EN 8
	#define GAMEIMP_MY_DESTROY 0x10

#define  GAMEIMP_START_LEVEL 1
#define  GAMEIMP_START_CUTSCENE 2
#define  GAMEIMP_START_CGI_CUTSCENE 3
#define  GAMEIMP_STOP_LEVEL 4
#define  GAMEIMP_STOP_CUTSCENE 5
#define  GAMEIMP_DESTROY_ALL 9


HOOK(int,__fastcall,GameImpOnChangeActions,0x82184F28,int a1, int a2){
	int v4; // r28
	int a1l;



	v4 = *(_DWORD *)(a1 + 4);
	*(_DWORD *)(a1 + 8) = a2;
	*(_DWORD *)(a1 + 4) = a2;

	if (a2 == 2 && CUTSCENE_MODE == true ){
		BranchTo(0x8216A6A8,int,a1); // Havok for cutscenes

	}

	int case1 = *(int*)(a1 + 0x4);
	int case2 = *(int*)(a1 + 0x8);


	std::stringstream ss; ss << "Before : ";  ss << v4; ss << "-"; ss << a2;
	DebugLogV2::PrintNextFixed(ss.str());



	if (case2 != 4) ClearPPLPlayers();
	
	if (case2 == GAMEIMP_DESTROY_ALL){
		const char* scene_nm = "";
		SMDATA_PPL_CHANGE_SCENE _msg_data;
		_msg_data.sender_xuid = _socket.GetXUID(0);
		memcpy(&_msg_data.scene_name,scene_nm,strlen(scene_nm)+1);
		SocketMessage msg = DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_msg_data);
		Players_DATA[_socket.GetXUID(0)].scene = scene_nm;
		_socket.SendTCPMessageToSRCL(&msg);

	}

	if (case2 != 4){ GameIMP_LOADED_SCENE = false; GameIMP_LOADED_CUTSCENE = false;}



	//0-1, Default

	//Death 1-0, 0-1


	switch ( v4 )
	{
	case 1:
		switch ( a2 )
		{
		case 2:
		case 3:
		case 6:
		case 7:
			goto LABEL_3;
		case 4:
		case 8:
			goto LABEL_15;
		default:
			goto LABEL_4;
		}
	case 2:
		a1l = BranchTo(0x82180260,int,a1, a2);
		break;
	case 3:
		a1l = BranchTo(0x821804A8,int,a1);
		break;
	case 4:
		switch ( a2 )
		{
		case 2:
		case 3:
		case 6:
		case 7:
LABEL_3:
			BranchTo(0x8216E088,int,a1);
			a1l = BranchTo(0x8217F878,int,a1, 1);
			break;
		default:
LABEL_4:
			a1l = BranchTo(0x8216E088,int,a1);
			break;
		}
		break;
	case 5:
		a1l = BranchTo(0x82581100,int,(_DWORD *)(a1 + 0x13E4), 0);
		if ( (_BYTE)a1l )
		{
			a1l = BranchTo(0x82615D60,int,a1 + 0x13E4);
			if ( a1l )
				a1l = (*(int (__fastcall **)(int, int))(*(_DWORD *)a1l + 4))(a1l, 1);
		}
		break;
	case 6:
		a1l = BranchTo(0x821805F0,int,(_DWORD *)a1);
		break;
	case 7:
		if ( *(_DWORD *)(a1 + 0x1808) )
			*(_DWORD *)(a1 + 0x1808) = 0;
		BranchTo(0x8260DF88,int,*(_DWORD *)(a1 + 0xC), a1 + 0x1548, 1);
		break;
	default:
		break;
	}
LABEL_15:
	switch ( a2 )
	{
	case 1:
		switch ( v4 )
		{
		case 0:
		case 2:
		case 3:
		case 5:
		case 6:
		case 7:
			BranchTo(0x821826A8,int,a1);
			break;
		default:
			break;
		}
		a1l = BranchTo(0x82177C88,int,a1, a2);
		break;
	case 2:
		BranchTo(0x82181448,int,(int *)a1, a2);
		break;
	case 3:
		BranchTo(0x82181A90,int,a1);
		break;
	case 4:
		if ( v4 >= 2 && v4 <= 3 )
			BranchTo(0x821820D0,int,a1, a2);                   // Restart-Load
		BranchTo(0x82172188,int,(int *)a1);
		break;
	case 5:
		a1l = (int)BranchTo(0x82172258,int,a1);
		break;
	case 6:
		a1l = BranchTo(0x82181C30,int,(_DWORD *)a1);
		break;
	case 7:
		BranchTo(0x821715E8,int,a1);
		break;
	case 8:
		a1l = (int)BranchTo(0x82172308,int,a1);
		break;
	case 9:
		BranchTo(0x8216E088,int,a1);
		BranchTo(0x8217F878,int,a1, 1);
		BranchTo(0x8216A548,int,a1);
		break;
	default:
		break;
	}




	switch (case2){
			case GAMEIMP_START_LEVEL:
				{
					GameIMP_LOADED_SCENE = true;
					std::string* t01 = (std::string *)(a1 + 0x1214);
					const char* scene_nm = t01->c_str();
					SMDATA_PPL_CHANGE_SCENE _msg_data;
					_msg_data.sender_xuid = _socket.GetXUID(0);
					memcpy(&_msg_data.scene_name,scene_nm,strlen(scene_nm)+1);
					SocketMessage msg = DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_msg_data);
					Players_DATA[_socket.GetXUID(0)].scene = scene_nm;
					_socket.SendTCPMessageToSRCL(&msg);
					SpawnPPLPlayers();
				}
			case GAMEIMP_START_CUTSCENE:
				GameIMP_LOADED_CUTSCENE = true;
				if (CUTSCENE_MODE) SpawnPPLPlayers();
				break;
	}



	std::stringstream ss1; ss1 << "After : ";  ss1 << v4; ss1 << "-"; ss1 << a2;
	DebugLogV2::PrintNextFixed(ss1.str());

	return a1;

}


int __fastcall GameImpEngGlobalActionsRecieved(int a1, double a2, int a3, int a4, int a5){



	BranchTo(0x82185D30,int,a1,a2);
	
	switch ( *(_DWORD *)(a1 + 4) )
	{
	case GAMEIMP_ON_LEVEL:
		{
			int index =1 ;
			for (std::map<XUID,PPL_DATA>::iterator it = Players_DATA.begin();it != Players_DATA.end();it++){
				if (it->second.local) continue;
				if (it->first == _socket.GetXUID(0)) continue;
		
				if (it->second.object_player){
					Sonicteam::DocMarathonImp* impl = *(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
					Sonicteam::GameImp* gameimp = *(Sonicteam::GameImp**)(impl->DocCurrentMode + 0x6C);
					Sonicteam::SoX::MessageReceiver* PMessages = (Sonicteam::SoX::MessageReceiver *)(it->second.object_player + 0x20);
					
					int buffer[0x40] = {0};
					buffer[0] = 0x10007;
					PMessages->OnMessageRecieved((Sonicteam::SoX::Message*)buffer);

					if (index >= 4) {continue;}
					BranchTo(0x82457BE8,int,gameimp->GamePropActiveArea[index],	&buffer[4]);
					BranchTo(0x82461510,int,gameimp->GamePropGenerateArea[index],	&buffer[4]);


					
				}
				index++;

			}
			{

				Sonicteam::GameImp* gameimp = (Sonicteam::GameImp*)(a1);
				if (gameimp->DocMarathon->DocGameRuleContext) BranchTo(0x821EDDB0,int,gameimp->DocMarathon->DocGameRuleContext,Players_DATA.size());

			}

		}
		break;
	
	}
	return 0;
}



int __fastcall GameIMP_DESTRUCTION(int a1, char a2){


	GameIMP_LOADED_SCENE = false;
	if (_socket.IsClient() || _socket.IsServer()){

		//Send Client XUID to Host
		{
			SMDATA_PPL_CHANGE_SCENE _msg_data;
			_msg_data.sender_xuid = _socket.GetXUID(0);
			memcpy(&_msg_data.scene_name,"",11);
			SocketMessage msg = DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_msg_data);

			Players_DATA[_socket.GetXUID(0)].scene = "";
			_socket.SendTCPMessageToSRCL(&msg);
		}

		for (std::map<XUID,PPL_DATA>::iterator it =		Players_DATA.begin();it !=Players_DATA.end();it++){
			it->second.object_player  = 0;
			it->second.CSD = 0;

		}

	}


	return BranchTo(0x82180820,int,a1,a2);
}


int __fastcall ObjectUpdate(int a1, double a2){



	bool IsLocalPlayer = GetLocalPlayer() == a1;
	bool IsNetworkPlayer = false;
	PPL_DATA* NetworkPlayer_DATA;


	if (_socket.IsWorks() && IsLocalPlayer == false){	

		for (std::map<XUID,PPL_DATA>::iterator it = Players_DATA.begin();it != Players_DATA.end();it++){
			if (it->second.object_player == 0 ) continue;
			if (it->second.object_player != a1) continue;
			if (it->second.local) continue;;
			NetworkPlayer_DATA = &it->second;
			IsNetworkPlayer = true;
		//	std::stringstream bruhx;  bruhx << "BRUHHX"   << it->second.Context_Animation_ID << "-" << it->second.Context_Animation_STATE;
		//	DebugLogV2::PrintNextFixed(bruhx.str());

			
		}
	}

	



	Sonicteam::SoX::Engine::Task* _this = (Sonicteam::SoX::Engine::Task*)a1;
	Sonicteam::DocMarathonImp* docmarathon =  (Sonicteam::DocMarathonImp*)_this->TaskEngineDoc;

	int CameraManPlayer =  BranchTo(0x82160658,int,a1, *(_DWORD *)(a1 + 0x90));

	int CameraManPlayer_MessageReceiver = CameraManPlayer + 0x20;
	if ( !CameraManPlayer ) CameraManPlayer_MessageReceiver = 0;
	 *(_DWORD *)(a1 + 0x94) = CameraManPlayer_MessageReceiver;// Store Camera O_O

	int LastSetupModuleIndex = *(_DWORD *)(a1 + 0x100);
	int NextSetupModuleIndex = *(_DWORD *)(a1 + 0xFC);

	 if ( NextSetupModuleIndex != LastSetupModuleIndex )
		 BranchTo(0x82195858,int,a1,docmarathon,LastSetupModuleIndex);

	 std::string chara_last = *(std::string*)(a1 + 0x1D8);
	 BranchTo(0x82197028,int,a1); //SuperFUNC
	 std::string chara_next = *(std::string*)(a1 + 0x1D8);

	 if (chara_last != chara_next && _socket.IsWorks() && IsLocalPlayer){


		 const char* plr_lua =  ((std::string*)(a1 + 0x58))->c_str();
		 SMDATA_PPL_CHANGE_CHR _data3;
		 _data3.sender_xuid = _socket.GetXUID(0);
		 memcpy(&_data3.sender_character,plr_lua,strlen(plr_lua)+1);
		 SocketMessage msg3  =  DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_data3);
		 _socket.SendUDPMessageToSRCL(&msg3);
		 Players_DATA[_socket.GetXUID(0)].player_pkg =plr_lua;

	 }


	 float PlayerSpeed_Delta = (float)(*(float *)(a1 + 0x17C) * (float)a2);// 0x180 - CameraYPos
	 int arg_buffer_3[3];
	 arg_buffer_3[2] = *(int*)&PlayerSpeed_Delta;
	 arg_buffer_3[1] = *(int*)&PlayerSpeed_Delta;
	 arg_buffer_3[0] = 0x82581EA8;


	 //Simplified Way (ai,zock, automatic_dead,lockon_dead)
	 std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>* IStepableP1 = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>*>(a1 + 0x24C);  
	 for ( std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>::iterator it = IStepableP1->begin();it != IStepableP1->end();it++){
		Sonicteam::Player::IStepable*  boost_step = (*it).get();
		boost_step->OnStepable(PlayerSpeed_Delta);

	 }

	 BranchTo(0x82196C10,int,a1);
	 BranchTo(0x82196B98,int,a1);
	 BranchTo(0x821B7968,int,a1, a1 + 0x29C, 0);
	 BranchTo(0x82196AB8,int,a1);

	 if (!IsNetworkPlayer) BranchTo(0x82196F78,int,a1); //CheckAIFLAGS


	 if (IsNetworkPlayer){

		 Sonicteam::Player::State::CommonContext* PlayerContext = *(Sonicteam::Player::State::CommonContext**)((*(int*)(a1 + 0xE4)) + 0x50);
		 if ( Sonicteam::Player::State::SonicContext* sonic_context =  dynamic_cast<Sonicteam::Player::State::SonicContext*>(PlayerContext)){
			 sonic_context->CurrentGemImage = NetworkPlayer_DATA->gem_color;
		 }
	 }

	 BranchTo(0x82196850,int,a1); //UpgradeRunner(


//

	

	 if (!IsNetworkPlayer){
		 if (Sonicteam::Player::State::IMachine* PlayerMachine = *(Sonicteam::Player::State::IMachine**)(a1 + 0xE4)){

			 if (Sonicteam::Player::State::ICommonContext* context = dynamic_cast<Sonicteam::Player::State::ICommonContext*>(PlayerMachine->GetMashineContext().get())){
				 context->ICOnPostInputTick();

				 std::stringstream ss; ss <<std::hex << context->Input;
				// DebugLogV2::PrintNextFixed(ss.str());

				 if ((context->Input & 0x80) != 0 && PlayerMachine->GetCurrentMashineStateID() != 0x26){
					 PlayerMachine->ChangeMashineState(0xA);
				 }
			 }

			 PlayerMachine->OnMashineTick(PlayerSpeed_Delta);



		 }
		
	 }
	
	 if (IsNetworkPlayer){

		 Sonicteam::Player::State::CommonContext* PlayerContext = *(Sonicteam::Player::State::CommonContext**)((*(int*)(a1 + 0xE4)) + 0x50);
		 PlayerContext->AnimationState = NetworkPlayer_DATA->Context_Animation_STATE;
		 PlayerContext->CurrentAnimation = NetworkPlayer_DATA->Context_Animation_ID;

		 if (dynamic_cast<Sonicteam::Player::State::CommonContext*>(PlayerContext)){

	
	
			 PlayerContext->ContextIFFlag2 = NetworkPlayer_DATA->ContextFlags;
			 PlayerContext->ExportPostureRequestFlag = NetworkPlayer_DATA->ExportPostureRequestFlag;
			 PlayerContext->ExportWeaponRequestFlag = NetworkPlayer_DATA->ExportWeaponRequestFlag;
			 PlayerContext->ContextIFFlag = NetworkPlayer_DATA->UnknownFlags01;
			 PlayerContext->UnknownFlags0xC8 = NetworkPlayer_DATA->UnknownFlags0xC8;
			 PlayerContext->StickFixedRotationMb = NetworkPlayer_DATA->StickFixedRotationMb;



		//	 std::stringstream test; test << "[NetworkPlayer] : ";
		//	 test << PlayerContext->ContextFlags << " " << PlayerContext->ExportWeaponRequestFlag << " " << PlayerContext->UnknownFlags01 << " " << PlayerContext->UnknownFlags0xC8;
		//	 DebugLogV2::PrintNextFixed(test.str());
		 }


	 }


	 BranchTo(0x821966E0,int,a1);	
	 BranchTo(0x82196CF8,int,a1);

	 //gamemaster,lockon,homing,path_ld,lockon_lightdash,gauge,sonic_weapons,input vehicle,rodeo,amigo change,talk,waterslider,item,input,gravity,impulse,path,path_ld,path_col,score
	 std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>* IStepableP2 = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>*>(a1 + 0x25C);

	 for ( std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>::iterator it = IStepableP2->begin();it != IStepableP2->end();it++){
		 Sonicteam::Player::IStepable*  boost_step = (*it).get();
		 boost_step->OnStepable(PlayerSpeed_Delta);

		
	 }

	 int PlayerPosture = *(_DWORD *)(a1 + 0xDC);




	
	 if (IsNetworkPlayer){
		 
		 if (Sonicteam::Player::RootFrame* PlayerFrame = *(Sonicteam::Player::RootFrame **)(a1 + 0xCC)){

			 PlayerFrame->RFTransformMatrix0x70 = NetworkPlayer_DATA->RFTransformMatrix0x70_FRAME;
			 *(XMVECTOR*)((*(UINT32*)(a1 + 0xDC)) + 0xB0)  = NetworkPlayer_DATA->Position_FRAME;
			 *(XMVECTOR*)((*(UINT32*)(a1 + 0xDC)) + 0xC0)  = NetworkPlayer_DATA->Rotation_Posture;
		 }


		 if ( PlayerPosture && !*(_BYTE *)(a1 + 0xC9)  )
		 {
			 (*(void (__fastcall **)(int, double))(*(_DWORD *)PlayerPosture + 8))(PlayerPosture, PlayerSpeed_Delta);// 82200538 (c_posture_control_func)
		 }
	 }


	 if ( PlayerPosture && *(_BYTE *)(a1 + 0xC9) )
	 {
		 (*(void (__fastcall **)(int, double))(*(_DWORD *)PlayerPosture + 8))(PlayerPosture, PlayerSpeed_Delta);// 82200538 (c_posture_control_func)
	 }
	 else
	 {
		 Sonicteam::Player::RootFrame* PlayerFrame = *(Sonicteam::Player::RootFrame **)(a1 + 0xCC);
		 if ( PlayerFrame)
		 {
			 PlayerFrame->FrameGetTransformMatrix2();
			 XMMATRIX matrix =  PlayerFrame->FrameGetTransformMatrix2();
			 BranchTo(0x82594698,int,*(_DWORD **)(a1 + 0xCC),&matrix);
		 }
	 }
	 if (IsNetworkPlayer){
		 if (Sonicteam::Player::RootFrame* PlayerFrame = *(Sonicteam::Player::RootFrame **)(a1 + 0xCC)){

			 PlayerFrame->RFTransformMatrix0x70 = NetworkPlayer_DATA->RFTransformMatrix0x70_FRAME;
			 *(XMVECTOR*)((*(UINT32*)(a1 + 0xDC)) + 0xB0)  = NetworkPlayer_DATA->Position_FRAME;
			 *(XMVECTOR*)((*(UINT32*)(a1 + 0xDC)) + 0xC0)  = NetworkPlayer_DATA->Rotation_Posture;
		 }

	 }

	



	 BranchTo(0x82196768,int,a1);
	 //model ,(one or more )
	 std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>* IStepableP3 = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>*>(a1 + 0x26C);
	 for ( std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>::iterator it = IStepableP3->begin();it != IStepableP3->end();it++){
		 Sonicteam::Player::IStepable*  boost_step = (*it).get();
		 boost_step->OnStepable(PlayerSpeed_Delta);

		 

	 }
//??
	 std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>* IStepableP4 = reinterpret_cast<std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>*>(a1 + 0x27C);
	 for ( std::vector<boost::shared_ptr<Sonicteam::Player::IStepable>>::iterator it = IStepableP4->begin();it != IStepableP4->end();it++){
		 Sonicteam::Player::IStepable*  boost_step = (*it).get();
		 boost_step->OnStepable(PlayerSpeed_Delta);
		
	 }


	 int result = *(_DWORD *)(a1 + 0x2FC);
	 *(unsigned long long *)(a1 + 0x200) = 0; // v2 but it == 0
	 *(unsigned long long *)(a1 + 0x214) = 0;
	 if ( result )
		 BranchTo(0x82533520,int,result,PlayerSpeed_Delta);


	 if (_socket.IsWorks() && IsLocalPlayer){
		 Sonicteam::Player::RootFrame* PlayerFrame = *(Sonicteam::Player::RootFrame **)(a1 + 0xCC);
		 Sonicteam::Player::State::CommonContext* PlayerContext = *(Sonicteam::Player::State::CommonContext**)((*(int*)(a1 + 0xE4)) + 0x50);
		 Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		 UINT32 gameimp = *(UINT32*)(impl->DocCurrentMode + 0x6C);


		  SMDATA_PPL_CHANGE_TRANSFORM _data_;
		  SMDATA_PPL_CHANGE_CBEHAVIOUR _data2_;

		 {
			 _data_.sender_xuid = _socket.GetXUID(0);
			 _data_.Transform = PlayerFrame->FrameGetTransformMatrix2();
			 _data_.Position = *(XMVECTOR*)((*(UINT32*)(a1 + 0xDC)) + 0xB0) ;
			 _data_.Rotation = *(XMVECTOR*)((*(UINT32*)(a1 + 0xDC)) + 0xC0) ;
		 }
		 {
			 _data2_.sender_xuid = _socket.GetXUID(0);
			 _data2_.AnimationID =   PlayerContext->CurrentAnimation;
			 _data2_.AnimationState = PlayerContext->AnimationState;
			 if (dynamic_cast<Sonicteam::Player::State::CommonContext*>(PlayerContext)){
				 _data2_.ContextFlags = PlayerContext->ContextIFFlag2;
				 _data2_.ExportPostureRequestFlag = PlayerContext->ExportPostureRequestFlag;
				 _data2_.ExportWeaponRequestFlag = PlayerContext->ExportWeaponRequestFlag;
				 _data2_.UnknownFlags01 = PlayerContext->ContextIFFlag;
				 _data2_.UnknownFlags0xC8 = PlayerContext->UnknownFlags0xC8;
				 _data2_.StickFixedRotationMb = PlayerContext->StickFixedRotationMb;

					 if ( Sonicteam::Player::State::SonicContext* sonic_context =  dynamic_cast<Sonicteam::Player::State::SonicContext*>(PlayerContext)){
						 _data2_.gem_color = sonic_context->CurrentGemImage;
					 }
				


				// std::stringstream test; test << "[NetworkPlayer] : ";
				// test << PlayerContext->ContextFlags << " " << PlayerContext->ExportWeaponRequestFlag << " " << PlayerContext->UnknownFlags01 << " " << PlayerContext->UnknownFlags0xC8;
				// DebugLogV2::PrintNextFixed(test.str());


			 }
			 
			 
			// std::stringstream hero; hero << _data2_.AnimationID << "-" << _data2_.AnimationState;
			 //DebugLogV2::PrintNextFixed(hero.str());
		 }

		
		
		 
		 SocketMessage msg  =  DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_data_);
		 SocketMessage msg2  =  DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_data2_);
	

		// std::stringstream bruh;  bruh << "X : " << _data_.Position.x << "Y : " << _data_.Position.y << "Z : " << _data_.Position.z;
		// DebugLogV2::PrintNextFixed(bruh.str());


		 if (_socket.IsClient()){
			 _socket.SendUDPMessageToServer(&msg);
			 _socket.SendUDPMessageToServer(&msg2);
			 
		 }
		 else{
			 Players_DATA[_socket.GetXUID(0)].Context_Animation_ID = PlayerContext->CurrentAnimation;
			 Players_DATA[_socket.GetXUID(0)].Context_Animation_STATE = PlayerContext->AnimationState;
			 Players_DATA[_socket.GetXUID(0)].RFTransformMatrix0x70_FRAME =PlayerFrame->FrameGetTransformMatrix2();
			 Players_DATA[_socket.GetXUID(0)].Position_FRAME =*(XMVECTOR*)((*(UINT32*)(a1 + 0xDC)) + 0xB0) ;
			 Players_DATA[_socket.GetXUID(0)].Rotation_Posture =*(XMVECTOR*)((*(UINT32*)(a1 + 0xDC)) + 0xC0) ;

			 if ( Sonicteam::Player::State::SonicContext* sonic_context =  dynamic_cast<Sonicteam::Player::State::SonicContext*>(PlayerContext)){
				  Players_DATA[_socket.GetXUID(0)].gem_color  = sonic_context->CurrentGemImage;
			 }

			 if (dynamic_cast<Sonicteam::Player::State::CommonContext*>(PlayerContext)){
				 Players_DATA[_socket.GetXUID(0)].ContextFlags = PlayerContext->ContextIFFlag2;
				 Players_DATA[_socket.GetXUID(0)].ExportPostureRequestFlag = PlayerContext->ExportPostureRequestFlag;
				 Players_DATA[_socket.GetXUID(0)].ExportWeaponRequestFlag = PlayerContext->ExportWeaponRequestFlag;
				 Players_DATA[_socket.GetXUID(0)].UnknownFlags01 = PlayerContext->ContextIFFlag;
				 Players_DATA[_socket.GetXUID(0)].UnknownFlags0xC8 = PlayerContext->UnknownFlags0xC8;
				 Players_DATA[_socket.GetXUID(0)].StickFixedRotationMb = PlayerContext->StickFixedRotationMb;
			 }

			 _socket.SendUDPMessageToClients(&msg);
			 _socket.SendUDPMessageToClients(&msg2);

		 }
		
	 }
	

	 return 0;

}

int __fastcall ObjectEvents(int a1, Sonicteam::SoX::Message* msg){

	bool IsLocalPlayer = GetLocalPlayer() == a1 - 0x20;


    int result = BranchTo(0x82197598,int,a1,msg);


	if (_socket.IsWorks() && (msg->MessageInfo2 - 0x20) == GetLocalPlayer()){
		if (msg->MessageInfo == 0x13010 && result){
	
			
		}
	}

	return result;
}


static std::map<int, const char*> GameIMP_MESSAGES;
int __fastcall GameIMPMessageReciever(int a1, Sonicteam::SoX::Message* a2){



	if (a2->MessageInfo != 122884 && a2->MessageInfo != 86089){

		std::stringstream test; 
		if (GameIMP_MESSAGES.find(a2->MessageInfo) == GameIMP_MESSAGES.end()){
			test << "[GameImp][Message] " << std::hex << a2->MessageInfo;
		//	DebugLogV2::log.push_back(test.str());
		//	DebugLogV2::PrintNextFixed(test.str());
		}
		else{
//				test << "[GameImp][Message] " << std::hex << GameIMP_MESSAGES[a2->MessageInfo];
///
	//			DebugLogV2::log.push_back(test.str());
	//			DebugLogV2::PrintNextFixed(test.str());
		}

	
	

	}


	if (_socket.IsWorks() && (_socket.IsClient() || _socket.IsServer())){

		if (a2->MessageInfo == 0x15009){
			if ( BranchTo(0x82167CA0,int,a1,a2->MessageInfo2) == 0){
					SMDATA_PPL_CHANGE_RINGS _data3;
					_data3.sender_xuid = _socket.GetXUID(0);
					_data3.RingsCount =   a2->MessageInfo3;	
					SocketMessage msg3  =  DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_data3);
					_socket.SendUDPMessageToSRCL(&msg3);
					Players_DATA[_socket.GetXUID(0)].RingsCount = a2->MessageInfo3;		
				}
		}

		if (a2->MessageInfo == 0x15016){
			//ShowXenonMessage(L"MSG","T");

		}
		//StartObjInfo
		if (a2->MessageInfo == 0x1500A && *(int*)((int)a2 + 0x30) == 0){

			const char* plr_lua =  *(const char**)((int)a2 + 0x34);
	//	ShowXenonMessage(L"MSG",plr_lua);
	//	ShowXenonMessage(L"MSG",*(int*)((int)a2 + 0x30),0);
	
			SMDATA_PPL_CHANGE_CHR _data3;
			_data3.sender_xuid = _socket.GetXUID(0);
			memcpy(&_data3.sender_character,plr_lua,strlen(plr_lua)+1);
			SocketMessage msg3  =  DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_data3);
			_socket.SendUDPMessageToSRCL(&msg3);
			Players_DATA[_socket.GetXUID(0)].player_pkg =plr_lua;

			//	std::stringstream test; test<< std::hex << a2->MessageInfo;;
			//	DebugLogV2::log.push_back(test.str());
			//	DebugLogV2::PrintNextFixed("CHR SPAWN");
		}





	}



	//0x1c001
	//0x1501c -- SCORE CHANGE (a2 + 4 , ActorID)
	//0x15009 -- RINGS CHANGE

	
	
	return BranchTo(0x8217BB68,int,a1,a2);


}

int __fastcall Character_AmigoSwitch(int result){





	int v1; // r31
	int v2; // r11
	_DWORD *v3; // r3
	int v4[2]; // [sp+50h] [-30h] BYREF
	int v5[4]; // [sp+58h] [-28h] BYREF

	v1 = result;
	if ( *(_DWORD *)(result + 0x1C) )             // On Switch Process
	{
		v4[1] = 0;
		v4[0] = 0x1300E;
		result = (*(int (__fastcall **)(int, int *))(*(_DWORD *)(*(_DWORD *)(result + 0x28) + 0x20) + 4))(
			*(_DWORD *)(result + 0x28) + 0x20,
			v4);                             // 0000000082197598
		if ( (_BYTE)result )
		{
			if ( *(byte*)(&v4[1]) )
			{
				v2 = *(_DWORD *)(v1 + 0x28);
				v5[0] = 0x13010;
				if ( v2 )
					v5[1] = v2 + 0x20;
				else
					v5[1] = 0;
				*(byte*)&v5[2] = 1;
				v3 = (_DWORD *)BranchTo(0x82205720,int,(_DWORD *)(v1 + 0xC), 0);// **ReturnMyPhantom
				int HitObject = *(unsigned int*)(*v3 + 0x20) - 0x20;

				if (!IsNetworkPlayer(v2) && !IsNetworkPlayer(HitObject) && (HitObject + 0x20) != 0 && *(byte*)(HitObject + 0xCA) == 0x0 ){
					if (IsLocalPlayer(v2)){
						
						// OBJ AMIGO COLLISION ALSO O_O (82014ED8)
						std::stringstream test; test << "*(byte*)(HitObject + 0xCA) " << std::hex << HitObject;
						//DebugLogV2::PrintNextFixed(test.str());
						SMDATA_PPL_CHANGE_CHR _data_;
						_data_.sender_xuid = _socket.GetXUID(0);
						std::string* str = NULL;
						if ( *(unsigned int*)HitObject ==0x82014ED8 ){

							int v9[3];
							v9[0] = 0x15014;
							v9[1] =  *(int*)(HitObject + 0x184);
							v9[2] = 0xFFFFFFFF;
							int v4 = *(int*)(HitObject+0x4C);	
							(*(int (__fastcall **)(_DWORD, int *))(*(_DWORD *)v4 + 4))(v4, v9);
							int TargetActorID = v9[2];
							int HitObjectAmigo_ObjPlayer = BranchTo(0x82160658,int,HitObject,TargetActorID);
					
							if (HitObjectAmigo_ObjPlayer && *(byte*)(HitObjectAmigo_ObjPlayer + 0xCA) == 0x0 ){
								str =  (std::string*)(HitObjectAmigo_ObjPlayer + 0x58);
							}

						}
						else if ( *(unsigned int*)HitObject ==0x82003564 ){
							str =  (std::string*)(HitObject + 0x58);
						}
			
						if (str && !str->empty() && str->length() > 0 && str->c_str()) {
							const char* chr_full_lua = str->c_str();
							memcpy(&_data_.sender_character,chr_full_lua,strlen(chr_full_lua)+1);
							Players_DATA[_data_.sender_xuid].player_pkg = chr_full_lua;
							SocketMessage msg = DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_data_);
							_socket.SendUDPMessageToSRCL(&msg);
						}

					}

					result = (*(int (__fastcall **)(_DWORD, int *))(*(_DWORD *)*v3 + 4))(*v3, v5);// 0000000082595B90
				}	
			}
		}
	}
	return result;


}

void __fastcall Listener_Common_Input(int _this, Sonicteam::Player::Input::IListenerInputStruc01* PtrStructThatHasCameraObjAndKhronoController, double a3){

	BranchTo(0x82222428 ,int,_this,PtrStructThatHasCameraObjAndKhronoController,a3);

	if (PtrStructThatHasCameraObjAndKhronoController){

		if ((PtrStructThatHasCameraObjAndKhronoController->wLastButtons & SO_GAMEPAD_RAW_BUTTON_DPAD_UP) != 0) {
			 *(_DWORD *)(_this + 0x48) |= 0x400000u;// 
		}
		if ((PtrStructThatHasCameraObjAndKhronoController->wLastButtons & SO_GAMEPAD_RAW_BUTTON_DPAD_DOWN) != 0){
			*(_DWORD *)(_this + 0x48) |= 0x800000u;// 
		}
		if ((PtrStructThatHasCameraObjAndKhronoController->wLastButtons & SO_GAMEPAD_RAW_BUTTON_B) != 0){
			*(_DWORD *)(_this + 0x48) |= 0x80; // 
		}

	}


}

int __fastcall sub_821A0498(Sonicteam::Player::State::BasedObject<Sonicteam::Player::State::CommonContext> *_this,double a2){
	int input = _this->CObjContext->Input;
	if (input & 0x800000u){
		_this->CObjContext->SetAnimation(0x4D);
	}

	if ((_this->CObjContext->AnimationState & 1) != 0){

		if (_this->CObjContext->CurrentAnimation == 0x4D){
			_this->CObjContext->SetAnimation(0x4E);
		}

	}


	if (_this->CObjContext->CurrentStickBorder > 0){
		_this->ObjectMashine->ChangeMashineState(0);
	}
	return 0;
}


int __fastcall SetPlayer(lua_State *L){

	int v4 = lua_tonumber(L, 1);
	int v5 = lua_tonumber(L, 2);
	int v6 = lua_tonumber(L, 3);
	int v7 = lua_tonumber(L, 4);
	const char* v8 = lua_tostring(L, 5);
	int index = lua_tonumber(L, 6);
	bool v10 = lua_toboolean(L, 7);
	
	if (index == 0 && _socket.IsWorks()){
		const char* plr_lua = v8;
		SMDATA_PPL_CHANGE_CHR _data3;
		_data3.sender_xuid = _socket.GetXUID(0);
		memcpy(&_data3.sender_character,plr_lua,strlen(plr_lua)+1);
		SocketMessage msg3  =  DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_data3);
		_socket.SendUDPMessageToSRCL(&msg3);
		Players_DATA[_socket.GetXUID(0)].player_pkg =plr_lua;
	}


	return BranchTo(0x82462948,int,L);
}

void TagBattleMain::GlobalInstall_ONLINE()
{
	Players_DATA = std::map<XUID,PPL_DATA>();
	_socket = Socket();

	WRITE_DWORD(0X820268C4,SetPlayer);
	GameIMP_MESSAGES[0x1501c] = "Score";
	GameIMP_MESSAGES[0x15009] = "Rings";
	GameIMP_MESSAGES[0x1500A] = "SetPlayerLua";


	WRITE_DWORD(0x82001AF0,GameIMPMessageReciever);
	WRITE_DWORD(0x8200356C,ObjectUpdate);
	WRITE_DWORD(0x8200355C,ObjectEvents);

//	WRITE_DWORD(0X82033568	,MainModeOnMessageRecieved);
	//WRITE_DWORD(0x82001AF0	,GameImpOnMessageRecieved);


	WRITE_DWORD(0x82001B3C,GameImpEngGlobalActionsRecieved);
	INSTALL_HOOK(GameImpOnChangeActions);
	//WRITE_DWORD(0x82001AEC,GameIMP_DESTRUCTION);
	WRITE_DWORD(0x8200D104,Character_AmigoSwitch);

	
/*
	players_chr_remap["player/sonic_new"] = "sonic_new.lua";
	players_chr_remap["player/shadow"] = "shadow.lua";
	players_chr_remap["player/silver"] = "silver.lua";

	players_chr_remap["player/tails"] = "tails.lua";
	players_chr_remap["player/knuckles"] = "knuckles.lua";
	players_chr_remap["player/amy"] = "amy.lua";

	players_chr_remap["player/rouge"] = "rouge.lua";
	players_chr_remap["player/omega"] = "omega.lua";


	players_chr_remap["player/blaze"] = "blaze.lua";
	players_chr_remap["player/princess"] = "princess.lua";


	players_chr_remap["player/sonic_fast"] = "sonic_fast.lua";
	players_chr_remap["player/princess"] = "princess.lua";
*/



	WRITE_DWORD(0x82003568,RemoveCHR);
	WRITE_DWORD(0x821A0428 ,0x4E800020);
	WRITE_DWORD(0x821A0498, 0x4E800020);
	WRITE_DWORD(0x821A0468 ,0x4E800020);

	WRITE_DWORD(0x8200378C,sub_821A0498);
	WRITE_DWORD(0x82000950,EngineDocOnUpdateHE);
	WRITE_DWORD(0x8200BD64,Listener_Common_Input);

	

	//WRITE_DWORD(0x8229A5D0)
	
	

}