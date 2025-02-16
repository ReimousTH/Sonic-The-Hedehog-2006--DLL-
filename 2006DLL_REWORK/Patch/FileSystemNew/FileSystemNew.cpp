#include "FileSystemNew.h"


#include <Sox/RefCountObject.h>
#include <DocMarathonImp.h>

#include <Hook/HookBase.h>
#include <Hook/HookNew.h>


#include <System/Singleton.h>
#include <System/CreateStatic.h>
#include <Sox/ExFileSystem.h>

#include <string>
#include <sstream>


// Declare the function pointer
REF_TYPE(Sonicteam::SoX::IResource) (*ArcHandle)(std::string, int, int) = (REF_TYPE(Sonicteam::SoX::IResource) (*)(std::string, int, int))0x82583528;

//AFTER
HOOKV3(0x82160B98,void*,CreateMarathonState,(Sonicteam::DocMarathonImp*,void*),(_this,arg2),Sonicteam::DocMarathonImp* _this, void* arg2){


	HANDLE FileHandle;
	std::stringstream arc_full_path;
	std::stringstream error_msg; 

	Sonicteam::SoX::ExFileSystem* FileSystem = &SSINGLETON(Sonicteam::SoX::ExFileSystem)::getInstance();


	
	for (std::map<std::string,FlagFile>::iterator it = FileSystemNew::FileSystemNewMap.begin();it!= FileSystemNew::FileSystemNewMap.end();it++){


		 std::string arc_full_path = FileSystem->FileSystemGetFullPath(it->first,it->second.flag);

		 if (FileSystem->FileSystemIsFileExist(arc_full_path)){
			// PushXenonMessage(L"MSG",arc_full_path.c_str());
			 _this->ArcVector1.push_back(ArcHandle(it->first,it->second.flag,it->second.SaveInMemory)); //-1 IF NOT 1 resource is being loaded but ??? something else happend and it destroy object depsite i can find csdobject in memory

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


void FileSystemNew::GlobalInstall()
{
	INSTALL_HOOKV3EX(CreateMarathonState,-1,false);
}


void FileSystemNew::AddArc(std::string arc_path,unsigned int flag,unsigned int SaveInMemory)
{
	FlagFile data;
	data.flag = flag;
	data.SaveInMemory = SaveInMemory;
	FileSystemNewMap[arc_path] = data;
}


std::map<std::string,FlagFile> FileSystemNew::FileSystemNewMap;
