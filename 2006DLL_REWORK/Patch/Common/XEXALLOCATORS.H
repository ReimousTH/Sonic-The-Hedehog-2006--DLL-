#pragma once
#ifndef XEXALLOCATORS
#define XEXALLOCATORS




#include <xtl.h>
#undef realloc

extern void*& XapiProcessHeap;






extern WINBASEAPI HLOCAL WINAPI LocalAlloc(IN UINT uFlags,IN SIZE_T uBytes );

/*
extern WINBASEAPI HLOCAL WINAPI LocalReAlloc(IN HLOCAL hMem, IN SIZE_T uBytes,IN UINT uFlags);
*/


extern HLOCAL __stdcall LocalFree(HLOCAL hMem);







//Regular
inline void* operator new(size_t size) {

	if (XapiProcessHeap) {
		// Use the EXE's heap if available
		return ((void* (__fastcall *)(size_t))0x82186158)(size);
	} else {
		return HeapAlloc(GetProcessHeap(), 0, size);
	}

}

inline void operator delete(void* ptr) {
	if (ptr) {

		if (XapiProcessHeap) {
			((void* (__fastcall *)(void*))0x82186190)(ptr);
		}
		else{
			HeapFree(GetProcessHeap(), 0, ptr); 
		}
	}
	
}






inline void * __cdecl malloc(__in size_t _Size){
	if (XapiProcessHeap) {
		return ((void* (__fastcall *)(size_t))0x826DFDE0)(_Size);
	}
	else{
		return HeapAlloc(GetProcessHeap(),0,_Size);
	}

}

inline void __cdecl free(__inout_opt void * _Memory){
	if (XapiProcessHeap) {
		((void* (__fastcall *)(void*))0x826DFFE8)(_Memory);
	}
	else{
		HeapFree(GetProcessHeap(),0,_Memory);
	}
	
}


//not sure

extern void * __cdecl realloc(void* ptr, size_t size);

/*
void * __cdecl _recalloc(__in_opt void * _Memory, __in size_t _Count, __in size_t _Size);
void   __cdecl _aligned_free(__inout_opt void * _Memory);
void * __cdecl _aligned_malloc(__in size_t _Size, __in size_t _Alignment);
void * __cdecl _aligned_offset_malloc(__in size_t _Size, __in size_t _Alignment, __in size_t _Offset);
void * __cdecl _aligned_realloc(__in_opt void * _Memory, __in size_t _Size, __in size_t _Alignment);
void * __cdecl _aligned_recalloc(__in_opt void * _Memory, __in size_t _Count, __in size_t _Size, __in size_t _Alignment);
void * __cdecl _aligned_offset_realloc(__in_opt void * _Memory, __in size_t _Size, __in size_t _Alignment, __in size_t _Offset);
void * __cdecl _aligned_offset_recalloc(__in_opt void * _Memory, __in size_t _Count, __in size_t _Size, __in size_t _Alignment, __in size_t _Offset);
*/


#endif

