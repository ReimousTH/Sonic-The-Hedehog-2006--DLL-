#include "FileSystemNew.h"


#include <Sox/RefCountObject.h>
#include <DocMarathonImp.h>

#include <Hook/HookBase.h>
#include <Hook/HookNew.h>


#include <System/Singleton.h>
#include <System/CreateStatic.h>
#include <Sox/FileSystemXenon.h>
#include <Sox/FileHandleARC.h>

#include <string>
#include <sstream>


// Declare the function pointer
REF_TYPE(Sonicteam::SoX::IResource) (*ArcHandle)(std::string, int, int) = (REF_TYPE(Sonicteam::SoX::IResource) (*)(std::string, int, int))0x82583528;

std::map<std::string,Sonicteam::SoX::ArcHandle*> DLL_ARCS;

//AFTER
HOOKV3(0x82160B98,void*,CreateMarathonState,(Sonicteam::DocMarathonImp*,void*),(_this,arg2),Sonicteam::DocMarathonImp* _this, void* arg2){


	HANDLE FileHandle;
	std::stringstream arc_full_path;
	std::stringstream error_msg; 

	Sonicteam::SoX::FileSystemXenon* FileSystem = &SSINGLETON(Sonicteam::SoX::FileSystemXenon)::getInstance();

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
		std::string chunk_path  = mgr->RootPath;
		size_t find_index = -1;

		handlearc->SearchChunkEntry(ahandle,ahandle->_ArcFile.ChunkArray,ahandle->_ArcFile.ChunkArraySize /0x10,0,search_path,chunk_path,find_index);
		if (find_index != -1){
			Sonicteam::SoX::ArcFileChunkF chunkf = ahandle->_ArcFile.ChunkArray[find_index];

	
			if (ahandle->HandleBuffer){
				search_chunk.arc_path = "";
				search_chunk.arc_buffer = ahandle->HandleBuffer;

			}
			else{
				search_chunk.arc_path = ahandle->ArcFilePath.c_str();
				search_chunk.arc_buffer = 0;
			}
			search_chunk.ResourceOffset = chunkf.Offset;
			search_chunk.ResourceCompressedSize = chunkf.CompressedSize;

			if (chunkf.IsCompressed){
				search_chunk.ResourceDecompressedSize = chunkf.DecompressedSize;
			}
			else{
				search_chunk.ResourceDecompressedSize = chunkf.CompressedSize;
			}

			search_chunk.IsCompressed = chunkf.IsCompressed;
			search_chunk.Handle = ahandle;
			search_chunk.IsDownloadContent = ahandle->IsDownloadContent;


			return true;

		}
	};
	return false;	
}


char LoadHandleARC(Sonicteam::SoX::FileHandleARC* t){

	if (!t->IsLoaded){

		Sonicteam::SoX::ArcHandleMgr* mgr =  &Sonicteam::SoX::ArcHandleMgr::getInstance();
		Sonicteam::SoX::ArcHandleSearchResource chunkf;
		if (SearchChunkEX(t,mgr,t->filepath,chunkf)){
			
			BranchTo(0x828B2BF8,int,t,&chunkf); // LoadFileFromArc
			if (t->HBuffer){
				t->IsProcess = 	(char)t->func0x34(t->HBuffer,t->size);
			}
		
			t->IsProcess = 1;
			t->IsLoaded = 1;
	
			//return BranchTo(0x828B30A8,int,t); //run default

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
