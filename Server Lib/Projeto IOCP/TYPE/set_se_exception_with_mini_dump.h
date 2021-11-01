// Arquivo set_se_with_mini_dump.h
// Criado em 09/12/2019 as 02:32 por Acrisio
// Definição da função amba que set a função que captura os SE_EXCEPTION
// Gera o arquivo de dump e lança a exception com o endereço onde ocorreu a exception e o código da exception

#ifndef _STDA_SET_SE_EXCEPTION_WITH_MINI_DUMP_H
#define _STDA_SET_SE_EXCEPTION_WITH_MINI_DUMP_H

#include "../UTIL/hex_util.h"

// Definição do macro que define a função amba
#define STDA_SET_SE_EXCEPTION ::_set_se_translator([](unsigned int u, EXCEPTION_POINTERS *pExp) { \
\
								std::wstring datetime = L""; \
\
								datetime.resize(255); \
\
								tm ti{ 0 }; \
								time_t day_time; \
\
								/* Pega o time atual para construir o nome do arquivo de dump */ \
								std::time(&day_time); \
\
								/* exception dump file name with Time */ \
								if (localtime_s(&ti, &day_time) == 0) \
									wsprintf(&datetime[0], L"%.02u%.02u%.04u%.02u%.02u%.02u.dmp", ti.tm_mday, ti.tm_mon + 1, ti.tm_year + 1900, ti.tm_hour, ti.tm_min, ti.tm_sec); \
\
								datetime.resize(18); \
\
								HANDLE hFile = CreateFile(datetime.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL); \
\
								if (hFile != INVALID_HANDLE_VALUE) { \
\
									MINIDUMP_EXCEPTION_INFORMATION mei; \
\
									mei.ThreadId = GetCurrentThreadId(); \
									mei.ExceptionPointers = pExp; \
									mei.ClientPointers = TRUE; \
\
									bool sucesso = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MINIDUMP_TYPE(MINIDUMP_TYPE::MiniDumpWithFullMemoryInfo | MINIDUMP_TYPE::MiniDumpWithProcessThreadData | MINIDUMP_TYPE::MiniDumpWithThreadInfo | MINIDUMP_TYPE::MiniDumpWithDataSegs), &mei, NULL, NULL); \
\
									CloseHandle(hFile); \
\
									hFile = INVALID_HANDLE_VALUE; \
								} \
\
								throw exception("SEH Error capturado no endereco: 0x" + hex_util::lltoaToHex((int64_t)pExp->ExceptionRecord->ExceptionAddress), u); \
							});

#endif // !_STDA_SET_SE_EXCEPTION_WITH_MINI_DUMP_H