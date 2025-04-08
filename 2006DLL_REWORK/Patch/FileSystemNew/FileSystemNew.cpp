#include "FileSystemNew.h"


#include <Sox/RefCountObject.h>
#include <DocMarathonImp.h>

#include <Hook/HookBase.h>
#include <Hook/HookNew.h>


#include <System/Singleton.h>
#include <System/CreateStatic.h>
#include <Sox/FileSystemXenon.h>
#include <Sox/FileHandleARC.h>
#include <Sox/FileSystemARC.h>

#include <AtgAudio.h>

#include <string>
#include <vector>


#define MOD_DIR "game:\\mods\\"


#include <AtgInput.h>
#include <GameImp.h>



//Custom Task, accurrate to 06
class FSNTask:public Sonicteam::SoX::Engine::Task{

public:

	FSNTask(Sonicteam::SoX::Engine::Task* task):Sonicteam::SoX::Engine::Task(task){

	}
	~FSNTask(){};
	DESTRUCTION_HPP;
	virtual int OnMessageRecieved(SoXMessageType)
	{
		return 0;
	}


	virtual char* GetObjectType()
	{
		return "FileSystemNew Task";
	}


	virtual void OnTaskUpdate(float)
	{

		ATG::GAMEPAD* input = ATG::Input::GetMergedInput(0);


		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		Sonicteam::GameImp* gameimp = *(Sonicteam::GameImp** )(impl->DocCurrentMode + 0x6C);
		if (gameimp){


			if ((input->wLastButtons & XINPUT_GAMEPAD_Y) != 0){
			//	PushXenonMessage(L"MSG","YES");
				size_t task =  (size_t)(gameimp->GamePlayerDisplayTask[0].ThisObject);
				size_t hud_main = *(size_t*)(task + 0x4C);
				Sonicteam::CsdObject* csd = *(Sonicteam::CsdObject**)(hud_main + 0x54);
				Chao::CSD::CProject* cproj =  csd->FCProject;
				const char* name =  cproj->CProjectScene.begin()->first;
				Chao::CSD::CScene* scene  =  (cproj->CProjectScene.begin()->second.get()->get());
				size_t index =  scene->CObjectRaw->GetMotionIndexFromName("DefaultAnim");

			//	PushXenonMessage(L"MSG",(size_t)scene->CObjectRaw);
			//	PushXenonMessage(L"MSG",(size_t)scene->CObjectRaw->_Motion.count);
			//	PushXenonMessage(L"MSG",(const char*)scene->CObjectRaw->_Motion.NameTable.values[0].first.get());


				/*
				for (int i = 0;i<static_cast<size_t>(scene->CObjectRaw->_Motion.count);i++){

					Chao::IndexValueName* ivn =  &scene->CObjectRaw->_Motion.NameTable.values[i];
					if (strcmp(ivn->first.get(),"DefaultAnim") == 0){
						PushXenonMessage(L"MSG",ivn->first.get());
						PushXenonMessage(L"MSG",ivn->second.get());
					}
				//	PushXenonMessage(L"MSG",ivn->first.get());
				}
				*/

			

	//			PushXenonMessage(L"MSG",name);
	//			PushXenonMessage(L"MSG",index);
	

			}

		}
		


	//	PushXenonMessage(L"Task","T");
		return;
	}

};




bool DirectoryGetDirs(std::vector<std::string>& files,std::string& dir){

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((dir+std::string("*")).c_str(), &findFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return false;
	}
	do {
		if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			std::string fileName = findFileData.cFileName;
			files.push_back(fileName);
		}
	} while (FindNextFile(hFind, &findFileData) != 0);
	FindClose(hFind);
	return true;
}










// Declare the function pointer
REF_TYPE(Sonicteam::SoX::IResource) (*ArcHandle)(std::string, int, int) = (REF_TYPE(Sonicteam::SoX::IResource) (*)(std::string, int, int))0x82583528;
std::map<std::string,Sonicteam::SoX::ArcHandle*> DLL_ARCS;	
static std::vector<std::string> files; //cache_later



