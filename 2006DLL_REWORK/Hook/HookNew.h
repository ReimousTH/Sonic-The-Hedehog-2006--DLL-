//
// Made by iMoD1998	
// V3.1
//
#pragma once
#ifndef DETOUR_H
#define DETOUR_H

#include <xtl.h>

#include <iostream>
#include <sstream>
#include <string>



#include <stdint.h>
#include <vector>
#include <map>



#include <Core/Debug.h>

#define MASK_N_BITS(N) ( ( 1 << ( N ) ) - 1 )
#define POWERPC_HI(X) ( ( X >> 16 ) & 0xFFFF )
#define POWERPC_LO(X) ( X & 0xFFFF )

//
// PowerPC most significant bit is addressed as bit 0 in documentation.
//
#define POWERPC_BIT32(N) ( 31 - N )

//
// Opcode is bits 0-5. 
// Allowing for op codes ranging from 0-63.
//
#define POWERPC_OPCODE(OP)       ( OP << 26 )
#define POWERPC_OPCODE_ADDI      POWERPC_OPCODE( 14 )
#define POWERPC_OPCODE_ADDIS     POWERPC_OPCODE( 15 )
#define POWERPC_OPCODE_BC        POWERPC_OPCODE( 16 )
#define POWERPC_OPCODE_B         POWERPC_OPCODE( 18 )
#define POWERPC_OPCODE_BCCTR     POWERPC_OPCODE( 19 )
#define POWERPC_OPCODE_ORI       POWERPC_OPCODE( 24 )
#define POWERPC_OPCODE_EXTENDED  POWERPC_OPCODE( 31 ) // Use extended opcodes.
#define POWERPC_OPCODE_STW       POWERPC_OPCODE( 36 )
#define POWERPC_OPCODE_LWZ       POWERPC_OPCODE( 32 )

#define POWERPC_OPCODE_LFS       POWERPC_OPCODE( 48 )

#define POWERPC_OPCODE_LD        POWERPC_OPCODE( 58 )

#define POWERPC_OPCODE_STWU        POWERPC_OPCODE( 37 )

#define POWERPC_OPCODE_STD       POWERPC_OPCODE( 62 )
#define POWERPC_OPCODE_MASK      POWERPC_OPCODE( 63 )

#define POWERPC_EXOPCODE(OP)     ( OP << 1 )
#define POWERPC_EXOPCODE_BCCTR   POWERPC_EXOPCODE( 528 )
#define POWERPC_EXOPCODE_MTSPR   POWERPC_EXOPCODE( 467 )

//
// SPR field is encoded as two 5 bit bitfields.
//
#define POWERPC_SPR(SPR) (UINT32)( ( ( SPR & 0x1F ) << 5 ) | ( ( SPR >> 5 ) & 0x1F ) )

//
// Instruction helpers.
//
// rD - Destination register.
// rS - Source register.
// rA/rB - Register inputs.
// SPR - Special purpose register.
// UIMM/SIMM - Unsigned/signed immediate.
//
#define POWERPC_ADDI(rD, rA, SIMM)  (UINT32)( POWERPC_OPCODE_ADDI | ( rD << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | SIMM & 0xFFFF )
#define POWERPC_ADDIS(rD, rA, SIMM) (UINT32)( POWERPC_OPCODE_ADDIS | ( rD << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | SIMM &0xFFFF )
#define POWERPC_LIS(rD, SIMM)       POWERPC_ADDIS( rD, 0, SIMM ) // Mnemonic for addis %rD, 0, SIMM
#define POWERPC_LWZ(rS, DS, rA)      (UINT32)( POWERPC_OPCODE_LWZ | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | ( (INT16)DS & 0xFFFF ) )

#define POWERPC_LFS(rS, DS, rA)      (UINT32)( POWERPC_OPCODE_LFS | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | ( (INT16)DS & 0xFFFF ) )


