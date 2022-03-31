// dllmain.cpp : Define o ponto de entrada para o aplicativo DLL.
#if defined(_WIN32)
//#include "pch.h"
#include <Windows.h>
#elif defined(__linux__)
#include "Protocol70.hpp"
#endif

#if defined(_WIN32)
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#elif defined(__linux__)
__attribute__((destructor)) void fini() {
    Protocol70::destroy();
}
#endif
