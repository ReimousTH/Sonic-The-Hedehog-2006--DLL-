#include "DevTitle.h"

namespace DevTitleV2{


	void EditText(UINT32 Label1,const char* str){

		std::string msg = std::string(str);
		int length = msg.length() + 1;
		wchar_t* wcharPtr = new wchar_t[length];
		std::memset(wcharPtr, 0, length * sizeof(wchar_t));
		std::mbstowcs(wcharPtr, msg.c_str(), length);
		DebugLogV2::EditMessage(Label1,wcharPtr);
		free(wcharPtr);

	}

	int __fastcall DevTitleModeMessageReciever(int a1, Sonicteam::SoX::Message* a2){

		DWORD _this = (DWORD )(a1 - 0x20);


		BranchTo(0x824A0E38,int,a1,a2);



		return 1;



	}


	int __fastcall DocLoadMainMode(Sonicteam::DocMarathonImp* _this, int a2){
		
		BranchTo(0x82161AB8,int,_this,a2);

		//BranchTo(0x825E9E28,int,_this,a2);
	
		if ( !*(int*)0x82D35E78 )
			*(int*)0x82D35E78 = BranchTo(0x82161860,int);


		Label1 =  DebugLogV2::SpawnMessage(L"",400,600);
		Label2 =  DebugLogV2::SpawnMessage(L">>$<<","picture(button_a)",640,100);
		Label3 =  DebugLogV2::SpawnMessage(L">>[Selected]:[]<<",10,600);
		Label4 =  DebugLogV2::SpawnMessage(L"",500,200);

		return _this->DocSetCurrentMode(*(int*)0x82D35E78);

	}

	DWORD *__fastcall DevTitleUI(int a1, Sonicteam::DocMarathonImp *a2){


		//if ( !BranchTo(0x82581C68,int,(int)a2) ){
			int mode =  (int)a2->DocCurrentMode;
			int index = *(int*)(mode + 0x50);
			char buffer[100];



			const char* str =  *(const char**)((0x82B10A40 + (index * 8)));

			sprintf(buffer,">>[Selected]:[%s]<< : ", 	str);
			EditText(Label3,(const char*)buffer);
		//}

		


		return BranchTo(0x82162298,DWORD*,a1,a2);

	}



	const char* Names[] = {
		"CStage",
		"SArea",
		"SPlayer",
		"SID",
		"SComment"
	};

	const char* NamesF[] = {
		"CStage",
		"SArea",
		"SPlayer",
		"SID",
		"SComment"
	};


	int __fastcall StageSelectParameterStage(int a1, Sonicteam::DocMarathonImp* a2){



		int docmode = a2->DocCurrentMode;
		int ActiveStageMap  = *(int*)(docmode + 0x54);
		int Index = *(int*)(docmode + 0x5C);
		int IndexLR = *(int*)(docmode + 0x60); //Deep



		std::vector<UINT32>* stage_maps  = (std::vector<UINT32>*)(ActiveStageMap + 0x58);
		std::string* str =  (std::string*)(((*stage_maps)[Index]) + 0x20);
		Names[IndexLR] = str->c_str();
		

		for (int i = IndexLR+1;i<sizeof(Names)/4;i++){
			Names[i] = "...";
		}

		if (IndexLR == 2){

			 UINT32 CurrentStageMap  =  (UINT32)((*stage_maps))[Index];
			 std::vector<UINT32>* stage_maps_C  = (std::vector<UINT32>*)(CurrentStageMap + 0x58);
			 UINT32 IDStageMAP = (UINT32)((*stage_maps_C))[0];
			 std::string* IDStageMAPSTR =  (std::string*)(IDStageMAP + 0x20);
			 std::string* ComentStageMAPSTR =  (std::string*)(IDStageMAP + 0x3C);

			 if (!IDStageMAPSTR->empty()){
				 Names[IndexLR+1] = IDStageMAPSTR->c_str();

			 }
			 if (!ComentStageMAPSTR->empty()){
				 Names[IndexLR+2] = ComentStageMAPSTR->c_str();
			 }
			 
		

		}


		memcpy(&NamesF,Names,sizeof(NamesF));
		char buffer1[1024];
		sprintf(buffer1,">>[%s]<<",Names[IndexLR]);
		NamesF[IndexLR] = buffer1;

	
		
		char buffer[1024];
		sprintf(buffer,"Stage = [%s]\nArea = [%s]\nPlayer = [%s]\nID = [%s]\nComment = [%s]",NamesF[0],NamesF[1],NamesF[2],NamesF[3],NamesF[4]);



		DebugLogV2:EditText(Label4,(const char*)buffer);



		//stage_Map
		//str 0x20      Stage = "aqa ( Aquatic Base )",
		//0x58- result Select StageMap
		//0x50- Something Stagemaps??
		//0x54- Stagemap but changing after Confirm
		//0x5C - Index	




		return BranchTo(0x82161F20,int,a1,a2);
	}

