//-----------------------------------------------------------------------------
//  DLLModule.cpp
//
// Demonstrates the use of Dynamic Link Library (aka DLL) by
// Using the Load/FreeLibrary API.
//
//
// Xbox Advanced Technology Group.
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <xtl.h>
#include <assert.h>
#include "Core/2006DLL.h"

//-----------------------------------------------------------------------------
// Name: DllMain()
// Desc: The function which is called at initialization/termination of the process
//       and thread and whenever LoadLibrary() or FreeLibrary() are called.
//       The DllMain routine should not be used to load another module or call a
//       routine that may block.
//-----------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		STH2006DLLMain();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;

    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// Desc: Export a global constant variable
//-----------------------------------------------------------------------------
extern "C" const TCHAR szModuleName[] = TEXT("DLLModule.dll");

//-----------------------------------------------------------------------------
// Name: Function1()
// Desc: Export function
//-----------------------------------------------------------------------------
