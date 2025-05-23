#include "Debug.h"



std::vector<DMSG> _Message_;
std::map<DMSG_UI*,size_t> _Message_UI_;

void AddMessage(std::string& str)
{
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

}

void AddMessage(const char* fmt_str,...)
{

	va_list args;
	va_start(args, fmt_str);
	int size = vsnprintf(NULL, 0, fmt_str, args);
	va_end(args);

	if (size <= 0) return;


	char* buffer = new char[size + 1];


	va_start(args, fmt_str); 
	vsnprintf(buffer, size + 1, fmt_str, args);
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


}
