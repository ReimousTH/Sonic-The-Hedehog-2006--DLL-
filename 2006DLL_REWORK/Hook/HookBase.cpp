#include "HookBase.h"


bool HookV2::IsNotEmulatedHardWare = false;
UINT64 GetAddressADT(){
	return HookV2::IsNotEmulatedHardWare == true ? 0: 0x010000000;
}




HookV2::HookV2(void)
{
}

HookV2::~HookV2(void)
{
}

bool HookV2::CheckIsNotEmulatedHardWare()
{

	MEMORY_BASIC_INFORMATION data;
	SIZE_T result = VirtualQuery((void*)0x92000600, &data, sizeof(data));

	if (result == 0) {
		return true;
	}

	// Check if the memory is protected (you can adjust the condition as needed)
	if (data.Protect & (PAGE_NOACCESS | PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)) {
		return false;
	}


}

extern "C" HookV2::InstalHook(bool IsReturn /*= false*/,UINT32 addr,UINT32 fnc)
{

	//Preapare WriteData
	DWORD Instructions[ ARRAYSIZE( JumpASMNoPreserve ) ];
	memcpy( Instructions, JumpASMNoPreserve, sizeof( JumpASMNoPreserve ) );
	Instructions[ 0 ] |= ADDRESS_HIGHER( (UINT)fnc );
	Instructions[ 1 ] |= ADDRESS_LOWER( (UINT)fnc );
	Instructions[ 3 ] |= IsReturn == true ? 1 : 0; // bctrl, bctr





	DWORD OPrct;
	VirtualProtectEx(GetModuleHandle(NULL),GetAddress(addr),sizeof( JumpASMNoPreserve ),4,&OPrct); // Change Protection
	memcpy(GetAddress(addr),Instructions,sizeof(Instructions));
	VirtualProtectEx(GetModuleHandle(NULL),GetAddress(addr),sizeof( JumpASMNoPreserve ),OPrct,&OPrct); // Change Protection





}

HookV2::WriteWORD(UINT32 addr,WORD value)
{
	DWORD OPrct;
	VirtualProtectEx(GetModuleHandle(NULL),GetAddress(addr),2,4,&OPrct); // Change Protection
	memcpy(GetAddress(addr),&value,2); // CopyBytes
	VirtualProtectEx(GetModuleHandle(NULL),GetAddress(addr),2,OPrct,&OPrct); // Return Protect
}

HookV2::WriteDWORD(UINT32 addr,DWORD value)
{
	DWORD OPrct;
	VirtualProtectEx(GetModuleHandle(NULL),GetAddress(addr),4,4,&OPrct); // Change Protection
	memcpy(GetAddress(addr),&value,4); // CopyBytes
	VirtualProtectEx(GetModuleHandle(NULL),GetAddress(addr),4,OPrct,&OPrct); // Return Protect
}

HookV2::WriteNOP(UINT32 addr,int c)
{
	for (int i = 0;i<c;i++){
		HookV2::WriteDWORD(addr + (i*4),0x60000000);
	}

}

void* HookV2::GetAddress(UINT32 addr)
{
	return (void*)(GetAddressADT() + addr);
}