	int __fastcall EngineDocOnUpdate(Sonicteam::DocMarathonImp *a1, double a2){

		if (Label1 && Label2){

			char buffer[100];
			int docmode = (int)a1->DocCurrentMode;
			if (docmode){


				int index = *(int*)(docmode + 0x50);
				const char** str = *(const char***)(docmode + 0x80);
				const char* str2 = *(const char**)(docmode + 0x88);
				int flag = *(int*)(docmode + 0x84);

				if (str != 0){

					sprintf(buffer,">>[%s]:[%s]:[%x]<< :",*str,str2,flag);
					EditText(Label2,(const char*)buffer);


				}
		
			}			


		}

		return BranchTo(0x825EA610,int,a1,a2);
	}

	DWORD *__fastcall StageSelectParameterStageDestructor(DWORD *result, char a2){

		EditText(Label1,"");
		EditText(Label2,"");
		EditText(Label3,"");
		EditText(Label4,"");
	
		return BranchTo(0x82160D20,DWORD*,result,a2);
	}

	int __fastcall DocSetCurrentMode(int result, int a2){

		if (Label1 && Label2 && Label3 && Label3){
		EditText(Label1,"");
		EditText(Label2,"");
		EditText(Label3,"");
		EditText(Label4,"");
		}


		return BranchTo(0x825E8D78,int,result,a2);
	}

	static void ClearTaskFunc(Sonicteam::SoX::Engine::Task* task){
		*(int*)task = 0;
	};


	HOOK(Sonicteam::SoX::Engine::Task*,__stdcall,sub_0x825F0298,0x825F0298,Sonicteam::SoX::Engine::Task* a1,int a2){

		//ShowXenonMessage(L"MSG","DestroyObject");
		
		a1->Empty(a1);
		a1->EmptyParent(a1);
		a1->TaskList.ForEach(ClearTaskFunc);
		a1->TaskList.Empty();
	

		a1->LComponentList.ForEach(0);
		a1->LComponentList.Empty();

		a1->LinkedComponent.RemoveLink();


		Sonicteam::SoX::Memory::IUDestructible::DestroyObject(a1,(int)a2);
		return 0;


	}

	HOOK_EXTERN_C(Sonicteam::SoX::Engine::Task*, __fastcall,sub_82581470,0x82581470 ,void* a1,Sonicteam::SoX::Engine::Doc* doc){

		return new(a1)Sonicteam::SoX::Engine::Task(doc);
	}


	HOOK_EXTERN_C(Sonicteam::SoX::Component*, __fastcall,sub_825BAAA0,0x825BAAA0 ,void* a1,Sonicteam::SoX::Component* comp){

		return new(a1)Sonicteam::SoX::Component(comp);
	}



	
	void GlobalInstall()
	{

		//INSTALL_HOOK(sub_825BAAA0);
	
	
		//INSTALL_HOOK(sub_82581470);



		
	//	WRITE_DWORD(0x82000950,EngineDocOnUpdate);
		
		WRITE_DWORD(0x820009A0,DocSetCurrentMode);

		WRITE_DWORD(0x82160D20,StageSelectParameterStageDestructor);
		WRITE_DWORD(0x82000AFC,StageSelectParameterStage);

		WRITE_DWORD(0x82000B68,DevTitleUI);

		WRITE_DWORD(0x8202B1D8,DevTitleModeMessageReciever);
		WRITE_DWORD(0x82000AAC,DocLoadMainMode);
	}

}