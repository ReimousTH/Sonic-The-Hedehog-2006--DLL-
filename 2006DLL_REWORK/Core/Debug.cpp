#include "Debug.h"



std::vector<DMSG> _Message_;
std::map<DMSG_UI*,size_t> _Message_UI_;
size_t FPS_CAP = -1;



bool   DebugOptions::AlwaysRunModLoaderAtStartup = true;  //(_cntlzw(DebugLogV4HUD) & 0x20) != 0; !DebugLogV4HUD (not operation weird)
bool   DebugOptions::EnableDevStuff = true;  //(_cntlzw(DebugLogV4HUD) & 0x20) != 0; !DebugLogV4HUD (not operation weird)
bool   DebugOptions::DebugLogV4HUD = true;  //(_cntlzw(DebugLogV4HUD) & 0x20) != 0; !DebugLogV4HUD (not operation weird)
bool   DebugOptions::userScrolled = false;
bool   DebugOptions::HideXNCP = false;
bool   DebugOptions::showlog = true;
bool   DebugOptions::showuimsg = true;
bool   DebugOptions::BreakPoint = true;
bool   DebugOptions::YKhronoTimeACC = true;
CRITICAL_SECTION DebugOptions::g_MessageCriticalSection;

bool	DebugOptions::USEMUTEX = false;

double   DebugOptions::scrollOffset = 0;
double   DebugOptions::scroll_acc_min = 1.0;
double   DebugOptions::scroll_acc = 0;


std::map<std::string,size_t> DebugOptions::breakpoint_disabled;


bool& DebugOptions::GetAlwaysRunModLoaderAtStartup(){
	return AlwaysRunModLoaderAtStartup;
}
void DebugOptions::SetAlwaysRunModLoaderAtStartup(bool value){
	AlwaysRunModLoaderAtStartup = value;
}

bool& DebugOptions::GetEnableDevStuff(){
	return EnableDevStuff;
}


void DebugOptions::SetEnableDevStuff(bool value){
	EnableDevStuff = value;
}

bool& DebugOptions::GetDebugLogV4HUD(){
	return DebugLogV4HUD;
}
void DebugOptions::SetDebugLogV4HUD(bool value){
	DebugLogV4HUD = value;
}


	
bool& DebugOptions::GetuserScrolled(){
	return userScrolled;
}
void DebugOptions::SetuserScrolled(bool value){
	userScrolled = value;
}

bool& DebugOptions::GetHideXNCP(){
	return HideXNCP;
}
void DebugOptions::SetHideXNCP(bool value){
	HideXNCP = value;
}


bool& DebugOptions::Getshowlog(){
	 return showlog;
}
void DebugOptions::Setshowlog(bool value){
	showlog = value;
}

bool& DebugOptions::Getshowuimsg(){
	return showuimsg;
}
void DebugOptions::Setshowuimsg(bool value){
	showuimsg = value;
}

bool& DebugOptions::GetBreakPoint(){
	return BreakPoint;

}

void DebugOptions::SetBreakPoint(bool value){
	BreakPoint = value;
}



double& DebugOptions::GetscrollOffset(){
	return scrollOffset;
}
 void DebugOptions::SetscrollOffset(double value){
	scrollOffset = value; 
}


 double& DebugOptions::Getscroll_acc_min(){
	return scroll_acc_min;
}
 void DebugOptions::Setscroll_acc_min(double value){
	scroll_acc_min = value;
}

 double&DebugOptions:: Getscroll_acc(){
	return scroll_acc;
}
 void DebugOptions::Setscroll_acc(double value){
	scroll_acc = value;
}




 bool& DebugOptions::GetYKhronoTimeACC()
 {
	 return YKhronoTimeACC;
 }

 void DebugOptions::SetYKhronoTimeACC(bool value)
 {
	  YKhronoTimeACC = value;
 }

 void DebugOptions::Initilize()
 {
	  InitializeCriticalSection(&g_MessageCriticalSection);
 }

 void AddMessage(std::string& str)
{
	if (!DebugOptions::GetEnableDevStuff()) return;
	EnterCriticalSection(&DebugOptions::g_MessageCriticalSection); // Lock

	bool find = false;
	int cc = 0;
	for (std::vector<DMSG>::iterator it  =_Message_.begin();it !=_Message_.end();it++){
		if (it->_m == str){
			it->_c ++;
			it->StartAnimation();
			find = true;
			DMSG msg = *it;
			if ( (_Message_.size() - cc) > 10 ){
				_Message_.erase(it);
				_Message_.push_back(msg);
			}
			break;
		}
		cc++;
	}

	//
	if (!find){
		DMSG _n;
		_n._c = 1;
		_n._m = str;
		_n.StartAnimation();
		_Message_.push_back(_n);
	}

	LeaveCriticalSection(&DebugOptions::g_MessageCriticalSection); // Unlock

}

#define EX_BUFFER 0x10
void AddMessage(const char* fmt_str,...)
{
	
	if (!DebugOptions::GetEnableDevStuff()) return;
	va_list args;
	va_start(args, fmt_str);
	int size = vsnprintf(NULL, 0, fmt_str, args);
	va_end(args);

	if (size <= 0) return;


	char* buffer = new char[size + EX_BUFFER];


	va_start(args, fmt_str); 
	vsnprintf(buffer, size + EX_BUFFER, fmt_str, args);
	va_end(args);

	std::string result(buffer);
	delete[] buffer;


	AddMessage(result);
}

void ClearMessages()
{
	_Message_.clear();
}

\
//find a way
void PushBreakPoint(const char* _file,size_t _line,const char* fmt_str,...)
{

	if (!DebugOptions::GetEnableDevStuff()) return;
	if (!DebugOptions::GetBreakPoint()) return;
	std::string __file = std::string(_file);

	//Disable BreakPoint logic 
	if (DebugOptions::breakpoint_disabled.find(__file) != DebugOptions::breakpoint_disabled.end() && DebugOptions::breakpoint_disabled[__file] == _line ){
		return; // disable breakpoint
	}
	

	char buffer[4096];
	va_list args;

	// Format header
	int header_len = sprintf_s(buffer, sizeof(buffer), "File: %s\nLine: %d\n\nMessage:\n", _file, _line);

	// Format user message with vsnprintf_s
	va_start(args, fmt_str);
	vsnprintf_s(
		buffer + header_len,            // Destination
		sizeof(buffer) - header_len,    // Remaining buffer size
		_TRUNCATE,                      // Truncate if too long
		fmt_str, 
		args
		);
	va_end(args);
	

	wchar_t wbuffer[4096];
	swprintf(wbuffer, sizeof(wbuffer)/sizeof(wchar_t), L"%hs", buffer);

	// Show message box
	MESSAGEBOX_RESULT result;
	XOVERLAPPED m_Overlapped = {0};
	LPCWSTR buttons[2] = { L"Continue",L"Disable Breakpoint" };

	XShowMessageBoxUI(
		0,
		L"BREAKPOINT HIT",
		wbuffer,
		2,
		buttons,
		0,  // Default button index
		XMB_ALERTICON,
		&result,
		&m_Overlapped
		);

	// Wait for completion
	while(!XHasOverlappedIoCompleted(&m_Overlapped)) {
		Sleep(10);  // Prevent CPU spin
	}
	//Disable Breakpoint logic
	if (result.dwButtonPressed == 1){
		DebugOptions::breakpoint_disabled[__file] = _line;
	}


}


