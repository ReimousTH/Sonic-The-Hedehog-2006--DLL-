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

#include <d3d9.h>
#include <d3dx9.h>


#include <xtl.h>
#include <xboxmath.h>
#include <AtgApp.h>
#include <AtgFont.h>
#include <AtgHelp.h>
#include <AtgInput.h>
#include <AtgResource.h>
#include <AtgInput.h>
#include <AtgUtil.h>
#include <xaudio.h>
#include <xmedia.h>
#include <AtgSimpleShaders.h>




// Declare the function pointer
REF_TYPE(Sonicteam::SoX::IResource) (*ArcHandle)(std::string, int, int) = (REF_TYPE(Sonicteam::SoX::IResource) (*)(std::string, int, int))0x82583528;
std::map<std::string,Sonicteam::SoX::ArcHandle*> DLL_ARCS;	
static std::vector<std::string> files; //cache_later


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






class ModSelectAPP:public ATG::Application{


public:
		static ATG::Font m_font;

		~ModSelectAPP(){
			_texture.clear();
		}

private:

	IDirect3DSurface9* pBackBuffer;
	IDirect3DSurface9* pFrontBuffer;
	IDirect3DSurface9* pDepthStencil;


	std::map<std::string,void*> _texture;
	std::map<std::string,size_t> _texture_size;
	

	void LoadTexture(std::string path){





		DWORD _size_read;

		HANDLE _file =  CreateFile(path.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);

		LARGE_INTEGER _size;
		GetFileSizeEx(_file,&_size);
		void* buffer = malloc(_size.LowPart);
		ReadFile(_file,buffer,_size.LowPart,&_size_read,0);

		
		size_t lindex = path.find_last_of("\\");
		path = path.substr(lindex + 1);


		_texture[path] = buffer; //load_texture
		_texture_size[path] = _size.LowPart; //load_texture



		

		CloseHandle(_file);

		
	}





	std::vector<std::string> _directory;
	std::vector<std::string> _directory_from_save;
	int index;
	int index_max;
	size_t pos_y;
	std::vector<bool> _selected;
	const char* modb_db_file;
	HANDLE MOD_DB;
	DWORD m_DeviceID;

	float _down;
	float _up;


public:


	void ProcessDeviceUI(){

		DWORD dwRet;
		XOVERLAPPED     m_Overlapped;    
		ZeroMemory( &m_Overlapped, sizeof( XOVERLAPPED ) );
		m_DeviceID = XCONTENTDEVICE_ANY;
		ULARGE_INTEGER iBytesRequested = {0};
		unsigned int SignUser =0;


		dwRet = XShowDeviceSelectorUI( SignUser, // User to receive input from
			XCONTENTTYPE_SAVEDGAME,   // List only save game devices
			0,                       // No special flags
			iBytesRequested,         // Size of the device data struct
			&m_DeviceID,            // Return selected device information
			&m_Overlapped );

		while( !XHasOverlappedIoCompleted( &m_Overlapped ));

	}
	void OpenContent(){

		XCONTENT_DATA contentData  = {0};
		strcpy_s( contentData.szFileName, "DLL" );
		wcscpy_s( contentData.szDisplayName, L"DLL");
		contentData.dwContentType = XCONTENTTYPE_SAVEDGAME;
		contentData.DeviceID = m_DeviceID;
		unsigned int SignUser =0;
		XContentCreate(SignUser,"save",&contentData,XCONTENTFLAG_OPENALWAYS,NULL,NULL,0);
	}
	void CloseContent(){
		XContentClose("save",0);
	}




	void splitByNewline(std::vector<std::string>& out,const std::string& _out_str) {
		std::vector<std::string> lines;
		std::stringstream ss(_out_str);
		std::string line;

		while (std::getline(ss, line)) {
			out.push_back(line);
		}

	}


	void LoadSaveFile(){

		ProcessDeviceUI();
		OpenContent();

		// Create the file using Windows API
		MOD_DB = CreateFile(
			modb_db_file,
			GENERIC_READ |  GENERIC_WRITE,
			0,
			NULL, // Security attributes
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL // Template file
			);


	
		if (MOD_DB != INVALID_HANDLE_VALUE){
	
			LARGE_INTEGER size;
			DWORD _out_read;
			GetFileSizeEx(MOD_DB,&size);
			const char* _buffer = (const char*)malloc(size.LowPart + 1);
			std::string _out_str;
			memset((void*)_buffer,0,size.LowPart+1);
			if (ReadFile(MOD_DB,(void*)_buffer,size.LowPart,&_out_read,0)){
				_out_str = std::string(_buffer);

			}
			splitByNewline(_directory_from_save,_out_str);
			delete _buffer;

		}
		CloseHandle(MOD_DB);
		CloseContent();

	}
	


	void InitializeDevice(D3DDevice* device){
		ATG::Application::m_pd3dDevice = (ATG::D3DDevice*)device;
		ATG::g_pd3dDevice= 	ATG::Application::m_pd3dDevice;
	}
	void InitializeFont(){
		if( FAILED( m_font.Create( "game:\\Media\\Fonts\\FOT-NewRodin ProN EB_16.xpr" ) ) ){
			PushXenonMessage(L"MSG","ATGAPPERR_MEDIANOTFOUND");
		}

		m_font.SetWindow( ATG::GetTitleSafeArea() );
	}
	void InitializeBaseShader(){
		ATG::SimpleShaders::Initialize( NULL, NULL );
	}

