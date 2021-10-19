// Arquivo socket.h
// Criado em 02/03/2017 por Acrisio
// Definição da classe socket

#pragma once
#ifndef _STDA_SOCKET_H
#define _STDA_SOCKET_H

#if defined(_WIN32)
	#ifndef _WINSOCK2_H
		#define _WINSOCK2_H
		#include <winsock2.h>
	#endif
#elif defined(__linux__)
	#include "../UTIL/WinPort.h"
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <pthread.h>
	#include <time.h>
	#include "../THREAD POOL/thread.h"

	#define CLOCK_SOCKET_TO_CHECK CLOCK_MONOTONIC_RAW
#endif

#include <string>
#include <vector>

namespace stdA {
	class socket {
	public:
		explicit socket(int option = 0);

		explicit socket(SOCKET socket, int option = 0);

		~socket();

		SOCKET detatch();

		void attach(SOCKET socket);

		void close();

		void abortiveClose();

		void shutdown(int como);

		void listen(int max_listen_same_time);

		void bind(const SOCKADDR_IN &address);

		void connect(std::string host, size_t port);

		void connect(const SOCKADDR_IN* _s_addr);

	private:
#if defined(_WIN32)
#define DIFF_TICK(a, b, c) (INT64)(((INT64)((a.QuadPart) - (b.QuadPart)) * 1000000 / (c.QuadPart)) / 1000)
#endif

		typedef struct _connection_history {
			void clear() { memset(this, 0, sizeof(_connection_history)); };
			_connection_history* next;
			UINT32 ip;
#if defined(_WIN32)
			LARGE_INTEGER tick;
#elif defined(__linux__)
			timespec tick;
#endif
			UINT32 count;
			unsigned ddos : 1;
		}ConnectHistory, *PConnectHistory;

		typedef struct _connection_live {
			void clear() {
				memset(this, 0, sizeof(_connection_live));
#if defined(_WIN32)
				sock = INVALID_SOCKET;
#elif defined(__linux__)
				sock.fd = INVALID_SOCKET;
				sock.connect_time.tv_sec = 0;
				sock.connect_time.tv_nsec = 0;
#endif
			};
			_connection_live* next;
			UINT32 ip;
#if defined(_WIN32)
			LARGE_INTEGER tick;
#elif defined(__linux__)
			timespec tick;
#endif
			SOCKET sock;
		}ConnectLive, *PConnectLive;

		typedef struct _access_control {
			UINT32 ip;
			UINT32 mask;
		}AccessControl, *PAccessControl;

		enum _aco {
			ACO_DENY_ALLOW,
			ACO_ALLOW_DENY,
			ACO_MUTUAL_FAILURE
		};

	public:
		// ip rules, se habilitado, o DDoS Proteção também é habilitado
		bool ip_rules;

	private:
		// Conexão histórico, ip & 0xFFFF
		PConnectHistory connect_history[0x10000];

		// Conexão Viva, ip & 0xFFFF
		PConnectLive connect_live[0x10000];

		// Access Control
		std::vector < AccessControl > access_allow;
		std::vector < AccessControl > access_deny;
		int access_order;

		// DDoS Protection
		UINT32 ddos_count;
		UINT32 ddos_interval;
		UINT32 ddos_autoreset;

		// Limit of connection per IP
		UINT32 limit_connection_per_ip;

#if defined(_WIN32)
		DWORD dwThread;			// ID Thread Timer
		HANDLE hThread;			// Handle Thread Timer
#elif defined(__linux__)
		thread *hThread;		// Handle Thread Timer
#endif

#if defined(_WIN32)
		static DWORD WINAPI pThread_Timer(LPVOID lpParameter);			// Thread que chame a função connec_check_clear de 5 em 5 minutos
#elif defined(__linux__)
		static void* pThread_Timer(LPVOID lpParameter);			// Thread que chame a função connec_check_clear de 5 em 5 minutos
#endif

#if defined(_WIN32)
		LARGE_INTEGER gettick();										// Pega o QueryPerformanceCounter do mommento
#elif defined(__linux__)
		timespec gettick();												// Pega o QueryPerformanceCounter do mommento
#endif

#if defined(_WIN32)
		LONG volatile m_thread_check_on;								// atomic variable on while thread condition on or off
#elif defined(__linux__)
		int32_t volatile m_thread_check_on;								// atomic variable on while thread condition on or off
#endif

	public:
		int connect_check(SOCKET _sock, UINT32 ip);						// Check se o ip pode conectar

		int connect_check2(SOCKET _sock, int _connect_ok, UINT32 _ip);	// Verifica se o ip pode ter mais conexões que é permitido

		bool connect_check_continue();									// Verifica se continuar com a thread_timer

		void thread_connect_check_off();								// Desligar a thread connect_check, modificando a variavel atomica

		UINT32 getDDoS_AutoReset();										// Get Interval DDoS Auto Reset history

		void reload_config_file();										// Ler novamente o aquivo de configuração do socket

		static int getConnectTime(SOCKET _sock);						// [Estática que não precisa da classe socket para executar essa função] Pega o Tempo de conexão do socket no sistema

	private:
#if defined(_WIN32)
		int connect_check_clear(LARGE_INTEGER tick);					// Limpa o histórico de conexão que já passaram do tempo permitido
		int connect_live_check_clear(LARGE_INTEGER tick);				// Limpa o histórico de conexão com o mesmo ip connectado
#elif defined(__linux__)
		int connect_check_clear(timespec tick);							// Limpa o histórico de conexão que já passaram do tempo permitido
		int connect_live_check_clear(timespec tick);					// Limpa o histórico de conexão com o mesmo ip connectado
#endif

		int connect_check_is_live();									// Tira as connexões que não estão masi conectada da lista ligada

		void close_connect_history();									// limpa e fecha o histórico de conexão

		void close_connect_live();										// Limpa e fecha conexões vivas

		void close_access_allow();										// limpa o vector de access allow

		void close_acces_deny();										// limpa o vector de access deny

		int access_ipmask(const char* str, PAccessControl acc);			// passa de ip str para ip mask

		void config_read(const char* file_name);						// Ler as configuraçao do socket
		
	protected:
		SOCKET m_socket;
		SOCKADDR_IN m_sockaddr;

#if defined(_WIN32)
		CRITICAL_SECTION m_cs_ip;										// Critical Section para o Historico de conexão
		CRITICAL_SECTION m_cs_ipx;										// Critical Section para as Conexões Vivas
#elif defined(__linux__)
		pthread_mutex_t m_cs_ip;
		pthread_mutex_t m_cs_ipx;
#endif
	};
}

#endif