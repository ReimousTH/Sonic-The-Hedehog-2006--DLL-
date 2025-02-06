//
// Made by iMoD1998	
// V3.1
//

#ifndef DETOUR_H
#define DETOUR_H

#include <xtl.h>
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
#define POWERPC_OPCODE_LD        POWERPC_OPCODE( 58 )
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
#define POWERPC_ADDI(rD, rA, SIMM)  (UINT32)( POWERPC_OPCODE_ADDI | ( rD << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | SIMM )
#define POWERPC_ADDIS(rD, rA, SIMM) (UINT32)( POWERPC_OPCODE_ADDIS | ( rD << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | SIMM )
#define POWERPC_LIS(rD, SIMM)       POWERPC_ADDIS( rD, 0, SIMM ) // Mnemonic for addis %rD, 0, SIMM
#define POWERPC_LWZ(rS, DS, rA)      (UINT32)( POWERPC_OPCODE_LWZ | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | ( (INT16)DS & 0xFFFF ) )
#define POWERPC_LI(rD, SIMM)        POWERPC_ADDI( rD, 0, SIMM )  // Mnemonic for addi %rD, 0, SIMM
#define POWERPC_MTSPR(SPR, rS)      (UINT32)( POWERPC_OPCODE_EXTENDED | ( rS << POWERPC_BIT32( 10 ) ) | ( POWERPC_SPR( SPR ) << POWERPC_BIT32( 20 ) ) | POWERPC_EXOPCODE_MTSPR )
#define POWERPC_MTCTR(rS)           POWERPC_MTSPR( 9, rS ) // Mnemonic for mtspr 9, rS
#define POWERPC_ORI(rS, rA, UIMM)   (UINT32)( POWERPC_OPCODE_ORI | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | UIMM )
#define POWERPC_BCCTR(BO, BI, LK)   (UINT32)( POWERPC_OPCODE_BCCTR | ( BO << POWERPC_BIT32( 10 ) ) | ( BI << POWERPC_BIT32( 15 ) ) | ( LK & 1 ) | POWERPC_EXOPCODE_BCCTR )
#define POWERPC_STD(rS, DS, rA)     (UINT32)( POWERPC_OPCODE_STD | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | ( (INT16)DS & 0xFFFF ) )
#define POWERPC_STW(rS, DS, rA)     (UINT32)( POWERPC_OPCODE_STW | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | ( (INT16)DS & 0xFFFF ) )
#define POWERPC_LD(rS, DS, rA)      (UINT32)( POWERPC_OPCODE_LD | ( rS << POWERPC_BIT32( 10 ) ) | ( rA << POWERPC_BIT32( 15 ) ) | ( (INT16)DS & 0xFFFF ) )

//
// Branch related fields.
//
#define POWERPC_BRANCH_LINKED    1
#define POWERPC_BRANCH_ABSOLUTE  2
#define POWERPC_BRANCH_TYPE_MASK ( POWERPC_BRANCH_LINKED | POWERPC_BRANCH_ABSOLUTE )

#define POWERPC_BRANCH_OPTIONS_ALWAYS ( 20 )







#define HOOKADDR(address) HookNew::UseEmulatedAddress == true ? (void*)(0x10000000 + (DWORD)address) : (void*)(0x10000000 + (DWORD)address)

class HookNew {
public:
	static std::map<void*, std::vector<HookNew*>>* SaveBuffer; 
	static int SavedBufferSIZE;

	void* OriginalFunc;
	void* TargetFunc;
	char RunPosition; // -1 = Before, 1 = After
	bool Block;

	// Function to create a hook
	static void CreateHook(void* Target, void* TargetMap, void* Original,char RunPosition,bool block);
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


/*

*/


#define HOOKV3(addressTo, return_type, HookFuncName, call_args,args_name,...) \
	void* HookFuncName##addressto = (void*)addressTo; \
	return_type HookFuncName##MAP(__VA_ARGS__) { \
	return_type return_value = (return_type)0; \
	if (HookNew::SaveBuffer && HookNew::SaveBuffer->find((void*)addressTo) != HookNew::SaveBuffer->end()){\
		\
		bool block = false; \
		for (std::vector<HookNew*>::iterator it =(*HookNew::SaveBuffer)[(void*)addressTo].begin() ; it != (*HookNew::SaveBuffer)[(void*)addressTo].end(); ++it) { \
			if ((*it)->RunPosition == -1){\
				block |= (*it)->Block;\
				void* tar = (*it)->TargetFunc; \
				return_value = ((return_type (__fastcall *)call_args)tar)args_name; \
			}\
		}\
		void* jump_to = (*HookNew::SaveBuffer)[(void*)addressTo][0]->OriginalFunc; \
		if (!block)  return_value =  ((return_type (__fastcall *)call_args)(jump_to))args_name; \
		\
		for (std::vector<HookNew*>::iterator it =(*HookNew::SaveBuffer)[(void*)addressTo].begin() ; it != (*HookNew::SaveBuffer)[(void*)addressTo].end(); ++it) { \
			if ((*it)->RunPosition == 1){\
				void* tar = (*it)->TargetFunc; \
				return_value = ((return_type (__fastcall *)call_args)tar)args_name; \
			}\
		}\
		return return_value; \
\
	}\
	return return_value ;\
}\
	return_type HookFuncName(__VA_ARGS__) \

#define INSTALL_HOOKV3(HookFuncName) \
	HookNew::CreateHook(HookFuncName,HookFuncName##MAP,HookFuncName##addressto,-1,false)


#define INSTALL_HOOKV3EX(HookFuncName,RunPosition,Block) \
	HookNew::CreateHook(HookFuncName,HookFuncName##MAP,HookFuncName##addressto,RunPosition,Block)





#endif // !DETOUR_H