// Arquivo GGAuth.cpp
// Criado em 11/03/2020 as 19:17 por Acrisio
// Implemenetação da classe GGAuth

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "GGAuth.hpp"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/UTIL/hex_util.h"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/set_se_exception_with_mini_dump.h"

#include "../Game Server/game_server.h"

#if defined(_WIN32)
#include <DbgHelp.h>
#endif

using namespace stdA;

#if defined(_WIN32)
#define BEGIN_THREAD_SETUP(_type_class) DWORD result = 0; \
						   STDA_SET_SE_EXCEPTION \
						   try { \
								_type_class *pTP = reinterpret_cast<_type_class*>(lpParameter); \
								if (pTP) {
#elif defined(__linux__)
	
	#ifdef BEGIN_THREAD_SETUP
		#undef BEGIN_THREAD_SETUP
	#endif

#define BEGIN_THREAD_SETUP(_type_class) void* result = (void*)0; \
						   try { \
								_type_class *pTP = reinterpret_cast<_type_class*>(lpParameter); \
								if (pTP) {
#endif

#if defined(__linux__)
	#ifdef END_THREAD_SETUP
		#undef END_THREAD_SETUP
	#endif
#endif

#define END_THREAD_SETUP(name_thread)	  } \
							}catch (exception& e) { \
								_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE)); \
							}catch (std::exception& e) { \
								_smp::message_pool::getInstance().push(new message(e.what(), CL_FILE_LOG_AND_CONSOLE)); \
							}catch (...) { \
								_smp::message_pool::getInstance().push(new message(std::string((name_thread)) + " -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n", CL_FILE_LOG_AND_CONSOLE)); \
							} \
							_smp::message_pool::getInstance().push(new message("Saindo do trabalho->" + std::string((name_thread)))); \
						return result; \

#if defined(_WIN32)
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif

#include <cstdint>

#if INTPTR_MAX == INT64_MAX
//#pragma comment(lib, "../CSAuth/ggsrvlib26_MT.lib") // New Lib
#elif INTPTR_MAX == INT32_MAX
#pragma comment(lib, "../CSAuth/ggsrvlib26_MT.lib")
#else
#error Unknown pointer size or missing size macros!
#endif

#if INTPTR_MAX == INT64_MAX

// My CCSAuth2
CCSAuth2::CCSAuth2() : m_bAuth(false) {
}

CCSAuth2::~CCSAuth2() {

	// Close User
	Close();
}

void CCSAuth2::Init(uint32_t _socket_id) {

	if (m_bAuth)
		Close();

	m_socket_id = _socket_id;

	auto error = sgs::gs::getInstance().ggAuthCreateUser(m_socket_id);

	if (error != ERROR_SUCCESS)
		_smp::message_pool::getInstance().push(new message("[CCSAuth2::Init][Error] Code: " + std::to_string(error), CL_FILE_LOG_AND_CONSOLE));
	else
		m_bAuth = true;	// Success
}

inline void CCSAuth2::InitCSAuthState(PGG_CSAUTH_STATE m_CSAuthState) {
	memset(m_CSAuthState, 0, sizeof(GG_CSAUTH_STATE));
};

void CCSAuth2::Close() {

	if (m_bAuth)
		sgs::gs::getInstance().ggAuthDeleteUser(m_socket_id);

	m_bAuth = false;

	m_socket_id = (uint32_t)~0u;
}

UINT32 CCSAuth2::GetAuthQuery() {

	UINT32 error = 1234;

	if (m_bAuth) {

		if ((error = sgs::gs::getInstance().ggAuthGetQuery(m_socket_id)) != ERROR_SUCCESS)
			Close();
	}

	return error;
}

UINT32 CCSAuth2::CheckAuthAnswer() {

	UINT32 error = 1234;

	if (m_bAuth) {

		if ((error = sgs::gs::getInstance().ggAuthCheckAnswer(m_socket_id, m_AuthAnswer)) != ERROR_SUCCESS)
			Close();
	}

	return error;
}

UINT32 CCSAuth2::CheckUserCSAuth(bool bCheck) {

	UINT32 error = 1234;

	/*if (m_bAuth) {

		if ((error = GGAuthCheckUserCSAuth(m_auth, bCheck)) != ERROR_SUCCESS)
			Close();
	}*/

	return error;
}

int CCSAuth2::Info(char* dest, int length) {

	int error = 1234;

	/*if (m_bAuth) {

		int error = GGAuthUserInfo(m_auth, dest, length);

		if (error != ERROR_SUCCESS)
			Close();
	}*/

	return error;
}

int CCSAuth2::CheckUpdated() {

	int error = 1234;

	if (m_bAuth) {

		/*if ((error = GGAuthCheckUpdated(m_auth)) != ERROR_SUCCESS)
			Close();*/
	}

	return error;
}

void CCSAuth2::AllowOldVersion() {

	/*if (m_bAuth)
		GGAuthAllowOldVersion(m_auth);*/
}
#elif INTPTR_MAX == INT32_MAX
GGAUTHS_API void NpLog(int mode, char* msg) // referenced by
{
	if (mode & (NPLOG_DEBUG | NPLOG_ERROR)) // for examples 
		_smp::message_pool::getInstance().push(new message(std::string("[game_server::NpLog][Log] ") + msg, CL_FILE_LOG_AND_CONSOLE));
};

GGAUTHS_API void GGAuthUpdateCallback(PGG_UPREPORT report) // referenced by 
{
	char s[2048];

	sprintf_s(s, 2048, "GGAuth version update [%s] : [%ld] -> [%ld] \n", // for examples
		report->nType == 1 ? "GameGuard Ver" : "Protocol Num",
		report->dwBefore,
		report->dwNext);

	_smp::message_pool::getInstance().push(new message(std::string("[game_server::GGAuthUpdateCallback][Log] ") + s, CL_FILE_LOG_AND_CONSOLE));
};

GGAuth::GGAuth(uint32_t _numActiveSession) : m_state(false), m_quit_update_timer(INVALID_HANDLE_VALUE), m_thread_update_timer(INVALID_HANDLE_VALUE) {

	DWORD err = InitGameguardAuth(NULL, _numActiveSession, true, 0x03);

	if (err != ERROR_SUCCESS) {

		_smp::message_pool::getInstance().push(new message("[GGAuth::GGAuth][Error] Nao conseguiu inicializar GameGuard Auth. Error Code: " + std::to_string(err), CL_FILE_LOG_AND_CONSOLE));

		return;

	}
	else
		_smp::message_pool::getInstance().push(new message("[GGAuth::GGAuth][Log] Inicializou o InitGameguardAuth com sucesso.", CL_FILE_LOG_AND_CONSOLE));

	// Create Event
	if ((m_quit_update_timer = CreateEvent(NULL, TRUE, FALSE, NULL)) == INVALID_HANDLE_VALUE || m_quit_update_timer == NULL) {

		_smp::message_pool::getInstance().push(new message("[GGAuth::GGAuth][Error] Nao conseguiu criar o evento do GGAuthUpdateTimer. Error Code: " + std::to_string(GetLastError()), CL_FILE_LOG_AND_CONSOLE));

		return;
	}

	// Init Thread UpdateTimer GG
	if ((m_thread_update_timer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GGAuth::updateTimerProc, this, 0, &err/*Thread ID*/)) == INVALID_HANDLE_VALUE || m_thread_update_timer == NULL) {

		_smp::message_pool::getInstance().push(new message("[GGAuth::GGAuth][Error] Nao conseguiu criar a thread do GGAuthUpdateTimer. Error Code: " + std::to_string(GetLastError()), CL_FILE_LOG_AND_CONSOLE));

		return;

	}
	else
		_smp::message_pool::getInstance().push(new message("[GGAuth::GGAuth][Log] Criou thread GGAuthUpdateTimer com sucesso. ID=" + std::to_string(err), CL_FILE_LOG_AND_CONSOLE));

	m_state = true;
}

