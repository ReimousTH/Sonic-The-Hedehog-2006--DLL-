#include "HookNew.h"





std::map<void*, std::vector<HookNew*>>* HookNew::SaveBuffer;

int HookNew::SavedBufferSIZE = 0;


void HookNew::memcpy(void * _Dst,const void * _Src,size_t _Size){

	DWORD OPrct;//ProtectionFlag
	VirtualProtectEx(GetModuleHandle(NULL),	HOOKADDR(_Dst),_Size,4,&OPrct); 
	std::memcpy(HOOKADDR(_Dst),_Src,_Size);
	VirtualProtectEx(GetModuleHandle(NULL),HOOKADDR(_Dst),_Size,OPrct,&OPrct);
}

HookNew* HookNew::CreateHook(void* Target, void* TargetMap, void* Original,char fp13,int fp13_number,lua_State* fp13_state,char RunPosition,bool block,int register_choose) {


	const DWORD Buffer = reinterpret_cast<DWORD>(HookNew::SavedBufferFunc);
	const DWORD BufferSize = HookNew::SavedBufferSIZE;
	const DWORD BufferPoint = Buffer + BufferSize;
	DWORD GenCopyInCount = 4;
	if (fp13){
		GenCopyInCount = GenCopyInCount + 6;
	}
	DWORD GenFuncSize = (GenCopyInCount * 0x4);


	HookNew* hook = new HookNew();
	hook->OriginalFunc = reinterpret_cast<void*>(BufferPoint);
	hook->TargetFunc = Target;
	hook->RunPosition = RunPosition;
	hook->Block = block;
	hook->L = fp13_state;
	hook->lua_function =fp13_number; 


	// Check if the original function is already hooked
	if (HookNew::SaveBuffer->find(Original) == HookNew::SaveBuffer->end()) {
		HookNew::SaveBuffer->insert(std::make_pair(Original, std::vector<HookNew*>()));

		DWORD BranchBuffer[4];
		int StartPos = BufferPoint; // Start position for copying instructions
		int OriginalPos = reinterpret_cast<int>(Original); // Pointer to original function

		// Copy instructions from the original function
		for (int i = 0; i < GenCopyInCount; ++i) {
			DWORD Instruction = *reinterpret_cast<int*>(OriginalPos);

			// Check if it's a branch instruction
			if ((Instruction & 0xFC000000) == POWERPC_OPCODE_B) {
				char linked = (Instruction & 1);
				INT32 BranchOffset = (Instruction & (MASK_N_BITS(24) << 2));

				// Sign-extend the branch offset if necessary
				if (BranchOffset >> (26 - 1)) {
					BranchOffset |= ~MASK_N_BITS(26);
				}
				int address_to = OriginalPos + BranchOffset;

				// Prepare branch instructions
				BranchBuffer[0] = POWERPC_LIS(register_choose, POWERPC_HI(address_to));
				BranchBuffer[1] = POWERPC_ORI(register_choose, register_choose, POWERPC_LO(address_to));
				BranchBuffer[2] = POWERPC_MTCTR(register_choose);
				BranchBuffer[3] = 0x4E800420 | linked;

				

		
				memcpy((void*)(StartPos), BranchBuffer, sizeof(BranchBuffer)); // Copy data to hook address
				StartPos += sizeof(BranchBuffer);
			} else {
				memcpy((void*)(StartPos), &Instruction, sizeof(Instruction)); // Copy data to hook address
				StartPos += sizeof(Instruction);
			}
			OriginalPos += 4; // Move to the next instruction
		}

		// Finalize the branch to the target function
		BranchBuffer[0] = POWERPC_LIS(register_choose, POWERPC_HI(reinterpret_cast<DWORD>(Original) + GenFuncSize));
		BranchBuffer[1] = POWERPC_ORI(register_choose, register_choose, POWERPC_LO(reinterpret_cast<DWORD>(Original) + GenFuncSize));
		BranchBuffer[2] = POWERPC_MTCTR(register_choose);
		BranchBuffer[3] = 0x4E800420;

		memcpy((void*)(StartPos), BranchBuffer, sizeof(BranchBuffer)); // Final copy for target
		StartPos += sizeof(BranchBuffer);

		// Update saved buffer size
		HookNew::SavedBufferSIZE += (StartPos - BufferPoint);

		// Replace original function with branch to target map function



		int MoveOriginal = (int)Original;

		if (fp13){

			//-----------------------------------IN WORK----------------------------------------(FP13)
			DWORD SAVE_ADDRESS_BUFFER[6];
			SAVE_ADDRESS_BUFFER[0] = POWERPC_STWU(1,-0x8,1);
			SAVE_ADDRESS_BUFFER[1] = POWERPC_LIS(register_choose, POWERPC_HI(reinterpret_cast<DWORD>(Original)));
			SAVE_ADDRESS_BUFFER[2] = POWERPC_ORI(register_choose, register_choose, POWERPC_LO(reinterpret_cast<DWORD>(Original)));
			SAVE_ADDRESS_BUFFER[3] = POWERPC_STW(register_choose,4,1);
			SAVE_ADDRESS_BUFFER[4] = POWERPC_LFS(13,0,1); //lfs f13,0(r1) 
			SAVE_ADDRESS_BUFFER[5] = POWERPC_ADDI(1,1,8); 
			memcpy((void*)(MoveOriginal),SAVE_ADDRESS_BUFFER,sizeof(SAVE_ADDRESS_BUFFER) );
			MoveOriginal+= sizeof(SAVE_ADDRESS_BUFFER);
			//-----------------------------------------------------------------------------------------
		}

		BranchBuffer[0] = POWERPC_LIS(register_choose, POWERPC_HI(reinterpret_cast<DWORD>(TargetMap)));
		BranchBuffer[1] = POWERPC_ORI(register_choose, register_choose, POWERPC_LO(reinterpret_cast<DWORD>(TargetMap)));
		BranchBuffer[2] = POWERPC_MTCTR(register_choose);
		BranchBuffer[3] = 0x4E800420;

		memcpy((void*)(MoveOriginal), BranchBuffer, sizeof(BranchBuffer)); // Final copy for original hook
	}

	(*HookNew::SaveBuffer)[Original].push_back(hook);

	return hook;

	// Optionally flush instruction cache if necessary
}

