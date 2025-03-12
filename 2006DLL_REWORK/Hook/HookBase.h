

#ifndef HOOK_BASE
#define HOOK_BASE

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




#pragma endregion Definitions



#endif

