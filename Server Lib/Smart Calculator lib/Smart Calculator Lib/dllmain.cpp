// dllmain.cpp : Define o ponto de entrada para o aplicativo DLL.

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include "pch.h"
#include <Windows.h>
#endif

#include <iostream>

#include "../SOCKET/pool.hpp"

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
		  break;
    case DLL_THREAD_DETACH:
		  break;
    case DLL_PROCESS_DETACH:
		stdA::sPool::getInstance().close();
        break;
    }
    return TRUE;
}
#elif defined(__linux__)
__attribute__((destructor)) void fini() {
	stdA::sPool::getInstance().close();
}
#endif

