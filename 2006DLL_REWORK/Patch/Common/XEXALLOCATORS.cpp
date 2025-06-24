#include "XEXALLOCATORS.H"


#undef LocalReAlloc

//#pragma optimize("g", on)

extern void*& XapiProcessHeap = *(void**)0x82D57540; // Address to EXE's heap

WINBASEAPI HLOCAL WINAPI LocalAlloc(IN UINT uFlags,IN SIZE_T uBytes)
{
	HANDLE handle = GetProcessHeap();
	if (XapiProcessHeap) handle = XapiProcessHeap;
	return (HLOCAL)HeapAlloc(handle,(uFlags >> 3) & 8,uBytes);

}

/*
WINBASEAPI HLOCAL WINAPI LocalReAlloc(IN HLOCAL hMem, IN SIZE_T uBytes,IN UINT uFlags)
{

	HANDLE handle = GetProcessHeap();
	if (XapiProcessHeap) handle = XapiProcessHeap;

	if ((uFlags & 0x40) != 0){
		uFlags = 8;
	}
	else{
		uFlags = (8*~uFlags) & 0x10;
	}
	return (HLOCAL)HeapReAlloc(handle,uFlags,hMem,uBytes);

}
*/

HLOCAL __stdcall LocalFree(HLOCAL hMem)
{
	HANDLE handle = GetProcessHeap();
	if (XapiProcessHeap) handle = XapiProcessHeap;
	return (HLOCAL)HeapFree(handle,0,hMem);

}

void * __cdecl realloc(void* ptr, size_t size)
{
	HANDLE handle = GetProcessHeap();
	if (XapiProcessHeap) handle = XapiProcessHeap;

	if (!ptr)
		return malloc(size);
	if (!size) {
		free(ptr);
		return 0;
	}

	if (size < 0xFFFFF000)
		return HeapReAlloc(handle, 0, ptr, size);
	return 0;

}
//#pragma optimize("", off) 