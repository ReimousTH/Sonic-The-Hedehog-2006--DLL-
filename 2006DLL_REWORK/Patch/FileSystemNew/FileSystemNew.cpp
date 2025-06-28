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
#include <Sox/ApplicationXenon.h>



//EXTRA

static LARGE_INTEGER APP_START_FRAME;
static LARGE_INTEGER FIRST_SCALE;
//static LARGE_INTEGER LAST_SCALE;

static float _mod_selector = 0.0;


const int lineHeight = 20;
const int viewportHeight = 720;
const int viewportTop = 0;
const int viewportBottom = 720;
const int screenWidth = 1280;
const int screenHeight = 720;




// Declare the function pointer
REF_TYPE(Sonicteam::SoX::IResource) (*ArcHandle)(std::string, int, int) = (REF_TYPE(Sonicteam::SoX::IResource) (*)(std::string, int, int))0x82583528;


volatile void (*sub_825B0988)(void*) = (volatile void (*)(void*))0x825B0988;
volatile void (*sub_825B0AC0)(void*) = (volatile void (*)(void*))0x825B0AC0;



std::map<std::string,Sonicteam::SoX::ArcHandle*> DLL_ARCS;	
std::map<std::string,Sonicteam::SoX::ArcHandle*> DLL_ARCS_NATIVE;	
static std::vector<std::string> DLL_ARCS_FOLDER; //cache_later
static std::vector<std::string> files; //cache_later

Sonicteam::DocMarathonImp* DOC;




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




class IScene{


	size_t RefCount;
	
public:
	virtual bool Release(){
		if (--RefCount == 0){
			delete this;
		}
		return true;
	}
	virtual bool AddRef(){
		RefCount++;
		return true;
	}

	virtual bool Update() = 0;
	virtual bool Render() = 0;
	virtual bool OnEnd() = 0;
	virtual bool OnStart() = 0;
};




class ModSelectAPP:public ATG::Application{


public:
	static ATG::Font m_font;
	static ATG::Font m_font_2;
	static ATG::Font m_font_glyph;
	IScene* _scene;

private:


	const char* modb_db_file;
	HANDLE MOD_DB;
	DWORD m_DeviceID;
	bool _obj_init;
	bool _start_;


public:






	

	void InitializeDevice(D3DDevice* device){
		ATG::Application::m_pd3dDevice = (ATG::D3DDevice*)device;
		ATG::g_pd3dDevice= 	ATG::Application::m_pd3dDevice;
	}


	void RenderOBJInit(){

	//	ATG::SimpleShaders::Initialize( NULL, NULL );
		m_font.Create( "game:\\Media\\Fonts\\FOT-NewRodin ProN EB_16.xpr" );
		m_font_2.Create( "game:\\Media\\Fonts\\FOT-NewRodin Pro M_8.xpr" );
		m_font_glyph.Create( "game:\\Media\\Fonts\\Arial_16.xpr" );
		m_font.SetWindow(	ATG::GetTitleSafeArea());
		_obj_init = true;
	}
	void Loop(){

		size_t ApplicationXenon = *(size_t*)0x82D3B348;
		InitializeDevice(*(D3DDevice**)(ApplicationXenon + 0x24));;
		if (!_obj_init) RenderOBJInit();

		_scene->OnStart();
		_start_ = true;
		for (;;) {

			if (!_scene->Update()) break;
			if (!_scene->Render()) break;
		}
		_start_ = false;
		_scene->OnEnd();
		_scene->Release();
		_scene = 0;

	}
	void RenderOBJTerminate(){
		ATG::SimpleShaders::Terminate();
		m_font.Destroy();
	};



	void SetScene(IScene* _scene){
		if (this->_scene){ 
			this->_scene->OnEnd();
			this->_scene->Release();
		}

		if (_start_){
			_scene->OnStart();
		}
		_scene->AddRef();
		this->_scene = _scene;
	}




	VOID Run()
	{
		Loop();
	}


	virtual HRESULT Initialize()
	{


		return S_OK;
	}


	virtual HRESULT Update()
	{
		return S_OK;
	}



	virtual HRESULT Render()
	{
return S_OK;
	}

};



class D3DShader{

	std::string shader_name;
	LPD3DXEFFECT g_pEffect; //relee


	~D3DShader(){
		if (g_pEffect) g_pEffect->Release(); //no use
	}

	void LoadFromFile()
	{

		LPD3DXBUFFER pErrorBuffer = NULL;
			HRESULT hr = D3DXCreateEffectFromFile(
			ATG::g_pd3dDevice,
			shader_name.c_str(),
			NULL,        // No macros
			NULL,        // No include handler
			0,              // Flags
			NULL,        // Effect pool
			&g_pEffect,
			&pErrorBuffer
			);

		if (FAILED(hr))
		{
			if (pErrorBuffer)
			{
				OutputDebugStringA((char*)pErrorBuffer->GetBufferPointer());
				pErrorBuffer->Release();
			}
			return;
		}
	}
};



ModSelectAPP app;

ATG::Font ModSelectAPP::m_font;
ATG::Font ModSelectAPP::m_font_2;
ATG::Font ModSelectAPP::m_font_glyph;




class Button{
	

	std::string _Name;
	DWORD FX;
	DWORD FY;
	DWORD _color;
	bool Toggle;
	bool Focus;

	 bool* ToggleEx;


	public:

	Button(){
		ToggleEx = 0;
	}

	Button(const char* Name,DWORD FX,DWORD FY,DWORD _color){
		SetName(Name);
		SetPos(FX,FY);
		SetColor(_color);
		ToggleEx = 0;
	}


	Button(const char* Name,DWORD FX,DWORD FY,DWORD _color, bool* Toggle){
		SetName(Name);
		SetPos(FX,FY);
		SetColor(_color);
		ToggleEx = Toggle;
	}



	

	virtual void SetPos(DWORD FX, DWORD FY){
		this->FX = FX;
		this->FY = FY;
	}
	virtual void SetColor(DWORD _color){
		this->_color = _color;
	}
	virtual void SetColor(byte A,byte R,byte G,byte B){
		this->_color = D3DCOLOR_ARGB(A,R,G,B);
	}

