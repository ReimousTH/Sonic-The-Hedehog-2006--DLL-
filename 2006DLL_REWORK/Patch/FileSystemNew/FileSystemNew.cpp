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


#include <string>
#include <vector>

#define MOD_DIR "game:\\mods\\"
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

//AFTER
HOOKV3(0x82160B98,void*,CreateMarathonState,(Sonicteam::DocMarathonImp*,void*),(_this,arg2),Sonicteam::DocMarathonImp* _this, void* arg2){

	Sonicteam::SoX::ArcHandleMgr* mgr = &Sonicteam::SoX::ArcHandleMgr::getInstance();
	Sonicteam::SoX::FileSystemXenon* xenon = &Sonicteam::SoX::FileSystemXenon::getInstance();
	Sonicteam::SoX::FileSystemXenon* FileSystem = &SSINGLETON(Sonicteam::SoX::FileSystemXenon)::getInstance();
	Sonicteam::SoX::FileSystemArc* ArcSystem = &Sonicteam::SoX::FileSystemArc::getInstance();
	//ArcSystem->FileSystemFolder.push_back(std::string("game:\\"));
	xenon->FileSystemFolder.push_back(std::string("game:\\"));
	static std::vector<std::string> files; //cache_later

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
				DLL_ARCS[arc_file] = (Sonicteam::SoX::ArcHandle*)(_this->ArcVector1.rbegin())->get();
			}	

			for (std::vector<std::string>::iterator jt = dir_arcs_win32.begin(); jt != dir_arcs_win32.end(); ++jt) {
				std::string arc_file =  mod_dir_win32 + *jt;
				arc_file = arc_file.substr(6);
				_this->ArcVector1.push_back(ArcHandle(arc_file,3,0));
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
	static std::vector<std::string> files; //cache_later
	static bool files_fail = false;
	Sonicteam::SoX::ArcHandleMgr* mgr = &Sonicteam::SoX::ArcHandleMgr::getInstance();
	Sonicteam::SoX::FileSystemXenon* xenon = &Sonicteam::SoX::FileSystemXenon::getInstance();
	std::string fpath = Sonicteam::SoX::FileHandleARC::GetARCChunkPath(mgr, path);

	if (files_fail) return false;
	if (files.size() == 0)  files_fail = !DirectoryGetDirs(files,std::string(MOD_DIR));


	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
		//PushXenonMessage(L"MSG",it->c_str());
		std::string mod_dir = std::string(MOD_DIR) + *it + std::string("\\");
		std::string mod_file = mod_dir +  fpath;
		if ( xenon->FSPathExist(mod_file)){
		//	PushXenonMessage(L"MSG",mod_file.c_str());
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
		if (SearchChunkEX(t,mgr,t->filepath,chunkf)){
			
			BranchTo(0x828B2BF8,int,t,&chunkf); // LoadFileFromArc
			if (t->func0x34){
				t->IsProcess = 	(char)t->func0x34(t->HBuffer,t->size);
			}
			else{
				t->IsProcess = 1;
			}
		
			t->IsLoaded = 1;
			return t->IsProcess;
		}
		else if (SearchExternalFile(t->filepath,ext_path)){
			BranchTo(0X828B2B18,int,t,&ext_path); // Old File Loading
			if (t->func0x34){
				t->IsProcess = 	(char)t->func0x34(t->HBuffer,t->size);
			}
			else{
				t->IsProcess = 1;
			}
	
			t->IsLoaded = 1;
			return t->IsProcess;

		}
		else{
			return BranchTo(0x828B30A8,int,t); //run default
		}
	}

	return t->IsProcess;
}


void FileSystemNew::GlobalInstall()
{
	WRITE_DWORD(0x82080424,LoadHandleARC);
	INSTALL_HOOKV3EX(CreateMarathonState,-1,false,12);
}


void FileSystemNew::AddArc(std::string arc_path,unsigned int flag,unsigned int SaveInMemory)
{
	FlagFile data;
	data.flag = flag;
	data.SaveInMemory = SaveInMemory;
	FileSystemNewMap[arc_path] = data;
}


std::map<std::string,FlagFile> FileSystemNew::FileSystemNewMap;
