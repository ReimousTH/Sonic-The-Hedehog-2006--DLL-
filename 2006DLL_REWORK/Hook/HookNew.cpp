#include "HookNew.h"





std::map<void*, std::vector<HookNew*>>* HookNew::SaveBuffer;

int HookNew::SavedBufferSIZE = 0;


void HookNew::memcpy(void * _Dst,const void * _Src,size_t _Size){

	DWORD OPrct;//ProtectionFlag
	VirtualProtectEx(GetModuleHandle(NULL),	HOOKADDR(_Dst),_Size,4,&OPrct); 
	std::memcpy(HOOKADDR(_Dst),_Src,_Size);
	VirtualProtectEx(GetModuleHandle(NULL),HOOKADDR(_Dst),_Size,OPrct,&OPrct);
}

void HookNew::CreateHook(void* Target, void* TargetMap, void* Original,char RunPosition,bool block) {


	const DWORD Buffer = reinterpret_cast<DWORD>(HookNew::SavedBufferFunc);
	const DWORD BufferSize = HookNew::SavedBufferSIZE;
	const DWORD BufferPoint = Buffer + BufferSize;

	HookNew* hook = new HookNew();
	hook->OriginalFunc = reinterpret_cast<void*>(BufferPoint);
	hook->TargetFunc = Target;
	hook->RunPosition = RunPosition;
	hook->Block = block;

	// Check if the original function is already hooked
	if (HookNew::SaveBuffer->find(Original) == HookNew::SaveBuffer->end()) {
		HookNew::SaveBuffer->insert(std::make_pair(Original, std::vector<HookNew*>()));

		DWORD BranchBuffer[4];
		int StartPos = BufferPoint; // Start position for copying instructions
		int OriginalPos = reinterpret_cast<int>(Original); // Pointer to original function

		// Copy instructions from the original function
		for (int i = 0; i < 4; ++i) {
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
				BranchBuffer[0] = POWERPC_LIS(12, POWERPC_HI(address_to));
				BranchBuffer[1] = POWERPC_ORI(12, 12, POWERPC_LO(address_to));
				BranchBuffer[2] = POWERPC_MTCTR(12);
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
		BranchBuffer[0] = POWERPC_LIS(12, POWERPC_HI(reinterpret_cast<DWORD>(Original) + 0x10));
		BranchBuffer[1] = POWERPC_ORI(12, 12, POWERPC_LO(reinterpret_cast<DWORD>(Original) + 0x10));
		BranchBuffer[2] = POWERPC_MTCTR(12);
		BranchBuffer[3] = 0x4E800420;

		memcpy((void*)(StartPos), BranchBuffer, sizeof(BranchBuffer)); // Final copy for target
		StartPos += sizeof(BranchBuffer);

		// Update saved buffer size
		HookNew::SavedBufferSIZE += (StartPos - BufferPoint);

		// Replace original function with branch to target map function
		BranchBuffer[0] = POWERPC_LIS(12, POWERPC_HI(reinterpret_cast<DWORD>(TargetMap)));
		BranchBuffer[1] = POWERPC_ORI(12, 12, POWERPC_LO(reinterpret_cast<DWORD>(TargetMap)));
		BranchBuffer[2] = POWERPC_MTCTR(12);
		BranchBuffer[3] = 0x4E800420;

		memcpy((void*)(Original), BranchBuffer, sizeof(BranchBuffer)); // Final copy for original hook
	}

	(*HookNew::SaveBuffer)[Original].push_back(hook);

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