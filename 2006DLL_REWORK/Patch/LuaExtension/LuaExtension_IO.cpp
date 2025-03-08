#include "LuaExtension_IO.h"


#include <iostream>
#include <string>
#include <algorithm>

namespace DebugLogV2{


	struct IO_FILE{
		std::string file_path;
		std::string file_access; //write, read
		std::string file_mode; //
		HANDLE handle;
		void* LastReadBuffer;
		DWORD LastReadBufferSize;
		DWORD error;
		std::string error_string;
		unsigned int move;

		IO_FILE(){

		}
	};





	int IO_GlobalInstall(lua_State* L)
	{

		lua_register06(L, "File", IO__NEW);
		luaL_newmetatable06(L, "FileEx");
		lua_pushstring06(L,"__index"); lua_pushvalue(L,-2); lua_settable06(L,-3); // __index = OpenStateExMeta


		lua_pushstring06(L,"ReadFile"); lua_pushcfunction06(L,IO_ReadFile); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pushstring06(L,"WriteFile"); lua_pushcfunction06(L,IO_WriteFile); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pushstring06(L,"CloseFile"); lua_pushcfunction06(L,IO_CloseFile); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pushstring06(L,"GetErrorCode"); lua_pushcfunction06(L,IO_GetErrorCode); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pushstring06(L,"GetSize"); lua_pushcfunction06(L,IO_GetErrorCode); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pushstring06(L,"Move"); lua_pushcfunction06(L,IO_Move); lua_settable06(L,-3); // __index = MemoryMeta





		lua_pushstring06(L,"__gc"); lua_pushcfunction06(L,IO__GC); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pop(L,1);



		lua_register06(L,"free",IO__FREE);
		lua_register06(L,"malloc",IO__MALLOC);

		return 0;


	}

	unsigned int IO_GetNumber(lua_State* L, int arg) {
		unsigned int return_value = 0;

		if (lua_isnumber(L, arg)) {
			return_value = (unsigned int)lua_tonumber(L, arg); // C
		} else if (lua_isuserdata(L, arg)) {
			return_value = (unsigned int)(size_t)lua_touserdata(L, arg); // Cast to unsigned int
		} else if (lua_isstring(L, arg)) {
			const char* str = lua_tostring(L, arg);
			char* endptr;
			return_value = strtoul(str, &endptr, 16); // Base 16 for hex
			if (*endptr != '\0' && *endptr != ' ') { 
				luaL_error(L, "Invalid hex string: '%s'", str);
				return 0; 
			}
			if (errno == ERANGE)
			{
				luaL_error(L,"Hex string '%s' is out of unsigned integer range.",str);
				return return_value; //Will not reach here
			}
		} else {
			// Handle invalid argument type
			luaL_error(L, "Argument %d must be a number, userdata, or hex string.", arg);
			return return_value;
		}

		return return_value;
	}
	extern "C" int IO__FREE(lua_State* L){
		free(lua_touserdata06(L,1));
		return 1;
	}
	extern "C" int IO__MALLOC(lua_State* L){
		lua_pushlightuserdata(L,malloc(IO_GetNumber(L,1)));

		return 1;
	}

	extern "C" int IO__CreateMetatable(lua_State* L) {
		// Create a new userdata for the IO_FILE structure
		IO_FILE* file = (IO_FILE*)lua_newuserdata(L, sizeof(IO_FILE));
		memset(file,0,sizeof(IO_FILE));

		// Set the metatable for the userdata
		luaL_getmetatable(L, "FileEx");
		lua_setmetatable(L, -2);

		// Get parameters from Lua and store in the IO_FILE structure
		file->file_path = lua_tostring(L, 1);
		file->file_access = lua_tostring(L, 2);
		file->file_mode = lua_tostring(L, 3);
		file->handle = 0;

		DWORD desiredAccess = 0;
		DWORD shareMode = 0;
		DWORD creationDisposition = 0;

		// Determine access mode based on input strings
		if (file->file_access == "r") {
			desiredAccess = GENERIC_READ;
			shareMode = FILE_SHARE_READ;
			creationDisposition = OPEN_EXISTING;
		} else if (file->file_access == "w") {
			desiredAccess = GENERIC_WRITE;
			shareMode = 0; // No sharing for writing
			creationDisposition = CREATE_ALWAYS; // Overwrite existing file
		} else if (file->file_access == "a") {
			desiredAccess = GENERIC_WRITE;
			shareMode = 0;
			creationDisposition = OPEN_ALWAYS; // Create if not exists, open if exists
		} else {

			file->error_string = "Invalid file access mode.";
			return 1;
		}


		// Extract the filename
		std::string filename;
		std::wstringstream wfilename;
		size_t lastBackslash = file->file_path.find_last_of('\\');
		if (lastBackslash != std::string::npos) {
			filename = file->file_path.substr(lastBackslash + 1);
		} else {
			filename = file->file_path; // If no backslash, the whole path is the filename
		}
		wfilename << filename.c_str();




		DWORD dwRet;
		XOVERLAPPED     m_Overlapped;    
		ZeroMemory( &m_Overlapped, sizeof( XOVERLAPPED ) );
		DWORD m_DeviceID = XCONTENTDEVICE_ANY;
		ULARGE_INTEGER iBytesRequested = {0};
		unsigned int SignUser =0;


		dwRet = XShowDeviceSelectorUI( SignUser, // User to receive input from
			XCONTENTTYPE_SAVEDGAME,   // List only save game devices
			0,                       // No special flags
			iBytesRequested,         // Size of the device data struct
			&m_DeviceID,            // Return selected device information
			&m_Overlapped );

		while( !XHasOverlappedIoCompleted( &m_Overlapped ));


		XCONTENT_DATA contentData = {0};
		strcpy_s( contentData.szFileName, "DLL" );
		wcscpy_s( contentData.szDisplayName, L"DLL");
		contentData.dwContentType = XCONTENTTYPE_SAVEDGAME;
		contentData.DeviceID = m_DeviceID;

		XContentCreate(SignUser,"save",&contentData,XCONTENTFLAG_OPENALWAYS,NULL,NULL,0);

		// Create the file using Windows API
		file->handle = CreateFile(
			file->file_path.c_str(), // Convert std::string to C-style string
			desiredAccess,
			shareMode,
			NULL, // Security attributes
			creationDisposition,
			FILE_ATTRIBUTE_NORMAL,
			NULL // Template file
			);

		// Check for errors in creating the file
		if (file->handle == INVALID_HANDLE_VALUE) {
			DWORD error = GetLastError();
			file->error = error;
	
			
		}
	



		return 1; // Return userdata to Lua stack
	}




	
	
