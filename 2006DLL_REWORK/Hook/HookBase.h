#pragma once

#include <xtl.h>
#include <cassert>


class HookV2
{
public:
	

	static bool IsNotEmulatedHardWare;

	HookV2(void);
	~HookV2(void);
	
	static bool CheckIsNotEmulatedHardWare();
	static InstalHook(bool IsReturn /*= false*/,UINT32 addr,UINT32 fnc);
	static WriteDWORD(UINT32,DWORD);
	static WriteWORD(UINT32,WORD);
	static WriteNOP(UINT32,int c);
	static void* GetAddress(UINT32 addr);

};



#pragma region Definitions



const DWORD JumpASMNoPreserve[] = { /*Dont always have enough space to preserve registers*/
	0x3d600000, // lis %r11, Destination@h
	0x616b0000, // ori %r11, %r11, Destination@l
	0x7d6903a6, // mtctr %r11
	0x4E800420  // bcctr (Used For PatchInJumpEx bcctr 20, 0 == bctr)
};
const DWORD JumpASMNoPreserveI[] = { /*Dont always have enough space to preserve registers*/
	0x3d600000, // lis %r11, Destination@h
	0x616b0000, // ori %r11, %r11, Destination@l
	0x7d6903a6, // mtctr %r11
	0x4E800421  // bcctr (Used For PatchInJumpEx bcctr 20, 0 == bctr)
};



#define ADDRESS_HIGHER(X) ( ( X >> 16 ) & 0xFFFF )
#define ADDRESS_LOWER(X)  ( X & 0xFFFF )

#define HOOK_EXTERN_C(returnType, callingConvention, functionName, location, ...) \
	DWORD original_addr_##functionName = (int)location ;\
	typedef returnType callingConvention functionName(__VA_ARGS__); \
	functionName* original##functionName = (functionName*)(location+0x10); \
	extern "C" returnType callingConvention implOf##functionName(__VA_ARGS__)



#define HOOK(returnType, callingConvention, functionName, location, ...) \
	DWORD original_addr_##functionName = (int)location ;\
	typedef returnType callingConvention functionName(__VA_ARGS__); \
	functionName* original##functionName = (functionName*)(location+0x10); \
	returnType callingConvention implOf##functionName(__VA_ARGS__)


#define HOOKL(externs,returnType, callingConvention, functionName, location, ...) \
	DWORD original_addr_##functionName = (int)location ;\
	typedef returnType callingConvention functionName(__VA_ARGS__); \
	functionName* original##functionName = (functionName*)(location+0x10); \
	externs returnType callingConvention implOf##functionName(__VA_ARGS__)


#define INSTALL_HOOK(functionName) \
	HookV2::InstalHook(false,(UINT64)original_addr_##functionName, (int)implOf##functionName); \

#define INSTALL_HOOKR(functionName) \
	HookV2::InstalHook(true,(UINT64)original_addr_##functionName, (int)implOf##functionName); \



#define WRITE_DWORD(target,RWORD) \
	HookV2::WriteDWORD(target,(DWORD)RWORD) \

#define WRITE_WORD(target,RWORD) \
	HookV2::WriteWORD(target,(WORD)RWORD) \

#define WRITE_NOP(target,c) \
	HookV2::WriteNOP(target,c) \

#define  ADDR(i) \
	HookV2::GetAddress(i)

#define malloc06(size) \
	BranchTo(0x82186158,int,size) \

#define free06(ptr) \
	BranchTo(0x826DFFE8,int,ptr) \









#define BranchTo(offset,rtype,...) \
	((rtype (__fastcall *)(...))offset)(__VA_ARGS__) \

#define CBranchTo(offset,tp,rtype,...) \
	((rtype (tp *)(...))offset)(__VA_ARGS__) \









//#define KinectTEST
//#define TEST
#ifndef TEST
#define HOOK_VALUE 0xFFCF000
#define XEX_PC_HEADER 0x31000
#else
#define HOOK_VALUE 0
#define XEX_PC_HEADER 0

#endif


#pragma endregion Definitions

