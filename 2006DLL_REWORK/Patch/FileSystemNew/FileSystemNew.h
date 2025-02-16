#pragma once

#include <string>
#include <map>


struct FlagFile{
	unsigned int flag;
	unsigned int SaveInMemory;
};

class FileSystemNew{

public:
	static std::map<std::string,FlagFile> FileSystemNewMap;

	static void AddArc(std::string arc_path,unsigned int flag,unsigned int SaveInMemory);

	static void GlobalInstall();
};
