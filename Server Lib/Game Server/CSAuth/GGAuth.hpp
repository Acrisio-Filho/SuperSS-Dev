// Arquivo GGAuth.hpp
// Criado em 11/03/2020 as 19:11 por Acrisio
// Defini��o da classe GGAuth

#pragma once
#ifndef _STDA_GGAUTH_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include "../../Projeto IOCP/UTIL/event.hpp"
#endif

#include <cstdint>

#if INTPTR_MAX == INT64_MAX
//#incldue "new_gameguard"
#elif INTPTR_MAX == INT32_MAX
#include "ggsrv26.h"
#else
#error Unknown pointer size or missing size macros!
#endif

#include "../../Projeto IOCP/TYPE/singleton.h"

namespace stdA {

#if MY_CSAuth2 == TRUE

	// My CSAuth2
	class CCSAuth2 {
		public:
			// Constructor
			CCSAuth2();

			// Destructor
			~CCSAuth2();

		public:
			GG_AUTH_DATA m_AuthQuery;
			GG_AUTH_DATA m_AuthAnswer;

		protected:
			bool m_bAuth;
			LPGGAUTH m_auth;

		public:
			void  Init();
			UINT32 GetAuthQuery();
			UINT32 CheckAuthAnswer();
			UINT32 CheckUserCSAuth(bool bCheck);
			inline void InitCSAuthState(PGG_CSAUTH_STATE m_CSAuthState) { memset(m_CSAuthState, 0, sizeof(GG_CSAUTH_STATE)); };
			void  Close();
			int	  Info(char* dest, int length); // protocol information
			int	  CheckUpdated(); //���� ������� ������Ʈ ���θ� Ȯ���Ѵ�.	

			void	AllowOldVersion();	// 2009.05.14 - �����̵� ������ ���ؼ� �������� ����Ͽ� ���� ������ ���´�.
	};

#endif

// Disable GG, por enquanto, por que ele � x86
#if INTPTR_MAX == INT64_MAX
#define GGAUTHS_API
#define	NPLOG_DEBUG	0x00000001 
#define	NPLOG_ERROR	0x00000002

	typedef void*          LPGGAUTH;

	typedef struct _GG_UPREPORT
	{
		UINT32	dwBefore;   // Before version
		UINT32	dwNext;		// Now version
		int		nType;		// Update type 1 : gameguard version, 2 : protocol num
	} GG_UPREPORT, *PGG_UPREPORT;

	// gameguard auth data
	typedef struct _GG_AUTH_DATA
	{
		UINT32 dwIndex;
		UINT32 dwValue1;
		UINT32 dwValue2;
		UINT32 dwValue3;
	} GG_AUTH_DATA, *PGG_AUTH_DATA;

	typedef struct _GG_CSAUTH_STATE
	{
		UINT32	m_PrtcVersion;
		UINT32	m_GGVersion;
		UINT32	m_UserFlag;
	} GG_CSAUTH_STATE, *PGG_CSAUTH_STATE;

	/*uint32_t InitGameguardAuth(const char* _path, uint32_t _numActiveSession, bool _check, uint32_t _flag);

	void CleanupGameguardAuth(void);

	uint32_t GGAuthUpdateTimer();*/

	// My CSAuth2
	class CCSAuth2 {
	public:
		// Constructor
		CCSAuth2();

		// Destructor
		~CCSAuth2();

	public:
		GG_AUTH_DATA m_AuthQuery;
		GG_AUTH_DATA m_AuthAnswer;

	protected:
		bool m_bAuth;

		uint32_t m_socket_id;

	public:
		void  Init(uint32_t _socket_id);
		UINT32 GetAuthQuery();
		UINT32 CheckAuthAnswer();
		UINT32 CheckUserCSAuth(bool bCheck);
		inline void InitCSAuthState(PGG_CSAUTH_STATE m_CSAuthState);
		void  Close();
		int	  Info(char* dest, int length); // protocol information
		int	  CheckUpdated();

		void	AllowOldVersion();
	};
#endif

	class GGAuth {
		public:
			GGAuth(uint32_t _numActiveSession = 1000u);
			~GGAuth();

		protected:
			static DWORD WINAPI updateTimerProc(LPVOID lpParameter);

		private:
			bool m_state;

			// Thread updateTimeProc
#if defined(_WIN32)
			HANDLE m_quit_update_timer;	// Finish;
			HANDLE m_thread_update_timer;
#elif defined(__linux__)
			Event *m_quit_update_timer;	// Finish;
			Event *m_thread_update_timer;
#endif

			const uint32_t m_time_sleep = (5000 * 60); // 5min
	};

	typedef Singleton< GGAuth > SGGAuth;
}

#endif // !_STDA_GGAUTH_HPP