GGAuth::~GGAuth() {

	if (m_thread_update_timer != INVALID_HANDLE_VALUE && m_thread_update_timer != NULL) {

		// Termina thread
		if (m_quit_update_timer != INVALID_HANDLE_VALUE && m_quit_update_timer != NULL) {

			// Termina thread pelo event
			SetEvent(m_quit_update_timer);

		}
		else // Termina for�ado
			TerminateThread(m_thread_update_timer, -32);

		if (WaitForSingleObject(m_thread_update_timer, INFINITE) != WAIT_OBJECT_0)
			_smp::message_pool::getInstance().push(new message("[GGAuth::~GGAuth][Error] Nao conseguiu esperar pela thread update timer terminar. Error Code: " + std::to_string(GetLastError()), CL_FILE_LOG_AND_CONSOLE));

		CloseHandle(m_thread_update_timer);

		m_thread_update_timer = INVALID_HANDLE_VALUE;
	}

	CleanupGameguardAuth();

	m_state = false;
}

DWORD WINAPI GGAuth::updateTimerProc(LPVOID lpParameter) {
	BEGIN_THREAD_SETUP(GGAuth);

	DWORD err = 0;

	if (!pTP->m_state) {

		_smp::message_pool::getInstance().push(new message("[GGAuth::updateTimerProc][Error] GGAuth is not initialized.", CL_FILE_LOG_AND_CONSOLE));

		return -2;
	}

	while ((err = WaitForSingleObject(pTP->m_quit_update_timer, pTP->m_time_sleep)) == WAIT_TIMEOUT) {

		// GGAuthUpdateTimer
		if ((err = GGAuthUpdateTimer()) != ERROR_SUCCESS)
			_smp::message_pool::getInstance().push(new message("[GGAuth::updateTimerProc][Error] Nao conseguiu atualizar o GGAuthUpdateTimer. Error Code: " + std::to_string(err), CL_FILE_LOG_AND_CONSOLE));
	}

	_smp::message_pool::getInstance().push(new message("[GGAuth::updateTimerProc][Log] Finish Loop code: " + std::to_string(err) + (err != WAIT_OBJECT_0 ? std::string(", Win Error: ") + std::to_string(GetLastError()) : ""), CL_FILE_LOG_AND_CONSOLE));

	END_THREAD_SETUP("updateTimerProc()");
}
#else
#error Unknown pointer size or missing size macros!
#endif