#define POWERPC_LI(rD, SIMM)        POWERPC_ADDI( rD, 0, SIMM )  // Mnemonic for addi %rD, 0, SIMM
#define POWERPC_MTSPR(SPR, rS)      (UINT32)( POWERPC_OPCODE_EXTENDED | ( rS << POWERPC_BIT32( 10 ) ) | ( POWERPC_SPR( SPR ) << POWERPC_BIT32( 20 ) ) | POWERPC_EXOPCODE_MTSPR )
#define POWERPC_MTCTR(rS)           POWERPC_MTSPR( 9, rS ) // Mnemonic for mtspr 9, rS
#define POWERPC_ORI(rS, rA, UIMM)   (UINT32)( POWERPC_OPCODE_ORI | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | UIMM )
#define POWERPC_BCCTR(BO, BI, LK)   (UINT32)( POWERPC_OPCODE_BCCTR | ( BO << POWERPC_BIT32( 10 ) ) | ( BI << POWERPC_BIT32( 15 ) ) | ( LK & 1 ) | POWERPC_EXOPCODE_BCCTR )
#define POWERPC_STD(rS, DS, rA)     (UINT32)( POWERPC_OPCODE_STD | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | ( (INT16)DS & 0xFFFF ) )
#define POWERPC_STW(rS, DS, rA)     (UINT32)( POWERPC_OPCODE_STW | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | ( (INT16)DS & 0xFFFF ) )
#define POWERPC_LD(rS, DS, rA)      (UINT32)( POWERPC_OPCODE_LD | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | ( (INT16)DS & 0xFFFF ) )

#define POWERPC_STWU(rS, DS, rA)      (UINT32)( POWERPC_OPCODE_STWU | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | ( (INT16)DS & 0xFFFF ) )

//
// Branch related fields.
//
#define POWERPC_BRANCH_LINKED    1
#define POWERPC_BRANCH_ABSOLUTE  2
#define POWERPC_BRANCH_TYPE_MASK ( POWERPC_BRANCH_LINKED | POWERPC_BRANCH_ABSOLUTE )

#define POWERPC_BRANCH_OPTIONS_ALWAYS ( 20 )







#define HOOKADDR(address) HookNew::UseEmulatedAddress == true ? (void*)(0x10000000 + (DWORD)address) : (void*)(0 + (DWORD)address)



//LUA

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}




class HookNew {
public:
	static std::map<void*, std::vector<HookNew*>>* SaveBuffer; 
	static std::map<void*, std::vector<HookNew*>>* SaveBufferVFT; 
	static int SavedBufferSIZE;

	lua_State* L;
	int lua_function;


	void* OriginalFunc;
	void* TargetFunc;
	char RunPosition; // -1 = Before, 1 = After
	bool Block;

	// Function to create a hook
	static HookNew* CreateHook(void* Target, void* TargetMap, void* Original,char fp13,int fp13_number,lua_State* fp13_state,char RunPosition,bool block,int register_choose);
	
	static HookNew CreateVFTHook(void* TargetFunc);;
	
	static void memcpy(void * _Dst,const void * _Src,size_t _Size);
	static bool IsEmulated();
	static bool UseEmulatedAddress;

	



	// Static function to handle saved buffer functionality
	static void SavedBufferFunc();
	static void SavedBufferFunc2();
	static void SavedBufferFunc3();
	static void SavedBufferFunc4();
	static void SavedBufferFunc5();
	static void SavedBufferFunc6();
	static void SavedBufferFunc7();
	static void SavedBufferFunc8();
};




#define HookBranchTo(offset,rtype,...) \
	((rtype (__fastcall *)(...))offset)(__VA_ARGS__) \



extern "C" void CallLuaFunc();


#define HOOKV3EXMAP_SHELL(addressTo, return_type, HookFuncName, call_args,args_name,lua_arg_call,...)


