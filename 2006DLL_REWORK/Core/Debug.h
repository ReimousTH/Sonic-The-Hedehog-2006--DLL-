#ifndef CORE_DEBUG
#define CORE_DEBUG
#include <xtl.h>
#include <sstream>


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
