#ifndef CORE_DEBUG
#define CORE_DEBUG
#include <xtl.h>
#include <sstream>

#include <algorithm>
#include <vector>
#include <string>

#include <map>
#include <queue>
 

struct DMSG_UI;
struct DMSG;


extern std::map<DMSG_UI*,size_t> _Message_UI_;
extern std::vector<DMSG> _Message_;




struct DMSG {

	float lerp2(float a, float b, float t)
	{
		return a + t * (b - a);
	}

	std::string _m;
	size_t _c;
	float anim;
	DWORD def_color;
	DWORD sel_color;
	DWORD color;
	bool is_animating;

	void StartAnimation(float duration = 1.0f) {
		anim = 1.0f;
		is_animating = true;
	}

	void Lerp(float delta) {
		if(!is_animating) return;

		anim -= delta;

		if(anim <= 0.0f) {
			anim = 0.0f;
			is_animating = false;
			color = def_color; // Reset to default when done
			return;
		}


		BYTE def_r = (def_color >> 16) & 0xFF;
		BYTE def_g = (def_color >> 8) & 0xFF;
		BYTE def_b = def_color & 0xFF;

		BYTE sel_r = (sel_color >> 16) & 0xFF;
		BYTE sel_g = (sel_color >> 8) & 0xFF;
		BYTE sel_b = sel_color & 0xFF;

		color = D3DCOLOR_ARGB(
			255,
			static_cast<BYTE>(lerp2(sel_r, def_r, 1.0f - anim)),
			static_cast<BYTE>(lerp2(sel_g, def_g, 1.0f - anim)),
			static_cast<BYTE>(lerp2(sel_b, def_b, 1.0f - anim))
			);
	}

	DMSG() {
		def_color = D3DCOLOR_ARGB(255, 0, 255, 255); // Cyan
		sel_color = D3DCOLOR_ARGB(255, 255, 0, 0);   // Red (example)
		color = def_color;
	}
};



struct DMSG_UI{
	std::wstring _m;
	DWORD pos_x;
	DWORD pos_y;
	DWORD color;



public:

	void Zero(){
		ZeroMemory(this,sizeof(DMSG_UI));
	}

	void Release(){
		_Message_UI_.erase(_Message_UI_.find(this));
		delete this;
	}

	void SetColor(DWORD color){
		this->color = color;
	}
	void SetPosX(DWORD posX){
		this->pos_x = posX;
	}
	void SetPosY(DWORD posY){
		this->pos_y = posY;
	}
	void SetPosXY(DWORD posX,DWORD posY){
		SetPosX(posX);
		SetPosY(posY);
	}
	void SetMSG(std::wstring& msg){
		this->_m = msg;

	}

};





void AddMessage(std::string& str);
void ClearMessages();
void AddMessage(const char* fmt_str,...);
void PushBreakPoint(const char* _file,size_t _line,const char* fmt_str,...);



template <typename T>
void PushXenonMessage(LPCWSTR Title,T value)
{

	LPCWSTR g_pwstrButtonsX_INL[1] = { L"------------OK----------------" };

	std::wstringstream ss;
	ss <<std::hex << value;

	MESSAGEBOX_RESULT result;
	XOVERLAPPED m_Overlapped; 

	XShowMessageBoxUI(0,Title,ss.str().c_str(),1,g_pwstrButtonsX_INL,1,XMB_ALERTICON,&result,&m_Overlapped);

	
}

template <typename T>
XOVERLAPPED* PushXenonMessage(LPCWSTR Title,T value,XOVERLAPPED* m_Overlapped)
{

	MESSAGEBOX_RESULT result;
	LPCWSTR g_pwstrButtonsX_INL[1] = { L"------------OK----------------" };
	std::wstringstream ss;
	ss <<std::hex << value;
	XShowMessageBoxUI(0,Title,ss.str().c_str(),1,g_pwstrButtonsX_INL,1,XMB_ERRORICON,&result,m_Overlapped);
	return m_Overlapped;
}





template <typename T>
void PushXenonMessage(LPCWSTR Title,T value,MESSAGEBOX_RESULT* oute)
{
	LPCWSTR g_pwstrButtonsX_INL[1] = { L"------------OK----------------" };
	std::wstringstream ss;
	ss <<std::hex << value;

	XShowMessageBoxUI(0,Title,ss.str().c_str(),1,g_pwstrButtonsX_INL,1,XMB_ALERTICON,oute,NULL);

}



#endif