#if MY_CSAuth2 == TRUE

// My CCSAuth2
CCSAuth2::CCSAuth2() : m_auth(nullptr), m_bAuth(false) {

	// Create User
	m_auth = GGAuthCreateUser();

	// Init User
	Init();
}

CCSAuth2::~CCSAuth2() {

	// Close User
	Close();

	// Destroy User
	if (m_auth != nullptr)
		GGAuthDeleteUser(m_auth);

	m_auth = nullptr;
}

void CCSAuth2::Init() {

	if (m_auth != nullptr && !m_bAuth) {

		auto error = GGAuthInitUser(m_auth);

		if (error != ERROR_SUCCESS)
			_smp::message_pool::getInstance().push(new message("[CCSAuth2::Init][Error] Code: " + std::to_string(error), CL_FILE_LOG_AND_CONSOLE));
		else
			m_bAuth = true;	// Success
	}
}

void CCSAuth2::Close() {

	if (m_bAuth && m_auth != nullptr)
		GGAuthCloseUser(m_auth);

	m_bAuth = false;
}

UINT32 CCSAuth2::GetAuthQuery() {

	UINT32 error = 1234;

	if (!m_bAuth)
		Init();

	if (m_auth != nullptr && m_bAuth) {
		
		if ((error = GGAuthGetQuery(m_auth, &m_AuthQuery)) != ERROR_SUCCESS)
			Close();
		else {

			_GG_AUTH_PROTOCOL *prol = (_GG_AUTH_PROTOCOL*)new unsigned char[1024];

			auto ee = Info((char*)prol, 1024);

			_smp::message_pool::getInstance().push(new message("[CCSAuth2::GetAuthQuery][Log] ee: " + std::to_string(ee) + ", PROLTOCOL: " + hex_util::BufferToHexString((unsigned char*)prol, 1024), CL_FILE_LOG_AND_CONSOLE));
		}
	}

	return error;
}

UINT32 CCSAuth2::CheckAuthAnswer() {

	UINT32 error = 1234;

	if (!m_bAuth)
		Init();

	if (m_auth != nullptr && m_bAuth) {
		
		if ((error = GGAuthCheckAnswer(m_auth, &m_AuthAnswer)) != ERROR_SUCCESS)
			Close();
	}

	return error;
}

UINT32 CCSAuth2::CheckUserCSAuth(bool bCheck) {
	
	UINT32 error = 1234;

	if (!m_bAuth)
		Init();

	if (m_auth != nullptr && m_bAuth) {
		
		if ((error = GGAuthCheckUserCSAuth(m_auth, bCheck)) != ERROR_SUCCESS)
			Close();
	}

	return error;
}

int CCSAuth2::Info(char* dest, int length) {

	int error = 1234;

	if (!m_bAuth)
		Init();
	
	if (m_auth != nullptr && m_bAuth) {
		
		int error = GGAuthUserInfo(m_auth, dest, length);

		if (error != ERROR_SUCCESS)
			Close();
	}

	return error;
}

int CCSAuth2::CheckUpdated() {

	int error = 1234;

	if (m_auth != nullptr && m_bAuth) {
		
		if ((error = GGAuthCheckUpdated(m_auth)) != ERROR_SUCCESS)
			Close();
	}

	return error;
}

void CCSAuth2::AllowOldVersion() {

	if (!m_bAuth)
		Init();

	if (m_auth != nullptr && m_bAuth)
		GGAuthAllowOldVersion(m_auth);
}

#endif