bool HookNew::IsEmulated()
{
	MEMORY_BASIC_INFORMATION data;
	SIZE_T result = VirtualQuery((void*)0x92000600, &data, sizeof(data));

	// Check if VirtualQuery succeeded
	if (result == 0) {
		// Handle error (e.g., log it or set a flag)
		UseEmulatedAddress = false;
		return false;
	}

	// Check if the memory is protected (you can adjust the condition as needed)
	if (data.Protect & (PAGE_NOACCESS | PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)) {
		UseEmulatedAddress = true;
		return true;
	}

	UseEmulatedAddress = false;
	return false;
}

bool HookNew::UseEmulatedAddress;

#define EMPTY_SPAM_10 \
	__asm{blr};\
	__asm{blr};\
	__asm{blr};\
	__asm{blr};\
	__asm{blr};\
	__asm{blr};\
	__asm{blr};\
	__asm{blr};\
	__asm{blr};\
	__asm{blr};\

#define EMPTY_SPAM_20 \
	EMPTY_SPAM_10 \
	EMPTY_SPAM_10 \


#define EMPTY_SPAM_40 \
	EMPTY_SPAM_20 \
	EMPTY_SPAM_20 \

#define EMPTY_SPAM_100 \
	EMPTY_SPAM_40 \
	EMPTY_SPAM_40 \
	EMPTY_SPAM_20 \

#define EMPTY_SPAM_200 \
	EMPTY_SPAM_100 \
	EMPTY_SPAM_100 \

#define EMPTY_SPAM_400 \
	EMPTY_SPAM_200 \
	EMPTY_SPAM_200 \


#define EMPTY_SPAM_800 \
	EMPTY_SPAM_400 \
	EMPTY_SPAM_400

#define EMPTY_SPAM_1600 \
	EMPTY_SPAM_800 \
	EMPTY_SPAM_800 \

#define EMPTY_SPAM_3200 \
	EMPTY_SPAM_1600 \
	EMPTY_SPAM_1600 \

#define EMPTY_SPAM_6400 \
	EMPTY_SPAM_3200 \
	EMPTY_SPAM_3200 \

#define EMPTY_SPAM_12800 \
	EMPTY_SPAM_6400 \
	EMPTY_SPAM_6400 \

#define EMPTY_SPAM_25600 \
	EMPTY_SPAM_12800 \
	EMPTY_SPAM_12800 \

#define EMPTY_SPAM_51200 \
	EMPTY_SPAM_25600 \
	EMPTY_SPAM_25600 \






#pragma optimize("", off) // 
__declspec(naked) void HookNew::SavedBufferFunc() { EMPTY_SPAM_6400}
__declspec(naked) void HookNew::SavedBufferFunc2() { EMPTY_SPAM_6400}
__declspec(naked) void HookNew::SavedBufferFunc3() { EMPTY_SPAM_6400}
__declspec(naked) void HookNew::SavedBufferFunc4() { EMPTY_SPAM_6400}
__declspec(naked) void HookNew::SavedBufferFunc5() { EMPTY_SPAM_6400}
__declspec(naked) void HookNew::SavedBufferFunc6() { EMPTY_SPAM_6400}
__declspec(naked) void HookNew::SavedBufferFunc7() { EMPTY_SPAM_6400}
__declspec(naked) void HookNew::SavedBufferFunc8() { EMPTY_SPAM_6400}
#pragma optimize("", on) // Re-enable optimizations

extern "C" void CallLuaFunc()
{

	DWORD r3x,r4x,r5x,r6x,r7x,r8x,r9x,r10x;
	__asm{mr r3x,r3};
	__asm{mr r4x,r4};
	__asm{mr r5x,r5};
	__asm{mr r6x,r6};
	__asm{mr r7x,r7};
	__asm{mr r8x,r8};
	__asm{mr r9x,r9};
	__asm{mr r10x,r10};



	float f1 = 0.0f;
	__asm{fmr f1,fp13};
	HookNew* hook  = (HookNew*)*(DWORD*)&f1;
	lua_rawgeti06(hook->L,LUA_REGISTRYINDEX,hook->lua_function);
	
	lua_newtable06(hook->L);

	lua_pushstring06(hook->L,"r3");
	lua_pushlightuserdata(hook->L,(void*)r3x);
	lua_settable06(hook->L,-3);


	int status = lua_pcall(hook->L, 1, 1, 0);
	lua_pop(hook->L, 1); // Remove result from stack


	return; // Return the result
}