	void DestroyFont(){
		m_font.Destroy();
	}
	void DestroyBaseShader(){
		ATG::SimpleShaders::Terminate();
	}



	VOID Run()
	{
		HRESULT hr;
		_down = 0.0;
		_up = 0.0;

		size_t ApplicationXenon = *(size_t*)0x82D3B348;
		InitializeDevice(*(D3DDevice**)(ApplicationXenon + 0x24));


	//	LoadTexture("game:\\media\\tex01.dds"); 








		Initialize();



		// Run the game loop
		for (;;) {
			// Update the scene
			if (Update() != S_OK) break;

			// Render the scene
			Render();
		}



		DestroyFont();
		DestroyBaseShader();



	
	}


	virtual HRESULT Initialize()
	{

		//FIle_System

		modb_db_file = "save:\\modb_db.ini";

		InitializeBaseShader();
	


		pos_y = 0;
		DirectoryGetDirs(_directory,std::string(MOD_DIR));
		index = 0;
		

		InitializeFont();

		index_max = _directory.size();
		_selected = std::vector<bool>(index_max);
		return S_OK;
	}



	void selindex(int to){
		index+=to;
		if (index < 0){
			index = (index_max - 1) - (index + 1);
		}
		else{
			index = index % index_max;
		}
	}

	virtual HRESULT Update()
	{
		double delta = 1.0/60.0; // later 
		

		ATG::GAMEPAD* gc =  ATG::Input::GetMergedInput();


		if ((gc->wLastButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0){
			if (_down == 0.0) selindex(1);
			_down += delta;
			
		}
		else{
			_down = 0;
		}

		if (_down > 0.167){
			selindex(1);
			_down = 0.001;
		}


		if ((gc->wLastButtons & XINPUT_GAMEPAD_DPAD_UP) != 0){
			if (_up == 0.0) selindex(-1);
			_up += delta;

		}
		else{
			_up = 0;
		}

		if (_up > 0.167){
			selindex(-1);
			_up = 0.001;
		}





		if (gc->wPressedButtons & XINPUT_GAMEPAD_A){

			_selected[index] = !_selected[index];
		}

		if (gc->wPressedButtons & XINPUT_GAMEPAD_START){


			for (int i = 0;i<_directory.size();i++){
				if (_selected[i]) files.push_back(_directory[i]);
			
			}
			return S_FALSE;
		}
	
	
		

		return S_OK;
	}



	virtual HRESULT Render()
	{
		HRESULT hr;
		ATG::RenderBackground( 0xff0000ff, 0xff000000 );

		m_font.Begin();
		m_font.SetScaleFactors( 1.0f, 1.0f );
		m_font.DrawText( 0, 0, 0xffffff00,L"Mod Manager(Test)", ATGFONT_RIGHT );

		DWORD SEL_COLOR =          0xffff0000;         // selection color
		DWORD UNSEL_COLOR =        0xffffffff;         // unselection color
		DWORD ACT_COLOR =       D3DCOLOR_RGBA(0,0,255,255);         //
		DWORD Between = 16  + 4;

		DWORD w = 1270;
		DWORD h = 720;

		int scount = 0;
		for (int i = 0;i<_directory.size();i++){
			std::string _cur = _directory[i];
			std::wstring wideString(_cur.begin(), _cur.end());
			DWORD cur_color = UNSEL_COLOR;
			// Calculate vertical center position
			DWORD centerY = (h / 2);  // 240 in your case


			// Position items relative to center
			DWORD cur_y = (centerY + (i - index) * Between)  - (h/4); //offset

			if (index == i) {
				cur_color = _selected[i] ? SEL_COLOR | ACT_COLOR  : SEL_COLOR;
			}

			else{
				cur_color = _selected[i] ?  ACT_COLOR  : UNSEL_COLOR;
			}

			if (_selected[i]) scount++;


			m_font.DrawText(0, cur_y, cur_color, wideString.c_str());
		}


		wchar_t buffer[0x256];
		ZeroMemory(buffer, sizeof(buffer));
		swprintf_s(buffer, _countof(buffer), L"%d/%d/%d", index+1, index_max,scount);
		m_font.DrawText(0, 550, 0xffffff00, buffer, ATGFONT_CENTER_X);

		m_font.End();


		//try draw_texutre

		
	/*
		IDirect3DTexture9* pTexture = NULL;
		D3DXCreateTextureFromFileInMemory(m_pd3dDevice,_texture["tex01.dds"],_texture_size["tex01.dds"],&pTexture);
		ID3DXSprite* pSprite = NULL;
		D3DXCreateSprite(m_pd3dDevice, &pSprite);
		D3DXVECTOR3 position(100.0f, 100.0f, 0.0f); 
		pSprite->Begin(D3DXSPRITE_ALPHABLEND);
		pSprite->Draw(pTexture, NULL, NULL, &position, 0xffffffff);
		pSprite->End();

		pSprite->Release();
		pTexture->Release();



		*/

		m_pd3dDevice->Present(NULL, NULL, NULL, NULL);



		return S_OK;
	}

};
ATG::Font ModSelectAPP::m_font;


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