//AFTER
HOOKV3(0x82160B98,void*,CreateMarathonState,(Sonicteam::DocMarathonImp*,void*),(_this,arg2),Sonicteam::DocMarathonImp* _this, void* arg2){

	Sonicteam::SoX::ArcHandleMgr* mgr = &Sonicteam::SoX::ArcHandleMgr::getInstance();
	Sonicteam::SoX::FileSystemXenon* xenon = &Sonicteam::SoX::FileSystemXenon::getInstance();
	Sonicteam::SoX::FileSystemXenon* FileSystem = &SSINGLETON(Sonicteam::SoX::FileSystemXenon)::getInstance();
	Sonicteam::SoX::FileSystemArc* ArcSystem = &Sonicteam::SoX::FileSystemArc::getInstance();
	//ArcSystem->FileSystemFolder.push_back(std::string("game:\\"));
	xenon->FileSystemFolder.push_back(std::string("game:\\"));
	

	//Load mod arcs
	if (files.size() == 0){
		DirectoryGetDirs(files,std::string(MOD_DIR));

		for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {

			std::string mod_dir = std::string(MOD_DIR) + *it + std::string("\\");
			std::string mod_dir_win32 = mod_dir + "win32\\archives\\";
			std::string mod_dir_xenon = mod_dir + "xenon\\archives\\";
			std::vector<std::string> dir_arcs_xenon;
			std::vector<std::string> dir_arcs_win32;
			xenon->FSDirectoryGetFiles(dir_arcs_xenon,mod_dir_xenon,std::string("arc"));
			xenon->FSDirectoryGetFiles(dir_arcs_win32,mod_dir_win32,std::string("arc"));

			for (std::vector<std::string>::iterator jt = dir_arcs_xenon.begin(); jt != dir_arcs_xenon.end(); ++jt) {
				std::string arc_file =  mod_dir_xenon + *jt;
				arc_file = arc_file.substr(6);
				_this->ArcVector1.push_back(ArcHandle(arc_file,3,0));
				//PushXenonMessage(L"MSG",arc_file.c_str());
				DLL_ARCS[arc_file] = (Sonicteam::SoX::ArcHandle*)(_this->ArcVector1.rbegin())->get();
			}	

			for (std::vector<std::string>::iterator jt = dir_arcs_win32.begin(); jt != dir_arcs_win32.end(); ++jt) {
				std::string arc_file =  mod_dir_win32 + *jt;
				arc_file = arc_file.substr(6);
				_this->ArcVector1.push_back(ArcHandle(arc_file,3,0));
				//PushXenonMessage(L"MSG",arc_file.c_str());
				DLL_ARCS[arc_file] = (Sonicteam::SoX::ArcHandle*)(_this->ArcVector1.rbegin())->get();
			}	
		}
	}		

		



	HANDLE FileHandle;
	std::stringstream arc_full_path;
	std::stringstream error_msg; 



	for (std::map<std::string,FlagFile>::iterator it = FileSystemNew::FileSystemNewMap.begin();it!= FileSystemNew::FileSystemNewMap.end();it++){


		 std::string arc_full_path = FileSystem->FSGetPath(it->first,it->second.flag);

		 if (FileSystem->FSPathExist(arc_full_path)){
			 _this->ArcVector1.push_back(ArcHandle(it->first,it->second.flag,it->second.SaveInMemory)); //-1 IF NOT 1 resource is being loaded but ??? something else happend and it destroy object depsite i can find csdobject in memory
			 DLL_ARCS[arc_full_path] = (Sonicteam::SoX::ArcHandle*)(_this->ArcVector1.rbegin())->get();

		 }
		 else{
			 error_msg << "[FileSystemNew] Failed to Find :  " << arc_full_path << ", press OK to ignore" << std::endl; ;

		 }
		
	}

	if (HookNew::IsEmulated() && error_msg.str().size() > 0){ 

		XOVERLAPPED overlap;
		overlap.hEvent = CreateEvent(0,false,false,0);
		PushXenonMessage(L"Warning",error_msg.str().c_str(),&overlap);
		WaitForSingleObject(overlap.hEvent,INFINITE);
		CloseHandle(overlap.hEvent);
	}





	//Sonicteam::SoX::Engine::RootTask* Root = _this->RTask;
	Sonicteam::SoX::Engine::Task* task =  _this->DocTask[Sonicteam::DocMarathonImp::DocTaskType::TL_NonStopTask];
	FSNTask* _t = new FSNTask((Sonicteam::SoX::Engine::Task*)task);
	



	return (void*)_this;
}