#define HOOKV3EXMAP_LUA \
	__asm{mr r3,r3x};\
	__asm{mr r4,r4x};\
	__asm{mr r5,r5x};\
	__asm{mr r6,r6x};\
	__asm{mr r7,r7x};\
	__asm{mr r8,r8x};\
	__asm{mr r9,r9x};\
	__asm{mr r10,r10x};\
	__asm{fmr fp1,fp1x};\
	__asm{fmr fp2,fp2x};\
	__asm{bl CallLuaFunc};\

#define HOOKV3EXMAP(addressTo, return_type, HookFuncName, call_args,args_name,...) \
	return_type HookFuncName##MAP(__VA_ARGS__) { \
		void* addressTo2 = (void*)addressTo;\
		return_type return_value = (return_type)0; \
		if (HookNew::SaveBuffer && HookNew::SaveBuffer->find((void*)addressTo2) != HookNew::SaveBuffer->end()){\
		\
			bool block = false; \
			for (std::vector<HookNew*>::iterator it =(*HookNew::SaveBuffer)[(void*)addressTo2].begin() ; it != (*HookNew::SaveBuffer)[(void*)addressTo2].end(); ++it) { \
			if ((*it)->RunPosition == -1){\
				block |= (*it)->Block;\
				if ((*it)->lua_function != 0 && (*it)->L != 0)\
				{\
				}\
				else{\
					void* tar = (*it)->TargetFunc; \
					return_value = ((return_type (__fastcall *)call_args)tar)args_name; \
				}\
			}\
			}\
			void* jump_to = (*HookNew::SaveBuffer)[(void*)addressTo2][0]->OriginalFunc; \
			if (!block && jump_to )  return_value =  ((return_type (__fastcall *)call_args)(jump_to))args_name; \
			\
			for (std::vector<HookNew*>::iterator it =(*HookNew::SaveBuffer)[(void*)addressTo2].begin() ; it != (*HookNew::SaveBuffer)[(void*)addressTo2].end(); ++it) { \
			if ((*it)->RunPosition == 1){\
				if ((*it)->lua_function != 0 && (*it)->L != 0)\
				{\
				}\
				else{\
					void* tar = (*it)->TargetFunc; \
					return_value = ((return_type (__fastcall *)call_args)tar)args_name; \
				}\
			}\
			}\
			return return_value; \
				\
		}\
		return return_value ;\
	}\







#define HOOKV3EX(addressTo, return_type, HookFuncName, call_args,args_name,...) \
	void* HookFuncName##addressto = (void*)addressTo; \
	HOOKV3EXMAP(addressTo,return_type,HookFuncName,call_args,args_name,__VA_ARGS__); \
	return_type HookFuncName(__VA_ARGS__) \


#define HOOKV3EX_EXTERN_C(addressTo, return_type, HookFuncName, call_args,args_name,...) \
	void* HookFuncName##addressto = (void*)addressTo; \
	HOOKV3EXMAP(addressTo,return_type,HookFuncName,call_args,args_name,__VA_ARGS__); \
	extern "C" return_type HookFuncName(__VA_ARGS__) \




#define HOOKV3(addressTo, return_type, HookFuncName, call_args,args_name,...) HOOKV3EX(addressTo,return_type,HookFuncName,call_args,args_name,__VA_ARGS__)
#define HOOKV3_EXTERN_C(addressTo, return_type, HookFuncName, call_args,args_name,...) HOOKV3EX_EXTERN_C(addressTo,return_type,HookFuncName,call_args,args_name,__VA_ARGS__)


#define INSTALL_HOOKV3(HookFuncName) \
	HookNew::CreateHook(HookFuncName,HookFuncName##MAP,HookFuncName##addressto,0,0,0,-1,false,12)


#define INSTALL_HOOKV3EX(HookFuncName,RunPosition,Block,register_to_use) \
	HookNew::CreateHook(HookFuncName,HookFuncName##MAP,HookFuncName##addressto,0,0,0,RunPosition,Block,register_to_use)





#endif // !DETOUR_H