#pragma once

#include <string>
#include <map>
#include <Core/Debug.h>
#include <Sox/RefCountObject.h>
#include <Sox/IResource2.h>
#include <Sox/IResourceMgr.h>


struct FlagFile{
	unsigned int flag;
	unsigned int SaveInMemory;
};





class FileSystemNew{
public:
	static float lerp(float a, float b, float t);
	




	static std::map<std::string,FlagFile> FileSystemNewMap;

	static void AddArc(std::string arc_path,unsigned int flag,unsigned int SaveInMemory);

	static void GlobalInstall();
};
