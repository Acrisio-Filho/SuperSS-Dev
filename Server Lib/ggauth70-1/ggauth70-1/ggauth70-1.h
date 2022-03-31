// O bloco ifdef a seguir é a forma padrão de criar macros que tornam a exportação
// de uma DLL mais simples. Todos os arquivos nessa DLL são compilados com GGAUTH701_EXPORTS
// símbolo definido na linha de comando. Esse símbolo não deve ser definido em nenhum projeto
// que usa esta DLL. Desse modo, qualquer projeto cujos arquivos de origem incluem este arquivo veem
// Funções GGAUTH701_API como importadas de uma DLL, enquanto esta DLL vê símbolos
// definidos com esta macro conforme são exportados.
#if defined(_WIN32)
    #ifdef GGAUTH701_EXPORTS
        #define GGAUTH701_API __declspec(dllexport)
    #else
        #define GGAUTH701_API __declspec(dllimport)
    #endif
#elif defined(__linux__)
    #ifdef GGAUTH701_EXPORTS
        #define GGAUTH701_API __attribute__((visibility("default")))
    #else
        #define GGAUTH701_API __attribute__((visibility("hidden")))
    #endif
#endif

#if defined(_WIN32)
#include <Windows.h>
#endif

#include "../TYPE/ggauthprotocol_type.hpp"

extern "C" GGAUTH701_API UINT32 PrtcCheckAuthAnswer(UINT32 *_pUserFlag, GG_VERSION *_pGGVer, GG_AUTH_DATA *_pAuthAnswer, GG_AUTH_DATA *_pAuthChecked, UINT32 _nSequenceNum, UINT32 *_pLastValue4);
extern "C" GGAUTH701_API UINT32 PrtcCheckValidAnswer(GG_AUTH_DATA *_pAuthAnswer, UINT32 *_pReturnFlag);
extern "C" GGAUTH701_API void   PrtcDecryptAnswer(GG_AUTH_DATA *_pAuthAnswer);
extern "C" GGAUTH701_API UINT32 PrtcEncryptQuery(GG_AUTH_DATA *_pAuthQuery);
extern "C" GGAUTH701_API UINT32 PrtcGetAuthQuery(UINT32 *_pUserFlag, GG_VERSION *_pGGVer, CCSAuth2 *_pCCSAuth2, DWORD _nSequenceNum, DWORD _dwServerKey);
extern "C" GGAUTH701_API UINT32 PrtcGetInformation(UINT32 _type, GG_AUTH_DATA *_pAuthAnswer);
extern "C" GGAUTH701_API UINT32 PrtcGetVersion(BOOL _bUseTimer);
extern "C" GGAUTH701_API UINT32 PrtcLoopAuth(char* _pByLastLoop1, DWORD *_pdwLoop1AuthArray, char* _pByLastLoop2, DWORD *_pdwLoop2AuthArray, GG_AUTH_DATA *_pAuthAnswer, UINT32 _nSequenceNum);
extern "C" GGAUTH701_API UINT32 PrtcUpdateTimer(void);
