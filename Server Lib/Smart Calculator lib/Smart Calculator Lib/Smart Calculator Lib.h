// O bloco ifdef a seguir é a forma padrão de criar macros que tornam a exportação
// de uma DLL mais simples. Todos os arquivos nessa DLL são compilados com SMARTCALCULATORLIB_EXPORTS
// símbolo definido na linha de comando. Esse símbolo não deve ser definido em nenhum projeto
// que usa esta DLL. Desse modo, qualquer projeto cujos arquivos de origem incluem este arquivo veem
// Funções SMARTCALCULATORLIB_API como importadas de uma DLL, enquanto esta DLL vê símbolos
// definidos com esta macro conforme são exportados.
#if defined(_WIN32)
    #ifdef SMARTCALCULATORLIB_EXPORTS
        #define SMARTCALCULATORLIB_API __declspec(dllexport)
    #else
        #define SMARTCALCULATORLIB_API __declspec(dllimport)
    #endif
#elif defined(__linux__)
    #ifdef SMARTCALCULATORLIB_EXPORTS
        // __attribute__((visibility("default")))
        #define SMARTCALCULATORLIB_API __attribute__((visibility("default")))
    #else
        // __attribute__((visibility("hidden")))
        #define SMARTCALCULATORLIB_API __attribute__((visibility("hidden")))
        #define SMARTCALCULATORLIB_API_EXP __attribute__((visibility("default")))
    #endif
#endif

#if defined(_WIN32)
#pragma pack(push, 1)
#endif

#include <string>
#include "../../Projeto IOCP/TYPE/smart_calculator_type.hpp"
#include "../../Projeto IOCP/TYPE/smart_calculator_player_base.hpp"

using namespace stdA;

// Make Player Context
extern "C" SMARTCALCULATORLIB_API stContext* makePlayerContext(uint32_t _uid, eTYPE_CALCULATOR_CMD _type);

// Get Player Context
extern "C" SMARTCALCULATORLIB_API stContext* getPlayerContext(uint32_t _uid, eTYPE_CALCULATOR_CMD _type);

// Remove All Player Context
extern "C" SMARTCALCULATORLIB_API void removeAllPlayerContext(uint32_t _uid);

// Initialize Smart Calculator Lib
extern "C" SMARTCALCULATORLIB_API int initSmartCalculatorLib(const uint32_t _server_id);

// Send Command Server
extern "C" SMARTCALCULATORLIB_API void sendCommandServer(const char* _cmd);

#ifdef SMARTCALCULATORLIB_EXPORTS
extern "C" SMARTCALCULATORLIB_API void scLog(const char* _log, const eTYPE_LOG _type);
extern "C" SMARTCALCULATORLIB_API void responseCallBack(const uint32_t _id, const eTYPE_CALCULATOR_CMD _type, const char* _response, const eTYPE_RESPONSE _server);
#else
extern "C" SMARTCALCULATORLIB_API_EXP void scLog(const char* _log, const eTYPE_LOG _type);
extern "C" SMARTCALCULATORLIB_API_EXP void responseCallBack(const uint32_t _id, const eTYPE_CALCULATOR_CMD _type, const char* _response, const eTYPE_RESPONSE _server);
#endif // SMARTCALCULATORLIB_EXPORTS

#if defined(_WIN32)
#pragma pack(pop, 1)
#endif