	extern "C" int IO_ReadFile(lua_State* L){

		IO_FILE* file = 	(IO_FILE*)lua_touserdata06(L,1);
		size_t byte_to_read = IO_GetNumber(L,2);

		HANDLE handle = file->handle;
		void* buffer = malloc(byte_to_read);
		
		ReadFile(handle,buffer,byte_to_read,&file->LastReadBufferSize,0);
		lua_pushlightuserdata(L,buffer);

		return 1;
	}
	
	
	
	extern "C" int IO_WriteFile(lua_State* L)
	{
		IO_FILE* file = 	(IO_FILE*)lua_touserdata06(L,1);
		HANDLE handle = file->handle;
		void* buffer = lua_touserdata06(L,2);
		size_t buffer_size = (size_t)IO_GetNumber(L,3);
		WriteFile(handle,buffer,buffer_size,&file->LastReadBufferSize,0);
		return 0;
	}

	extern "C" int IO_GetSize(lua_State* L)
	{
		IO_FILE* file = 	(IO_FILE*)lua_touserdata06(L,1);
		LARGE_INTEGER size;
		GetFileSizeEx(file->handle,&size);
		lua_pushlightuserdata(L,(void*)size.LowPart);
		return 1;
	}

	extern "C" int IO_Move(lua_State* L)
	{
		IO_FILE* file = 	(IO_FILE*)lua_touserdata06(L,1);
		file->move += (int)IO_GetNumber(L,2);
		LARGE_INTEGER buffer;
		LARGE_INTEGER moveto;
		moveto.LowPart = file->move;

		BOOL bResult = SetFilePointerEx(
			file->handle,
			moveto,
			&buffer,
			FILE_BEGIN  
			);
		return 0;
	}
	extern "C" int MoveAt(lua_State* L)
	{
	
		IO_FILE* file = 	(IO_FILE*)lua_touserdata06(L,1);
		file->move = (int)IO_GetNumber(L,2);
		LARGE_INTEGER buffer;
		LARGE_INTEGER moveto;
		moveto.LowPart = file->move;

		BOOL bResult = SetFilePointerEx(
			file->handle,
			moveto,
			&buffer,
			FILE_BEGIN  
			);
		return 0;
	}

	extern "C" int IO_CloseFile(lua_State* L)
	{
		IO_FILE* file = 	(IO_FILE*)lua_touserdata06(L,1);
		CloseHandle(file->handle);
		return 0;
	}

	extern "C" int IO_GetErrorCode(lua_State* L)
	{
		IO_FILE* file = 	(IO_FILE*)lua_touserdata06(L,1);
		lua_pushlightuserdata(L,(void*)file->error);
		return 1;
	}

	extern "C" int IO__NEW(lua_State* L)
	{
		IO__CreateMetatable(L);
		return 1;
	}
	extern "C" int IO__GC(lua_State* L){
		IO_FILE* file = 	(IO_FILE*)lua_touserdata06(L,1);
		lua_pushlightuserdata(L,(void*)file->error);
		CloseHandle(file->handle);
		file->file_path.clear();
		file->file_mode.clear();
		file->file_access.clear();
		XContentClose("save",0);
		return 0;
	}


	



}