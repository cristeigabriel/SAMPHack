// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Init.h"

// Built for samp.dll   5BA5F0BE7AF99DFD03FB39E88A970A2B MD5
// Built for gta_sa.exe 170B3A9108687B26DA2D8901C6948A18 MD5
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Core::Init,
            nullptr, 0, nullptr);
        if (!hThread)
            return FALSE;

        CloseHandle(hThread);
    } break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