bool SearchChunkEX(Sonicteam::SoX::FileHandleARC* handlearc,Sonicteam::SoX::ArcHandleMgr* mgr,std::string& search_path,Sonicteam::SoX::ArcHandleSearchResource& search_chunk){



	for (std::map<std::string,Sonicteam::SoX::ArcHandle*>::iterator it = DLL_ARCS.begin();it!= DLL_ARCS.end();it++){

		Sonicteam::SoX::ArcHandle* ahandle =  it->second;
		size_t find_index = -1;
		std::string fpath =  Sonicteam::SoX::FileHandleARC::GetARCChunkPath(mgr,search_path);	
		find_index = Sonicteam::SoX::FileHandleARC::SearchChunkEntryE1(&ahandle->_ArcFile,fpath.c_str());
		if (find_index != -1){
			Sonicteam::SoX::ArcFileChunkF chunkf = ahandle->_ArcFile.ChunkArray[find_index];
			search_chunk.GetFromChunkFHandle(chunkf,ahandle);
			return true;
		}	
	
	};
	return false;	
}








bool SearchExternalFile(std::string& path,std::string& opath) {


	static bool files_fail = false;
	Sonicteam::SoX::ArcHandleMgr* mgr = &Sonicteam::SoX::ArcHandleMgr::getInstance();
	Sonicteam::SoX::FileSystemXenon* xenon = &Sonicteam::SoX::FileSystemXenon::getInstance();
	std::string fpath = Sonicteam::SoX::FileHandleARC::GetARCChunkPath(mgr, path);


	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
		//PushXenonMessage(L"MSG",it->c_str());
		std::string mod_dir = std::string(MOD_DIR) + *it + std::string("\\");
		std::string mod_file = mod_dir +  fpath;
		//PushXenonMessage(L"MSG",mod_file.c_str());
		if ( xenon->FSPathExist(mod_file)){
			//PushXenonMessage(L"MSG",mod_file.c_str());
			std::replace(mod_file.begin(), mod_file.end(), '/', '\\');
			opath = mod_file;
			return true;
		}
	}

	return false;
}


char LoadHandleARC(Sonicteam::SoX::FileHandleARC* t){

	std::string ext_path;
	if (!t->IsLoaded){

		Sonicteam::SoX::ArcHandleMgr* mgr =  &Sonicteam::SoX::ArcHandleMgr::getInstance();
		Sonicteam::SoX::FileSystemXenon* xenon = &Sonicteam::SoX::FileSystemXenon::getInstance();
		Sonicteam::SoX::ArcHandleSearchResource chunkf;


		if (SearchExternalFile(t->filepath,ext_path)){
			BranchTo(0X828B2B18,int,t,&ext_path); // Old File Loading
		}
		else if (SearchChunkEX(t,mgr,t->filepath,chunkf)){
			BranchTo(0x828B2BF8,int,t,&chunkf); // LoadFileFromArc
		}
		else if (t->SearchChunk(mgr,t->filepath,chunkf)){
			BranchTo(0x828B2BF8,int,t,&chunkf); // LoadFileFromArc
		}

		char pro = 0;
		if (t->HBuffer){
			if (t->func0x34){
				pro = 	(char)t->func0x34(t->HBuffer,t->size);
			}
			else{
				pro= 1;
			}
		}
		t->IsProcess = pro;
		t->IsLoaded = 1;
	}

	return t->IsProcess;
}