	virtual void SetName(std::string& Name){
		this->_Name = Name;
	}
	virtual void SetName(const char* cstr){
		this->_Name = std::string(cstr);
	}


	virtual void Update(bool ToggleCondition){
		if (ToggleEx == 0){
			if (Focus && ToggleCondition){
				Toggle = !Toggle;
			}
		}
		else{
			if (Focus && ToggleCondition){
				*ToggleEx = !*ToggleEx;
			}
			Toggle = *ToggleEx;
		}
	
		
	}



	virtual void FocusON(){
		Focus = true;
	}
	virtual void FocusOFF(){
		Focus = false;
	}

	virtual void Render(){
		
		std::wstring msg_raw(_Name.begin(), _Name.end()); //Convert Message



		DWORD _cur_color = _color;

		if (Focus){
			_cur_color = D3DCOLOR_ARGB(255,0,0,255);
		}
		if (Toggle){
			_cur_color = D3DCOLOR_ARGB(255,0,255,0);
		}


		if (Focus){
			msg_raw = L">>" + msg_raw;
		}


		ModSelectAPP::m_font.Begin();
		ModSelectAPP::m_font.DrawText(FX,FY,_cur_color,msg_raw.c_str(),0,0);
		ModSelectAPP::m_font.End();


	}
};

class OptionScene:public IScene{
public:

	
	Button*** _btn_map;
	static const DWORD _btn_map_size = 4;
	DWORD _sx;
	DWORD _sy;
	

	virtual bool Update() {
		double delta = 1.0/60.0;
		ATG::GAMEPAD* gc = ATG::Input::GetMergedInput();

		DWORD prev_sx = _sx;
		DWORD prev_sy = _sy;

		// Handle input with safe modulo
		if (gc->wPressedButtons & XINPUT_GAMEPAD_DPAD_UP) {
			_sy = (_sy - 1 + _btn_map_size) % _btn_map_size;
		}
		if (gc->wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
			_sy = (_sy + 1) % _btn_map_size;
		}
		if (gc->wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
			_sx = (_sx - 1 + _btn_map_size) % _btn_map_size;
		}
		if (gc->wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
			_sx = (_sx + 1) % _btn_map_size;
		}

	

		// Revert if invalid position
		if (_btn_map[_sy][_sx] == NULL) {

			_sx = prev_sx;
			_sy = prev_sy;
		}

		// Update focus only if position changed
		if (prev_sx != _sx || prev_sy != _sy) {
			_btn_map[prev_sy][prev_sx]->FocusOFF();
			_btn_map[_sy][_sx]->FocusON();
		}

		// Update buttons
		for (size_t r = 0; r < _btn_map_size; ++r) {
			for (size_t c = 0; c < _btn_map_size; ++c) {
				if (_btn_map[r][c]) {
					bool pressed = (gc->wPressedButtons & XINPUT_GAMEPAD_A) != 0;
					_btn_map[r][c]->Update(pressed);
				}
			}
		}


		if (gc->wPressedButtons & XINPUT_GAMEPAD_START){
			return false;
		}
		return true;
	}

	



	virtual bool Render(){

	
		ATG::RenderBackground( 0xff0000ff, 0xff000000 );




		ModSelectAPP::m_font.Begin();
		ModSelectAPP::m_font.DrawText(600,400,0xFFFFFFFF,L"OptionScene",0,0);
		ModSelectAPP::m_font.End();


		for (size_t r = 0; r < _btn_map_size; ++r) {
			for (size_t c = 0; c < _btn_map_size; ++c) {
				if (_btn_map[r][c]){
					_btn_map[r][c]->Render();
				}
			}
		}


		ModSelectAPP::m_pd3dDevice->Present(NULL, NULL, NULL, NULL);


		return true;

	}


	virtual bool OnEnd(){

		for (size_t r = 0; r < _btn_map_size; ++r) {
			for (size_t c = 0; c < _btn_map_size; ++c) {
				delete _btn_map[r][c];
			}
		}
		return true;
	}
	virtual bool OnStart(){

		_btn_map = new Button**[_btn_map_size];
		//Empty
		for (size_t r = 0; r < _btn_map_size; ++r) {
			_btn_map[r] = new Button*[_btn_map_size];
			for (size_t c = 0; c < _btn_map_size; ++c) {
				_btn_map[r][c] = NULL;
			}
		}
		_sx = 0;
		_sy = 0;
		
		_btn_map[0][0] = new Button("DebugLogV4HUD",0,0,D3DCOLOR_ARGB(255,255,0,0),&DebugOptions::GetDebugLogV4HUD());


		_btn_map[0][1] = new Button("userScrolled",300,0,D3DCOLOR_ARGB(255,255,0,0),&DebugOptions::GetuserScrolled());
		_btn_map[0][2] = new Button("HideXNCP",600,0,D3DCOLOR_ARGB(255,255,0,0),&DebugOptions::GetHideXNCP());
		_btn_map[0][3] = new Button("showlog",900,0,D3DCOLOR_ARGB(255,255,0,0),&DebugOptions::Getshowlog());

		//next-row
		_btn_map[1][0] = new Button("showuimsg",0,100,D3DCOLOR_ARGB(255,255,0,0),&DebugOptions::Getshowuimsg());
		_btn_map[1][1] = new Button("BreakPoint",300,100,D3DCOLOR_ARGB(255,255,0,0),&DebugOptions::GetBreakPoint());



		for (size_t r = 0; r < _btn_map_size; ++r) {
			for (size_t c = 0; c < _btn_map_size; ++c) {
				if (_btn_map[r][c]) _btn_map[r][c]->FocusOFF();
			}
		}
	


		return true;
	}

};



class ModScene:public IScene{


	DWORD m_DeviceID;
	std::vector<bool> _selected;
	HANDLE LoacArcThread;
	bool LoacArcThreadFinished;
	std::string _current_arc_process;

	float _down;
	float _up;


	bool changed;
	int index;
	int index_max;
	size_t pos_y;