		if (!CMInstance){
			CMInstanceSET(*(size_t*)0x82D3B6E8);
		}

		size_t ApplicationXenon = *(size_t*)0x82D3B348;
		IDirect3DDevice9*  Device = *(IDirect3DDevice9**)(ApplicationXenon + 0x24);
		if (Device){

			/*
			D3DVIEWPORT9 view;
			Device->GetViewport(&view);

			ATG::RenderBackground( 0xff0000ff, 0xff000000 );
			ModSelectAPP::m_font.Begin();


			ModSelectAPP::m_font.DrawText(0,0,D3DCOLOR_RGBA(255,0,0,255),L"?????????",ATGFONT_RIGHT);


			ModSelectAPP::m_font.End();
*/
			


		}

		

	
		


		ATG::GAMEPAD* input = ATG::Input::GetMergedInput(0);


		Sonicteam::DocMarathonImp* impl = 	*(Sonicteam::DocMarathonImp**)(*(UINT32*)0x82D3B348 + 0x180);
		Sonicteam::GameImp* gameimp = *(Sonicteam::GameImp** )(impl->DocCurrentMode + 0x6C);
		if (gameimp){



			if ((input->wLastButtons & XINPUT_GAMEPAD_Y) != 0){

			
				//_app.Run();
			//	PushXenonMessage(L"MSG","YES");
			//	size_t task =  (size_t)(gameimp->GamePlayerDisplayTask[0].ThisObject);
			//	size_t hud_main = *(size_t*)(task + 0x4C);
			//	Sonicteam::CsdObject* csd = *(Sonicteam::CsdObject**)(hud_main + 0x54);
			//	Chao::CSD::CProject* cproj =  csd->FCProject;
			//	const char* name =  cproj->CProjectScene.begin()->first;
			//	Chao::CSD::CScene* scene  =  (cproj->CProjectScene.begin()->second.get()->get());
			//	size_t index =  scene->CObjectRaw->GetMotionIndexFromName("DefaultAnim");

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




















//AFTER


HOOKV3(0x82160B98,void*,CreateMarathonState,(Sonicteam::DocMarathonImp*,void*),(_this,arg2),Sonicteam::DocMarathonImp* _this, void* arg2){


	ModSelectAPP app;
	app.Run();








	Sonicteam::SoX::ArcHandleMgr* mgr = &Sonicteam::SoX::ArcHandleMgr::getInstance();
	Sonicteam::SoX::FileSystemXenon* xenon = &Sonicteam::SoX::FileSystemXenon::getInstance();
	Sonicteam::SoX::FileSystemXenon* FileSystem = &SSINGLETON(Sonicteam::SoX::FileSystemXenon)::getInstance();
	Sonicteam::SoX::FileSystemArc* ArcSystem = &Sonicteam::SoX::FileSystemArc::getInstance();
	//ArcSystem->FileSystemFolder.push_back(std::string("game:\\"));
	xenon->FileSystemFolder.push_back(std::string("game:\\"));
	

	//Load mod arcs
	 {
		//DirectoryGetDirs(files,std::string(MOD_DIR));

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
	Sonicteam::SoX::Engine::Task* task =  _this->DocTask[Sonicteam::DocMarathonImp::DocTaskType::TL_PrepareRender];
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


bool __fastcall FileSystemDirExist(Sonicteam::SoX::FileSystemArc* _this,std::vector<std::string>& files_dir, std::string& pathf,std::string& extf){


	Sonicteam::SoX::FileSystemXenon* xenon = &Sonicteam::SoX::FileSystemXenon::getInstance();
	std::string path = pathf; //game://xenon/.xma
	path = path.substr(6); //remvoe game://


	bool result = 0;

	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
		std::string mod_dir = std::string(MOD_DIR) + *it + std::string("\\");
		std::string file_find = mod_dir + path;

		if (xenon->FSDirectoryGetFiles(files_dir,pathf,extf)){
			result |= true;
		}

	}

	bool result_2 =  BranchTo(0x825BE3F0 ,bool,_this,&files_dir,&pathf,&extf);


	result |= result_2;
	return result;
	
}




void FileSystemNew::GlobalInstall()
{

	//Use Auto Buffer because i cant create mine :C
	WRITE_DWORD(0x825B1C90,0x60000000);
	WRITE_DWORD(0x825B1C9C,0x60000000);

	WRITE_DWORD(0x82080424,LoadHandleARC);
	INSTALL_HOOKV3EX(CreateMarathonState,1,false,12);
	INSTALL_HOOKV3EX(ATGXMAFILEOpen,-1,true,12);
	WRITE_DWORD(0x8206313C,WMVCreateFile);
	WRITE_DWORD(0x8204839C,FileSystemIsFileExist);
	WRITE_DWORD(0x82048398,FileSystemDirExist);


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