//AFTER
HOOKV3(0x8238BCB8,HRESULT,ATGXMAFILEOpen,(ATG::XMAFile*,const char*,bool),(_this,strFileName,async),ATG::XMAFile *_this, const char *strFileName, bool async){

	Sonicteam::SoX::FileSystemXenon* xenon = &Sonicteam::SoX::FileSystemXenon::getInstance();

	std::string path = strFileName; //game://xenon/.xma
	path = path.substr(6); //remvoe game://



	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
		std::string mod_dir = std::string(MOD_DIR) + *it + std::string("\\");
		std::string file_find = mod_dir + path;

		if (xenon->FSPathExist(file_find)){
			return _this->Open(file_find.c_str(),async);
		}
	//	PushXenonMessage(L"MSG",file_find.c_str());

	}

	//PushXenonMessage(L"MSG",strFileName);
	



	return _this->Open(strFileName,async);

}


int __fastcall WMVCreateFile(void *_this, const char *szMediaFile, void* access, void *pvIoBuffer, unsigned int ulIoBlockSize, unsigned int ulIoBlockCount, unsigned int ulIoBlockJitter){

	Sonicteam::SoX::FileSystemXenon* xenon = &Sonicteam::SoX::FileSystemXenon::getInstance();


	std::string path = szMediaFile; //game://xenon/.xma
	path = path.substr(6); //remvoe game://


	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
		std::string mod_dir = std::string(MOD_DIR) + *it + std::string("\\");
		std::string file_find = mod_dir + path;

	
		if (xenon->FSPathExist(file_find)){
		//	PushXenonMessage(L"MSG",file_find.c_str());
			return BranchTo(0x827396C8,int,_this,file_find.c_str(),access,pvIoBuffer,ulIoBlockSize,ulIoBlockCount,ulIoBlockJitter);
		}
			

	}
	//PushXenonMessage(L"MSG",strFileName);
	return BranchTo(0x827396C8,int,_this,szMediaFile,access,pvIoBuffer,ulIoBlockSize,ulIoBlockCount,ulIoBlockJitter);


}




bool __fastcall FileSystemIsFileExist(Sonicteam::SoX::FileSystemArc* _this, std::string& pathf){


	Sonicteam::SoX::FileSystemXenon* xenon = &Sonicteam::SoX::FileSystemXenon::getInstance();


	std::string path = pathf; //game://xenon/.xma
	path = path.substr(6); //remvoe game://


	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
		std::string mod_dir = std::string(MOD_DIR) + *it + std::string("\\");
		std::string file_find = mod_dir + path;


		if (xenon->FSPathExist(file_find)){
			//PushXenonMessage(L"MSG",file_find.c_str());
			return true;
		}

	}


	return BranchTo(0x825BE438 ,int,_this,&path);
}




void FileSystemNew::GlobalInstall()
{
	WRITE_DWORD(0x82080424,LoadHandleARC);
	INSTALL_HOOKV3EX(CreateMarathonState,1,false,12);
	INSTALL_HOOKV3EX(ATGXMAFILEOpen,-1,true,12);
	WRITE_DWORD(0x8206313C,WMVCreateFile);
	WRITE_DWORD(0x8204839C,FileSystemIsFileExist);


	// XMA (CreateFile 8260FD24)
}


void FileSystemNew::AddArc(std::string arc_path,unsigned int flag,unsigned int SaveInMemory)
{
	FlagFile data;
	data.flag = flag;
	data.SaveInMemory = SaveInMemory;
	FileSystemNewMap[arc_path] = data;
}


std::map<std::string,FlagFile> FileSystemNew::FileSystemNewMap;