	std::vector<std::string> _directory;
	static std::map<std::string,D3DTexture*> _directory_thumbnail; //yes static 


	
	//let say, (c_str only COPY) test only
	static DWORD LoadThumbnailTask(void* c_str){
	
		LoadThumbnail(std::string((const char*)c_str));
		delete c_str; //remove pointer
		
		return 0;
	}
	static void LoadThumbnail(std::string& thumb_path){
		
		D3DTexture* out = NULL;
		D3DXCreateTextureFromFile(ATG::g_pd3dDevice,thumb_path.c_str(),&out);
		
		if (out){
			_directory_thumbnail[thumb_path] = out; //save-to-system
			out->AddRef(); //1 one time to not release later (release only when clear)
		}
		return;
	}
	
	
	
	
	void LoadThumbnail(){
		for (std::vector<std::string>::iterator it = _directory.begin();it!= _directory.end();it++){
			
			std::string path_to_thumbnail = std::string(MOD_DIR) + *it + std::string("\\thumbnail.dds");
			D3DTexture* out = NULL;
			D3DXCreateTextureFromFile(ATG::g_pd3dDevice,path_to_thumbnail.c_str(),&out);
			if (out)
				_directory_thumbnail[*it] = out;

		}
	}
	void FreeThumbnail(){
		for (std::map<std::string,D3DTexture*>::iterator it = _directory_thumbnail.begin();it!= _directory_thumbnail.end();it++){
			it->second->Release();
		}
	}



	std::vector<std::string> _directory_from_save;


	void selindex(int to){
		index+=to;
		if (index < 0){
			index = (index_max - 1) - (index + 1);
		}
		else{
			index = index % index_max;
		}
	}





	void UnLoadArcs(){

		Sonicteam::SoX::ArcHandleMgr* mgr = &Sonicteam::SoX::ArcHandleMgr::getInstance();
		Sonicteam::SoX::FileSystemXenon* xenon = &Sonicteam::SoX::FileSystemXenon::getInstance();
		Sonicteam::SoX::FileSystemXenon* FileSystem = &SSINGLETON(Sonicteam::SoX::FileSystemXenon)::getInstance();
		Sonicteam::SoX::FileSystemArc* ArcSystem = &Sonicteam::SoX::FileSystemArc::getInstance();
		Sonicteam::DocMarathonImp* _this = 	DOC;


		Sonicteam::SoX::IResourceMgr* ArcMgr = (Sonicteam::SoX::IResourceMgr*)(0x82D366D4);
		Sonicteam::SoX::ResourceManager* ResourceManager = (Sonicteam::SoX::ResourceManager*)0x82D3B224;


		int index = -1;
		for (std::map<int,Sonicteam::SoX::HoldMGR>::iterator it = ResourceManager->ManagerResouceMgr.begin();it!=ResourceManager->ManagerResouceMgr.end();it++){
			if (it->second.Mgr == ArcMgr) {
				index = it->first;
				break;
			}
		}


		for (std::map<std::string,Sonicteam::SoX::ArcHandle*>::iterator it = DLL_ARCS.begin();it!= DLL_ARCS.end();){


			REF_TYPE(Sonicteam::SoX::IResource) hold_more = (Sonicteam::SoX::IResource*)it->second;

			//	PushXenonMessage(L"MSG",it->second->ResourceStr3.c_str());
			//	PushXenonMessage(L"MSG",it->second);
			//	PushXenonMessage(L"MSG",it->second->GetReferenceCount());


			std::vector<REF_TYPE(Sonicteam::SoX::IResource)>::iterator xt =  std::find(_this->ArcVector1.begin(),_this->ArcVector1.end(),hold_more);
			if (xt != _this->ArcVector1.end()){
				_this->ArcVector1.erase(xt);
				//		PushXenonMessage(L"MSG","ERASED");
			}


			//	PushXenonMessage(L"MSG",it->second->GetReferenceCount());
			//	PushXenonMessage(L"MSG","______________________");

			it->second->LinkHandle.SafeDisconnect();
			it =DLL_ARCS.erase(it);
		}



	}

	static DWORD LoadArcs(void* param){
		//Load mod arcs
		//DirectoryGetDirs(files,std::string(MOD_DIR));



		Sonicteam::SoX::ArcHandleMgr* mgr = &Sonicteam::SoX::ArcHandleMgr::getInstance();
		Sonicteam::SoX::FileSystemXenon* xenon = &Sonicteam::SoX::FileSystemXenon::getInstance();
		Sonicteam::SoX::FileSystemXenon* FileSystem = &SSINGLETON(Sonicteam::SoX::FileSystemXenon)::getInstance();
		Sonicteam::SoX::FileSystemArc* ArcSystem = &Sonicteam::SoX::FileSystemArc::getInstance();
		ModScene* mrg = (ModScene*)param;
		Sonicteam::DocMarathonImp* _this = 	DOC;

		for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {

			std::string mod_dir = std::string(MOD_DIR) + *it + std::string("\\");
			std::string mod_dir_win32 = mod_dir + "win32\\archives\\";
			std::string mod_dir_xenon = mod_dir + "xenon\\archives\\";
			std::vector<std::string> dir_arcs_xenon;
			std::vector<std::string> dir_arcs_win32;
			xenon->FSDirectoryGetFiles(dir_arcs_xenon,mod_dir_xenon,std::string("arc"));
			xenon->FSDirectoryGetFiles(dir_arcs_win32,mod_dir_win32,std::string("arc"));

			
			std::vector<std::string> _arc_folder_xenon;
			std::vector<std::string> _arc_folder_win32;

			DirectoryGetDirs(_arc_folder_xenon,std::string(mod_dir_xenon)); 
			DirectoryGetDirs(_arc_folder_win32,std::string(mod_dir_win32));

			for (std::vector<std::string>::iterator jt = _arc_folder_xenon.begin(); jt != _arc_folder_xenon.end(); ++jt) {
				std::string arc_file =  mod_dir_xenon + *jt;
				DLL_ARCS_FOLDER.push_back(arc_file);
			}
			for (std::vector<std::string>::iterator jt = _arc_folder_win32.begin(); jt != _arc_folder_win32.end(); ++jt) {
				std::string arc_file =  mod_dir_win32 + *jt;
				DLL_ARCS_FOLDER.push_back(arc_file);
			}


				
		

			for (std::vector<std::string>::iterator jt = dir_arcs_xenon.begin(); jt != dir_arcs_xenon.end(); ++jt) {
				std::string arc_file =  mod_dir_xenon + *jt;
				arc_file = arc_file.substr(6);
				mrg->_current_arc_process = arc_file;
				_this->ArcVector1.push_back(ArcHandle(arc_file,3,0));
				//PushXenonMessage(L"MSG",arc_file.c_str());
				DLL_ARCS[arc_file] = (Sonicteam::SoX::ArcHandle*)(_this->ArcVector1.rbegin())->get();
			}	

			for (std::vector<std::string>::iterator jt = dir_arcs_win32.begin(); jt != dir_arcs_win32.end(); ++jt) {
				std::string arc_file =  mod_dir_win32 + *jt;
				arc_file = arc_file.substr(6);
				mrg->_current_arc_process = arc_file;
				_this->ArcVector1.push_back(ArcHandle(arc_file,3,0));
				//PushXenonMessage(L"MSG",arc_file.c_str());
				DLL_ARCS[arc_file] = (Sonicteam::SoX::ArcHandle*)(_this->ArcVector1.rbegin())->get();
			}
		}
		mrg->LoacArcThreadFinished = true;

		return true;
	}





	virtual bool Update(){

		double delta = 1.0/60.0; // later 


		ATG::GAMEPAD* gc =  ATG::Input::GetMergedInput();

		if (gc->wLastButtons != 0 ) {
			//PushBreakPoint(__FILE__,__LINE__,"%d",gc->wLastButtons);
		}


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

		if ((gc->wLastButtons & XINPUT_GAMEPAD_Y) != 0){

			app.SetScene(new OptionScene());
			return true;
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
			changed = true;
		}

		if (gc->wPressedButtons & XINPUT_GAMEPAD_START){


			this->UnLoadArcs();
			files.clear();
			DLL_ARCS_FOLDER.clear();
			for (int i = 0;i<_directory.size();i++){
				if (_selected[i]) files.push_back(_directory[i]);
			}
		
			if (files.size() > 0 ){
				//PushBreakPoint(__FILE__,__LINE__,"files.size()>0");
			}

			if (changed) ProcessSaveToDrive();
			ResumeThread(this->LoacArcThread);
			return true;
		}

		if (this->LoacArcThreadFinished ) return false;


		return true;
	}



	// Define vertex structure with untransformed coordinates
	struct Vertex2D {
		float x, y, z;  // Untransformed position (z=0 for 2D)
		float u, v;     // Texture coordinates
	};

	void DrawTexture(LPDIRECT3DTEXTURE9 texture, int x, int y, int width, int height) {
		// 1. Enable alpha blending
		ATG::g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		ATG::g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		ATG::g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	

		// 3. Set texture filtering
		ATG::g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		ATG::g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		// 4. Define vertices (ensure z=0 and RHW=1 if using D3DFVF_XYZRHW)
		struct Vertex2D {
			float x, y, z;
			float u, v;
		};

		Vertex2D quad[4] = {
			{ (float)x,         (float)y,          0.0f,  0.0f, 0.0f },
			{ (float)x+width,  (float)y,          0.0f,  1.0f, 0.0f },
			{ (float)x+width,  (float)y+height,   0.0f,  1.0f, 1.0f },
			{ (float)x,        (float)y+height,   0.0f,  0.0f, 1.0f }
		};

		// 5. Render
		ATG::g_pd3dDevice->SetTexture(0, texture);
		ATG::g_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
		ATG::g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, quad, sizeof(Vertex2D));

	
	}

	virtual bool Render(){

		HRESULT hr;
		ATG::RenderBackground( 0xff0000ff, 0xff000000 );

		ModSelectAPP::m_font.Begin();
		ModSelectAPP::m_font.SetScaleFactors( 1.0f, 1.0f );
		ModSelectAPP::m_font.DrawText( 0, 0, 0xffffff00,L"Mod Manager(v01) PRESS START TO EXIT", ATGFONT_RIGHT );

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


			ModSelectAPP::m_font.DrawText(0, cur_y, cur_color, wideString.c_str());
		}

		if (_directory_thumbnail.find(_directory[index]) != _directory_thumbnail.end()){

			D3DTexture* tex =  _directory_thumbnail[_directory[index]];

			DrawTexture(tex,0,0,640,480);
		}



		wchar_t buffer[0x256];
		ZeroMemory(buffer, sizeof(buffer));
		swprintf_s(buffer, _countof(buffer), L"%d/%d/%d", index+1, index_max,scount);
		ModSelectAPP::m_font.DrawText(0, 550, 0xffffff00, buffer, ATGFONT_CENTER_X);


		// Convert std::string to std::wstring
		std::wstring w_current_arc_process(_current_arc_process.begin(), _current_arc_process.end());

		wchar_t buffer2[0x256];
		ZeroMemory(buffer2, sizeof(buffer2));
		swprintf_s(buffer2, _countof(buffer2), L"ARC-PROCESS: %ls", w_current_arc_process.c_str());
		ModSelectAPP::m_font.DrawText(0, 550, 0xffffff00, buffer2, ATGFONT_RIGHT);
		ModSelectAPP::m_font.End();



		ModSelectAPP::m_pd3dDevice->Present(NULL, NULL, NULL, NULL);


		return true;

	}



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


	void ProcessLoadFromDrive(){
		ProcessDeviceUI();
		OpenContent();
	

		// Create the file using Windows API
		HANDLE MOD_DB = CreateFile(
			"save:\\mod.info",
			GENERIC_READ,
			0,
			NULL, // Security attributes
			OPEN_EXISTING,
			FILE_ATTRIBUTE_READONLY,
			NULL // Template file
			);

		if (MOD_DB != INVALID_HANDLE_VALUE){
			DWORD _size_high = 0;
			DWORD _size = GetFileSize(MOD_DB,&_size_high);
			char* sf  = (char*)malloc(_size);

			ReadFile(MOD_DB,sf,_size,&_size_high,0);


			DWORD _count = *(DWORD*)(sf + 4);

			for (int i = 0; i < _count; i++) {

				const char* _mod_name =  (const char*)((DWORD)sf + *(DWORD*)&sf[8 + (i*4)]);

				std::vector<std::string>::iterator it = std::find(_directory.begin(), _directory.end(), _mod_name);
				if (it != _directory.end()) {
					size_t index = std::distance(_directory.begin(), it);
					_selected[index] = true;
				}

			}
			free(sf);
		}

	
		CloseHandle(MOD_DB);
		CloseContent();
	}

	void ProcessSaveToDrive(){
		ProcessDeviceUI();
		OpenContent();


		// Create the file using Windows API
		HANDLE MOD_DB = CreateFile(
			"save:\\mod.info",
			GENERIC_READ |  GENERIC_WRITE,
			0,
			NULL, // Security attributes
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL // Template file
			);

	


		DWORD _OUT;
		DWORD _header = 0x73617665; //save
		WriteFile(MOD_DB,&_header,4,&_OUT,0);

		DWORD _count = files.size();
		WriteFile(MOD_DB,&_count,4,&_OUT,0);

		//8

		DWORD _data_offset = 8 + _count * 4;
		SetFilePointer(MOD_DB,8 + _count * 4,0,FILE_BEGIN); //Data Section

		for (std::vector<std::string>::iterator _f = files.begin();_f!= files.end();_f++){
			WriteFile(MOD_DB,_f->c_str(),_f->size(),&_OUT,0);
			WriteFile(MOD_DB,"\0",1,&_OUT,0);
		}


		SetFilePointer(MOD_DB,8,0,FILE_BEGIN);
		DWORD _pointer = _data_offset;
		for (std::vector<std::string>::iterator _f = files.begin();_f!= files.end();_f++){

			WriteFile(MOD_DB,&_pointer,4,&_OUT,0);
			_pointer += _f->size() + 1;

		}

		CloseHandle(MOD_DB);
		CloseContent();
	}

	virtual bool OnStart(){


		changed = false;
		LoacArcThread =  CreateThread(0,65535,LoadArcs,this, CREATE_SUSPENDED,0);
		LoacArcThreadFinished = false;




		pos_y = 0;
		DirectoryGetDirs(_directory,std::string(MOD_DIR));
	//	LoadThumbnail();
		index = 0;




		index_max = _directory.size();
		_selected = std::vector<bool>(index_max);

		this->UnLoadArcs();
		ProcessLoadFromDrive();

		return true;

	}

	virtual bool OnEnd(){
		FreeThumbnail();
		CloseHandle(LoacArcThread);

		return true;
	}
	


};



std::map<std::string,D3DTexture*>  ModScene::_directory_thumbnail;






HOOKV3(0x82160B98,void*,CreateMarathonState,(Sonicteam::DocMarathonImp*,void*),(_this,arg2),Sonicteam::DocMarathonImp* _this, void* arg2){



	DOC = _this;

	Sonicteam::SoX::ArcHandleMgr* mgr = &Sonicteam::SoX::ArcHandleMgr::getInstance();
	Sonicteam::SoX::FileSystemXenon* xenon = &Sonicteam::SoX::FileSystemXenon::getInstance();
	Sonicteam::SoX::FileSystemXenon* FileSystem = &SSINGLETON(Sonicteam::SoX::FileSystemXenon)::getInstance();
	Sonicteam::SoX::FileSystemArc* ArcSystem = &Sonicteam::SoX::FileSystemArc::getInstance();
	//ArcSystem->FileSystemFolder.push_back(std::string("game:\\"));
	xenon->FileSystemFolder.push_back(std::string("game:\\"));



	if (DebugOptions::GetEnableDevStuff()){
		size_t ApplicationXenon = *(size_t*)0x82D3B348;
		app.InitializeDevice(*(D3DDevice**)(ApplicationXenon + 0x24));;
		app.RenderOBJInit();

	}
	if (DebugOptions::GetAlwaysRunModLoaderAtStartup()){
		app.SetScene(new ModScene());
		app.Run();
	}

	









	HANDLE FileHandle;
	std::stringstream arc_full_path;
	std::stringstream error_msg; 



	for (std::map<std::string,FlagFile>::iterator it = FileSystemNew::FileSystemNewMap.begin();it!= FileSystemNew::FileSystemNewMap.end();it++){


		std::string arc_full_path = FileSystem->FSGetPath(it->first,it->second.flag);

		if (FileSystem->FSPathExist(arc_full_path)){
			_this->ArcVector1.push_back(ArcHandle(it->first,it->second.flag,it->second.SaveInMemory)); //-1 IF NOT 1 resource is being loaded but ??? something else happend and it destroy object depsite i can find csdobject in memory
			DLL_ARCS_NATIVE[arc_full_path] = (Sonicteam::SoX::ArcHandle*)(_this->ArcVector1.rbegin())->get();

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



bool SearchChunkEX_NATIVE(Sonicteam::SoX::FileHandleARC* handlearc,Sonicteam::SoX::ArcHandleMgr* mgr,std::string& search_path,Sonicteam::SoX::ArcHandleSearchResource& search_chunk){



	for (std::map<std::string,Sonicteam::SoX::ArcHandle*>::iterator it = DLL_ARCS_NATIVE.begin();it!= DLL_ARCS_NATIVE.end();it++){

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

bool SearchExternalArcFolder(std::string& path,std::string& opath) {


	static bool files_fail = false;
	Sonicteam::SoX::ArcHandleMgr* mgr = &Sonicteam::SoX::ArcHandleMgr::getInstance();
	Sonicteam::SoX::FileSystemXenon* xenon = &Sonicteam::SoX::FileSystemXenon::getInstance();
	std::string fpath = Sonicteam::SoX::FileHandleARC::GetARCChunkPath(mgr, path);


	for (std::vector<std::string>::iterator it = DLL_ARCS_FOLDER.begin(); it != DLL_ARCS_FOLDER.end(); ++it) {
		//PushXenonMessage(L"MSG",it->c_str());
		
		std::string mod_file = *it + std::string("\\")+ fpath;
		//PushXenonMessage(L"MSG",mod_file.c_str());

		//AddMessage("SearchExternalArcFolder: file :  %s",mod_file.c_str());

		if ( xenon->FSPathExist(mod_file)){

		//	AddMessage("SearchExternalArcFolder: file :  %s",mod_file.c_str());
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


		//No Arc System
		if (SearchExternalFile(t->filepath,ext_path)){
			BranchTo(0X828B2B18,int,t,&ext_path); // Old File Loading
		}
		//No Arc System, But ARCS like folder
		else if (SearchExternalArcFolder(t->filepath,ext_path)){
			BranchTo(0X828B2B18,int,t,&ext_path); // Old File Loading
		}
		//Load File from DLL ARC 
		else if (SearchChunkEX_NATIVE(t,mgr,t->filepath,chunkf)){
			BranchTo(0x828B2BF8,int,t,&chunkf); // LoadFileFromArc
		}
		//Load File From DLL ARC Mod
		else if (SearchChunkEX(t,mgr,t->filepath,chunkf)){
			BranchTo(0x828B2BF8,int,t,&chunkf); // LoadFileFromArc
		}
		//Original Behaviour
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



	for (std::vector<std::string>::iterator it = DLL_ARCS_FOLDER.begin();it!= DLL_ARCS_FOLDER.end();it++){
		std::string mod_file = *it + std::string("\\")+ path;
		if (xenon->FSPathExist(mod_file)){
			//AddMessage("DLL_ARCS_FOLDER, %s",mod_file.c_str());
			return true;
		}
	}

	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
		std::string mod_dir = std::string(MOD_DIR) + *it + std::string("\\");
		std::string file_find = mod_dir + path;

		if (xenon->FSPathExist(file_find)){
			//AddMessage("files, %s",file_find.c_str());
			return true;
		}

	}



	bool result = BranchTo(0x825BE438 ,int,_this,&path);
	//if (result) AddMessage("0x825BE438, %s",path.c_str());

	return result;
}


bool __fastcall FileSystemDirExist(Sonicteam::SoX::FileSystemArc* _this,std::vector<std::string>& files_dir, std::string& pathf,std::string& extf){


	Sonicteam::SoX::FileSystemXenon* xenon = &Sonicteam::SoX::FileSystemXenon::getInstance();
	std::string path = pathf; //game://xenon/.xma
	path = path.substr(6); //remvoe game://


	bool result = 0;


	for (std::vector<std::string>::iterator it = DLL_ARCS_FOLDER.begin();it!= DLL_ARCS_FOLDER.end();it++){
		std::string mod_file = *it + std::string("\\")+ path;
		if (xenon->FSDirectoryGetFiles(files_dir,mod_file,extf)){
			//AddMessage("FileSystemDirExist(DLL_ARCS_FOLDER), %s",mod_file.c_str());
			result |= true;
		}
	}



	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
		std::string mod_dir = std::string(MOD_DIR) + *it + std::string("\\");
		std::string file_find = mod_dir + path;

		if (xenon->FSDirectoryGetFiles(files_dir,file_find,extf)){
			//AddMessage("FileSystemDirExist(files), %s",file_find.c_str());
			result |= true;
		}

	}



	bool result_2 =  BranchTo(0x825BE3F0 ,bool,_this,&files_dir,&pathf,&extf);
	//AddMessage("[DLL] %s,.%s:%d",pathf.c_str(),extf.c_str() ,files_dir.size());

	result |= result_2;
	return result;

}






class APP_NEW:public Sonicteam::SoX::ApplicationXenon{

public:



	void PrecisionFrameCap_Xenon(LARGE_INTEGER StartFrame,int targetFPS) {

		if (targetFPS == -1 || targetFPS == -2) return;
		LARGE_INTEGER frequency, currentTime;
		QueryPerformanceFrequency(&frequency);  // ticks per second
		QueryPerformanceCounter(&currentTime);  // current ticks

		// Calculate elapsed time in seconds
		double elapsedSeconds = double(currentTime.QuadPart - StartFrame.QuadPart) / frequency.QuadPart;

		// Calculate target frame duration in seconds
		double targetFrameDuration = 1.0 / targetFPS;

		if (elapsedSeconds < targetFrameDuration) {
			// Calculate how many milliseconds to sleep
			DWORD sleepMilliseconds = DWORD((targetFrameDuration - elapsedSeconds) * 1000);

			// Sleep for the remaining time to cap the frame rate
			if (sleepMilliseconds > 0) {
				Sleep(sleepMilliseconds);
			}
			// Optional: busy wait for more precise timing (less CPU friendly)
			// This can be added if you want sub-millisecond precision
			// but usually Sleep is enough for typical frame caps.
		}
	}




	
	void D3DRenderFrameAndSwapBuffersEX()
	{


		_D3DDevice->SetShaderGPRAllocation(0,0,0);
		if (FPS_CAP != -2) _D3DDevice->SynchronizeToPresentationInterval(); //uncap FPS may cause issue on console (uncync from 60 )
		_D3DDevice->Resolve(0,0,FrontBufferTexture,0,0,0,0,0.0,0,0);
		_D3DDevice->Swap(FrontBufferTexture,0);

	}

	
	
	

	void _Run(){


		//FUCKS r22 register
		volatile int r22_dummy;
		__asm{
			mr r22_dummy,r22
			mr r22,r22_dummy
		}

		LARGE_INTEGER unk0x58;
		LARGE_INTEGER unk0x60;
		LARGE_INTEGER unk0x68;
		LARGE_INTEGER unk0x70;
		LARGE_INTEGER unk0x90;
		LARGE_INTEGER unk0x98;
		Sonicteam::SoX::PerformanceFrequency::QueryFrequency();
		Sonicteam::SoX::PerformanceFrequency::QPerfomanceCounter(&unk0x58);
		for (;!IsDisabled;){



			Sonicteam::SoX::PerformanceFrequency::QPerfomanceCounter(&unk0x60);
			APP_START_FRAME.QuadPart = unk0x60.QuadPart;

			sub_825B0988(this);
			sub_825B0AC0(this);

			DWORD _lock1 =  this->Lock1;
			if (this->NotifyTargetSignInState == 1){
				if (this->NotifyTargetSignInStateMSG == 0x3E5){
					_lock1 = 1;
				}
				else{
					this->NotifyTargetSignInState = 0;
					this->LastSignInState = XUserGetSigninState(*(int*)(0x82B7D6C8));
				}
			}
			this->Lock1 = _lock1;
			Sonicteam::SoX::PerformanceFrequency::QPerfomanceCounter(&unk0x68);
			LARGE_INTEGER unk0x68_unk0x58_calc; unk0x68_unk0x58_calc.QuadPart =  unk0x68.QuadPart - unk0x58.QuadPart;
			unk0x58.QuadPart = unk0x68.QuadPart;
			unk0x70 = Sonicteam::SoX::PerformanceFrequency::PerfomanceScaleEX(&unk0x68_unk0x58_calc);
			double delta = ((double)unk0x70.QuadPart * 0.000001);
			

			ATG::GAMEPAD* gc = ATG::Input::GetMergedInput(0);

			
			if (gc->wLastButtons & XINPUT_GAMEPAD_BACK){
				_mod_selector+= delta;


				if (DebugOptions::GetDebugLogV4HUD()){
					if (gc->fY2 <= -0.25){
						DebugOptions::SetuserScrolled( false );
					}	

					if (gc->wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN){
						ClearMessages();
					}
				}

			}
			else{
				_mod_selector = 0.0;

				if (abs(gc->fY2) > 0.25){
					DebugOptions::Setscroll_acc( DebugOptions::Getscroll_acc()  + (DebugOptions::Getscroll_acc_min() + delta));
					DebugOptions::SetuserScrolled( true );
				}
				else{
					DebugOptions::Setscroll_acc (DebugOptions::Getscroll_acc_min() + delta);
				}
				DebugOptions::SetscrollOffset(DebugOptions::GetscrollOffset() + gc->fY2 * (delta * DebugOptions::Getscroll_acc() ));


			}
			if (_mod_selector > 2.0){
				_mod_selector = 0.0;

				if (gc->wLastButtons & XINPUT_GAMEPAD_Y){
					DebugOptions::SetDebugLogV4HUD( !DebugOptions::GetDebugLogV4HUD() );
				}
				else if (gc->wLastButtons & XINPUT_GAMEPAD_B){
					DebugOptions::SetHideXNCP ( !DebugOptions::GetHideXNCP() ); //i'll think about it
				}
			
				else if (gc->bRightTrigger){
					DebugOptions::Setshowlog( !DebugOptions::Getshowlog() );
				}
				else if (gc->bLastLeftTrigger){
					DebugOptions::Setshowuimsg(!DebugOptions::Getshowuimsg());
				}
				else if (gc->wLastButtons & XINPUT_GAMEPAD_X && DebugOptions::GetEnableDevStuff()){
					app.SetScene(new OptionScene());
					app.Run();
				}

				else{
					app.SetScene(new ModScene());
					app.Run();
					this->ToTitle = 1;
				}
			}

			

			FIRST_SCALE.QuadPart = unk0x70.QuadPart;
			double delta2 = ((double)unk0x70.QuadPart * 0.000001);

			if (FPS_CAP != -1 && FPS_CAP != -2){

				float* flt_82000B88 = (float*)0x92000B88;
				float* flt_8200CEAC = (float*)0x9200CEAC;
				float* flt_82018EE4 = (float*)0x92018EE4;
				float* flt_82019A98 = (float*)0x92019A98;
				float* flt_82B3ECC0 = (float*)0x92B3ECC0; ///
		
				*flt_82000B88 = 1.0/(float)FPS_CAP;
				//*flt_8200CEAC = 1.0/(float)FPS_CAP;
				//*flt_82018EE4 = 1.0/(float)FPS_CAP;
				//*flt_82019A98 = 1.0/(float)FPS_CAP;
				//*flt_82B3ECC0 = 1.0/(float)FPS_CAP;


			}
			ProcessDoc(delta2);
			Sonicteam::SoX::PerformanceFrequency::QPerfomanceCounter(&unk0x90);
			LARGE_INTEGER unk0x90_unk0x60_calc; unk0x90_unk0x60_calc.QuadPart =  unk0x90.QuadPart - unk0x60.QuadPart;
			LARGE_INTEGER end_scale =  Sonicteam::SoX::PerformanceFrequency::PerfomanceScaleEX(&unk0x90_unk0x60_calc);
			this->PefScalePre.QuadPart = end_scale.QuadPart;

			Sonicteam::SoX::PerformanceFrequency::QPerfomanceCounter(&unk0x98);
			LARGE_INTEGER unk0x98_unk0x60_calc; unk0x98_unk0x60_calc.QuadPart =  unk0x98.QuadPart - unk0x60.QuadPart;
			LARGE_INTEGER last_scale =  Sonicteam::SoX::PerformanceFrequency::PerfomanceScaleEX(&unk0x98_unk0x60_calc);
			this->PerfScalePost.QuadPart = last_scale.QuadPart;


			PrecisionFrameCap_Xenon(unk0x60,FPS_CAP);

			

		}
	}

};



HOOKV3_EXTERN_C(0x825B1870,void*,APPRUN,(APP_NEW*),(_this), APP_NEW* _this){

	_this->_Run();
	return (_this);
}


float FileSystemNew::lerp(float a, float b, float t)
{
	return a + t * (b - a);
}





extern "C" void APP_NEW_RENDER(APP_NEW* _this){

	double delta = (double)FIRST_SCALE.QuadPart * 0.000001;
	double end_delta = (double)_this->PerfScalePost.QuadPart * 0.000001;
	unsigned __int64 freq =   Sonicteam::SoX::PerformanceFrequency::getInstanceQuick().LastFrequency.QuadPart;




	if (DebugOptions::GetDebugLogV4HUD() && DebugOptions::GetEnableDevStuff()){


		if (DebugOptions::Getshowuimsg()){

			ModSelectAPP::m_font_2.Begin();
			for (std::map<DMSG_UI*,size_t>::iterator it = _Message_UI_.begin();it != _Message_UI_.end();it++){
				DMSG_UI* obj = it->first;
				ModSelectAPP::m_font_2.DrawText(obj->pos_x,obj->pos_y,obj->color,obj->_m.c_str(),0,0);
			}
			ModSelectAPP::m_font_2.End();
		}
	
		wchar_t _out_message[256];
		memset(_out_message,0,sizeof(_out_message));

		MEMORYSTATUS statex;
		statex.dwLength = sizeof(statex);
		GlobalMemoryStatus(&statex);

		double _max_line = (double)_Message_.size() * lineHeight;
		double _max_line_pre_scroll = _max_line  - (0.65*viewportHeight);


		wsprintfW(_out_message,  L"FPS: %03d, PhysMem: %d\\%d, VirtualMem: %d\\%d,  PageMem: %d\\%d, scroll : %f\\%f   ",(int)(1.0/delta),statex.dwAvailPhys /1024,statex.dwTotalPhys/1024,statex.dwAvailVirtual/1024,statex.dwTotalVirtual/1024,statex.dwAvailPageFile/1024,statex.dwTotalPageFile/1024,-DebugOptions::GetscrollOffset(),_max_line_pre_scroll);

		ModSelectAPP::m_font_2.Begin();
		ModSelectAPP::m_font_2.DrawText(0,50,0xFF00FFFF,_out_message,ATGFONT_RIGHT);
		ModSelectAPP::m_font_2.End();





		if (!DebugOptions::GetuserScrolled()) {
			DebugOptions::SetscrollOffset(  FileSystemNew::lerp(DebugOptions::GetscrollOffset(), -_max_line_pre_scroll ,delta) );

		}
		if (DebugOptions::Getshowlog()){
			ModSelectAPP::m_font_2.Begin();
			int y_position = viewportTop + DebugOptions::GetscrollOffset();

			// Calculate first and last visible message indices
			int firstVisible = std::max(0, static_cast<int>((viewportTop - y_position) / lineHeight));
			int lastVisible = std::min(static_cast<int>(_Message_.size()) - 1, firstVisible + static_cast<int>((viewportBottom - viewportTop) / lineHeight));


			// Only process visible messages
			for (int i = firstVisible; i <= lastVisible && i < _Message_.size(); ++i) {
				DMSG& msg = _Message_[i];
				const float currentY = y_position + (i * lineHeight);

				std::wstring msg_raw(msg._m.begin(), msg._m.end());
				wchar_t _msg_dbg_[1024];
				wsprintfW(_msg_dbg_, L"%s | %d", msg_raw.c_str(), msg._c);

				msg.Lerp(delta);
				ModSelectAPP::m_font_2.DrawText(5, currentY, msg.color, _msg_dbg_, ATGFONT_LEFT);
			}
			ModSelectAPP::m_font_2.End();

		}



		//Help
		ModSelectAPP::m_font_2.Begin();
		ModSelectAPP::m_font_2.DrawText(0,670,D3DCOLOR_ARGB(255,255,255,255),L"Development Build(SOJ)",ATGFONT_RIGHT);
		ModSelectAPP::m_font_2.End();

	}


	 _this->D3DRenderFrameAndSwapBuffersEX();

}


int __fastcall CPlatformDrawCache1(int a1, int a2, __int64 a3)
{
	if (!DebugOptions::GetHideXNCP()){
		BranchTo(0x826315C8,int,a1,a2,a3);
	}
	return 0;
}
//RENDER AFTER  ALL ACTIONS
void __fastcall EngineDocOnUpdate(Sonicteam::DocMarathonImp *_doc, double a2){

	DWORD _d1 = _doc->Docuint0xD1;
	DWORD _01 = _doc->DocUnkFlag01;
	BranchTo(0x825EA610,int,_doc,a2);

	if ( ((_01 >> 2) & 1) != 0 ){

	}
	else if ( _d1 ){

	}
	else{
		if (_doc->DocCurrentApplication->D3DSetShaderGPRAllocation()) _doc->DocCurrentApplication->D3DRenderFrameAndSwapBuffers();
	}

	
}


int GetRefreshRateFromDevice(IDirect3DDevice9* pDevice)
{
	if (!pDevice) return 0;

	D3DDISPLAYMODE displayMode;
	if (FAILED(pDevice->GetDisplayMode(0, &displayMode))) // 0 = first monitor
		return 0;

	return displayMode.RefreshRate;
}

void FileSystemNew::GlobalInstall()
{


	if (FPS_CAP != -1 || FPS_CAP == -2) {

		DWORD interval = D3DPRESENT_INTERVAL_ONE;

		DWORD monitorRefreshRate = 60;
	

		if (FPS_CAP >= monitorRefreshRate) {
			interval = D3DPRESENT_INTERVAL_ONE; // Cap at 60 FPS
		}
		else if (FPS_CAP <= monitorRefreshRate / 4) {
			interval = D3DPRESENT_INTERVAL_FOUR; // Minimum interval (15 FPS)
		}
		else {
			interval = D3DPRESENT_INTERVAL_THREE;
		}

	

		
		AddMessage("[DLL] monitorRefreshRate : %d, inverval : %d ",monitorRefreshRate,interval);
		WRITE_DWORD(0x825B1C84, 0x38800000 | interval);
	}


	


//	WRITE_DWORD(0x82000950,EngineDocOnUpdate);
//	WRITE_DWORD(0x825EA72C,0x60000000);
//	WRITE_DWORD(0x825EA738,0x48000024);

	//	WRITE_DWORD(0x825EA65C,0x60000000);	


	
	WRITE_DWORD(0x8204DD78 ,CPlatformDrawCache1);
	WRITE_DWORD(0x8204D970,APP_NEW_RENDER);
	



	
	INSTALL_HOOKV3EX(APPRUN,-1,true,9);

	//Use Auto Buffer because i cant create mine :C
	WRITE_DWORD(0x825B1C90,0x60000000);
	WRITE_DWORD(0x825B1C9C,0x60000000);

	WRITE_DWORD(0x82080424,LoadHandleARC);
	INSTALL_HOOKV3EX(CreateMarathonState,1,false,9);
	INSTALL_HOOKV3EX(ATGXMAFILEOpen,-1,true,9);
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
