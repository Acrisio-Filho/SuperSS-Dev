// Arquivo socket.cpp
// Criado em 02/03/2017 por Acrisio
// Implementação da classe socket

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
	#include <WS2tcpip.h>
	#include <MSWSock.h>
#elif defined(__linux__)
	#include <memory.h>
	#include <netdb.h>
	#include <fcntl.h>
#endif

// Minha stricmp/strcasecmp
int myStrCaseCmp(const char* __s1, const char* __s2) {

#if defined(_WIN32)
	return _stricmp(__s1, __s2);
#elif defined(__linux__)
	return strcasecmp(__s1, __s2);
#endif
};

#include "socket.h"

#include "../UTIL/exception.h"

#include <iostream>
#include <fstream>
#include <ctime>

#include "../UTIL/message_pool.h"
#include "../TYPE/stda_error.h"

#define STDA_SOCKET_CONFIG_FILE_PATH "config/socket_config.ini"

using namespace stdA;

// Thread que chame a função connec_check_clear de 5 em 5 minutos
#if defined(_WIN32)
DWORD socket::pThread_Timer(LPVOID lpParameter) 
#elif defined(__linux__)
void* socket::pThread_Timer(LPVOID lpParameter)
#endif
{
	socket *my_sock = reinterpret_cast<socket*>(lpParameter);

#if defined(_WIN32)
	LARGE_INTEGER tick;
#elif defined(__linux__)
	timespec tick;
#endif
	
	std::clock_t interval = std::clock();

	_smp::message_pool::getInstance().push(new message("socket::pThread_Timer iniciada com sucesso!", CL_ONLY_CONSOLE));

	while (my_sock->connect_check_continue()) {

#if defined(_WIN32)
		Sleep(1000);	// Dorme de 1 em 1 segundo
#elif defined(__linux__)
		sleep(1); // 1 segundo
#endif

		my_sock->connect_check_is_live();

		if (((std::clock() - interval) / (double)CLOCKS_PER_SEC) >= (my_sock->getDDoS_AutoReset() / 1000.0)) {
#if defined(_WIN32)
			QueryPerformanceCounter(&tick);
#elif defined(__linux__)
			clock_gettime(CLOCK_SOCKET_TO_CHECK, &tick);
#endif

			my_sock->connect_check_clear(tick);
			my_sock->connect_live_check_clear(tick);

			interval = std::clock();
		}
	}

	_smp::message_pool::getInstance().push(new message("socket::pThread_Timer saindo...", CL_ONLY_CONSOLE));

#if defined(_WIN32)
	return 0;
#elif defined(__linux__)
	return (void*)0;
#endif
};

// Pega o QueryPerformanceCounter do mommento
#if defined(_WIN32)
LARGE_INTEGER socket::gettick() {
	LARGE_INTEGER r;

	QueryPerformanceCounter(&r);

	return r;
};
#elif defined(__linux__)
timespec socket::gettick() {
	timespec r;

	clock_gettime(CLOCK_SOCKET_TO_CHECK, &r);

	return r;
};
#endif

// Verifica se continuar com a thread_timer
bool socket::connect_check_continue() {
#if defined(_WIN32)
	return InterlockedCompareExchange(&m_thread_check_on, 1, 1);
#elif defined(__linux__)
	int32_t check = 1; // Compare
	return __atomic_compare_exchange_n(&m_thread_check_on, &check, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
#endif
};

// Desligar a thread connect_check, modificando a variavel atomica
void socket::thread_connect_check_off() {
#if defined(_WIN32)
	InterlockedExchange(&m_thread_check_on, 0);
#elif defined(__linux__)
	__atomic_store_n(&m_thread_check_on, 0, __ATOMIC_RELAXED);
#endif
};

// Get Interval DDoS Auto Reset history
UINT32 socket::getDDoS_AutoReset() {
	return ddos_autoreset;
};

void socket::reload_config_file() {

	// Bloqueia os check dos access ip mask
#if defined(_WIN32)
	EnterCriticalSection(&m_cs_ip);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_ip);
#endif

	// Limpa os vectores access allow e deny
	close_access_allow();
	close_acces_deny();

	try {
		
		// Ler novamento o arquivo de configuração do socket
		config_read(STDA_SOCKET_CONFIG_FILE_PATH);

	} catch (exception& e) {
		UNREFERENCED_PARAMETER(e);
		// Deadlock protection
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs_ip);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs_ip);
#endif

		// Relança a exception
		throw;
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_ip);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_ip);
#endif

	// Log
	_smp::message_pool::getInstance().push(new message("[socket::reload_config_file][Log] Socket config files has been reloaded.", CL_FILE_LOG_AND_CONSOLE));
};

int socket::getConnectTime(SOCKET _sock) {
	
#if defined(_WIN32)
	if (_sock != INVALID_SOCKET) {
		int seconds;
		int size_seconds = sizeof(seconds);

		if (0 == ::getsockopt(_sock, SOL_SOCKET, SO_CONNECT_TIME, (char*)&seconds, &size_seconds))
			return seconds;
		else
			return -2;	// Error ao pegar optsock SO_CONNECT_TIME
			//throw exception("[session::getConnectTime] erro ao pegar optsock SO_CONNECT_TIME.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION, 50, WSAGetLastError()));
	}
#elif defined(__linux__)
	if (_sock.fd != INVALID_SOCKET) {

		if (_sock.connect_time.tv_nsec == 0 && _sock.connect_time.tv_sec == 0)
			return -1;

		int rt_flg = fcntl(_sock.fd, F_GETFL);

		if (rt_flg == -1)
			return (errno == EAGAIN | errno == EBADF | errno == EACCES) ? -1 : -2; // -1 socket is not connected, -2 Erro ao pegar status flag do socket

		int error = 0;
		socklen_t len = sizeof (error);
		int retval = getsockopt (_sock.fd, SOL_SOCKET, SO_ERROR, &error, &len);

		if (retval != 0) {
			/* there was a problem getting the error code */
			return -1;
		}

		if (error != 0) {
			/* socket has a non zero error status */
			return -1;
		}

		timespec ts;

		clock_gettime(CLOCK_SOCKET_TO_CHECK, &ts);

		return (int)(((uint64_t)(ts.tv_sec * 1000000000 + ts.tv_nsec) - (uint64_t)(_sock.connect_time.tv_sec * 1000000000 + _sock.connect_time.tv_nsec)) / 1000000000);
	}
#endif

	return -1;
};

// Check se o ip pode conectar
int socket::connect_check(SOCKET _sock, UINT32 ip) {

	unsigned int  i, is_allowip = 0, is_denyip = 0;
	
	int connect_ok = 0;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_ip);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_ip);
#endif

	for (i = 0; i < access_allow.size(); i++) {
		if ((ip & access_allow[i].mask) == (access_allow[i].ip & access_allow[i].mask)) {
			is_allowip = 1;
			break;
		}
	}

	for (i = 0; i < access_deny.size(); i++) {
		if ((ip & access_deny[i].mask) == (access_deny[i].ip & access_deny[i].mask)) {
			is_denyip = 1;
			break;
		}
	}

	switch (access_order) {
	case ACO_DENY_ALLOW:
	default:
		if (is_denyip)
			connect_ok = 0; // rejeita
		else if (is_allowip)
			connect_ok = 2;	// aceita unconditional
		else
			connect_ok = 1; // aceita
		break;
	case ACO_ALLOW_DENY:
		if (is_allowip)
			connect_ok = 2;	// aceita unconditional
		else if (is_denyip)
			connect_ok = 0;	// rejeita
		else
			connect_ok = 1;	// aceita
		break;
	case ACO_MUTUAL_FAILURE:
		if (is_allowip && !is_denyip)
			connect_ok = 2;	// aceita unconditional
		else
			connect_ok = 0; // rejeita
		break;
	}

	// Libera o Critical section para não dá dead lock com a função de baixo que usa outro critical section
#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_ip);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_ip);
#endif

	int ret_chk2 = connect_ok;

	// Verifica se o ip já tem o limite de conexões por IP
	if ((ret_chk2 = connect_check2(_sock, connect_ok, ip)) <= 0)
		return ret_chk2;	// rejeita

#if defined(_WIN32)
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
#elif defined(__linux__)
	timespec frequency;
	clock_getres(CLOCK_SOCKET_TO_CHECK, &frequency);
#endif

	// Entra no critical section novamente
#if defined(_WIN32)
	EnterCriticalSection(&m_cs_ip);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_ip);
#endif

	PConnectHistory hist = connect_history[ip & 0xFFFF];

	while (hist) {
		if (ip == hist->ip) {
			if (hist->ddos) {

				// DDoS Attack
				char s_ip[4];
				memmove(s_ip, &ip, 4);

				_smp::message_pool::getInstance().push(new message("[socket::connect_check][WARNING] DDoS Atack From IP : "
						+ std::to_string((unsigned short)(unsigned char)s_ip[3]) + "." + std::to_string((unsigned short)(unsigned char)s_ip[2]) + "."
						+ std::to_string((unsigned short)(unsigned char)s_ip[1]) + "." + std::to_string((unsigned short)(unsigned char)s_ip[0]), CL_FILE_LOG_AND_CONSOLE));

				// Libera Critical Section
#if defined(_WIN32)
				LeaveCriticalSection(&m_cs_ip);
#elif defined(__linux__)
				pthread_mutex_unlock(&m_cs_ip);
#endif
				
				return (connect_ok == 2 ? 1 : 0);

			}else if (DIFF_TICK(gettick(), hist->tick, frequency) < ddos_interval) {
				
				hist->tick = gettick();

				if (hist->count++ >= ddos_count) {
					// DDoS Attack
					char s_ip[4];
					memmove(s_ip, &ip, 4);

					_smp::message_pool::getInstance().push(new message("[socket::connect_check][WARNING] DDoS Atack From IP : " 
							+ std::to_string((unsigned short)(unsigned char)s_ip[3]) + "." + std::to_string((unsigned short)(unsigned char)s_ip[2]) + "."
							+ std::to_string((unsigned short)(unsigned char)s_ip[1]) + "." + std::to_string((unsigned short)(unsigned char)s_ip[0]), CL_FILE_LOG_AND_CONSOLE));

					hist->ddos = 1;

					// Libera Critical Section
#if defined(_WIN32)
					LeaveCriticalSection(&m_cs_ip);
#elif defined(__linux__)
					pthread_mutex_unlock(&m_cs_ip);
#endif

					return (connect_ok == 2) ? 1 : 0;
				}

				// Libera Critical Section
#if defined(_WIN32)
				LeaveCriticalSection(&m_cs_ip);
#elif defined(__linux__)
				pthread_mutex_unlock(&m_cs_ip);
#endif

				return connect_ok;
			}else {
				
				hist->tick = gettick();
				hist->count = 0;

				// Libera Critical Section
#if defined(_WIN32)
				LeaveCriticalSection(&m_cs_ip);
#elif defined(__linux__)
				pthread_mutex_unlock(&m_cs_ip);
#endif

				return connect_ok;
			}
		}
		hist = hist->next;
	}

	hist = (PConnectHistory)malloc(sizeof(ConnectHistory));
	// Aqui da erro se não conseguir alocar a memória
	if (hist != nullptr) {

		hist->clear();
		hist->ip = ip;
		hist->tick = gettick();
		hist->next = connect_history[ip & 0xFFFF];
		connect_history[ip & 0xFFFF] = hist;
	}

	// Libera Critical Section
#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_ip);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_ip);
#endif

	return connect_ok;
};

int socket::connect_check2(SOCKET _sock, int _connect_ok, UINT32 _ip) {

	uint32_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_ipx);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_ipx);
#endif

	PConnectLive hist = connect_live[_ip & 0xFFFF];

	while (hist) {
		if (_ip == hist->ip) {
			// limit of connection per IP
			if (getConnectTime(hist->sock) >= 0 && ++count >= limit_connection_per_ip) {

				char s_ip[4];
				memmove(s_ip, &_ip, 4);

				_smp::message_pool::getInstance().push(new message("[socket::connect_check2][WARNING] Limit of Connection Exceded From IP : " + std::to_string((unsigned short)(unsigned char)s_ip[3]) 
						+ "." + std::to_string((unsigned short)(unsigned char)s_ip[2]) + "." + std::to_string((unsigned short)(unsigned char)s_ip[1]) 
						+ "." + std::to_string((unsigned short)(unsigned char)s_ip[0]), CL_FILE_LOG_AND_CONSOLE));

				// Libera Critical Section
#if defined(_WIN32)
				LeaveCriticalSection(&m_cs_ipx);
#elif defined(__linux__)
				pthread_mutex_unlock(&m_cs_ipx);
#endif
				
				return (_connect_ok == 2 ? 1 : -1);
			}
		}
		hist = hist->next;
	}

	hist = (PConnectLive)malloc(sizeof(ConnectLive));
	// Aqui da erro se não conseguir alocar a memória
	if (hist != nullptr) {

		hist->clear();
		hist->ip = _ip;
		hist->tick = gettick();
		hist->sock = _sock;
		hist->next = connect_live[_ip & 0xFFFF];
		connect_live[_ip & 0xFFFF] = hist;
	}

	// Libera Critical Section
#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_ipx);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_ipx);
#endif

	return _connect_ok;
};

// Limpa o histórico de conexão que já passaram do tempo permitido
#if defined(_WIN32)
int socket::connect_check_clear(LARGE_INTEGER tick) 
#elif defined(__linux__)
int socket::connect_check_clear(timespec tick) 
#endif
{

	unsigned int i, clear = 0, list = 0;
	ConnectHistory root;
	PConnectHistory prev_hist;
	PConnectHistory hist;

#if defined(_WIN32)
	LARGE_INTEGER frequency;

	QueryPerformanceFrequency(&frequency);
#elif defined(__linux__)
	timespec frequency;
	clock_getres(CLOCK_SOCKET_TO_CHECK, &frequency);
#endif

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_ip);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_ip);
#endif

	for (i = 0; i < 0x10000u; i++) {
		prev_hist = &root;
		root.next = hist = connect_history[i];
		while (hist) {
			if ((!hist->ddos && DIFF_TICK(tick, hist->tick, frequency) > ddos_interval * 3) ||
				(hist->ddos && DIFF_TICK(tick, hist->tick, frequency) > ddos_autoreset)) {
				prev_hist->next = hist->next;
				free(hist);
				hist = prev_hist->next;
				clear++;
			}else {
				prev_hist = hist;
				hist = hist->next;
			}
			list++;
		}
		connect_history[i] = root.next;
	}

	// Libera Critical Section
#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_ip);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_ip);
#endif

	return 0;
};

// Limpa o histórico de conexão com o mesmo ip connectado
#if defined(_WIN32)
int socket::connect_live_check_clear(LARGE_INTEGER tick) 
#elif defined(__linux__)
int socket::connect_live_check_clear(timespec tick) 
#endif
{

	unsigned int i, clear = 0, list = 0;
	ConnectLive root;
	PConnectLive prev_hist;
	PConnectLive hist;

	int seconds = 0;

#if defined(_WIN32)
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
#elif defined(__linux__)
	timespec frequency;
	clock_getres(CLOCK_SOCKET_TO_CHECK, &frequency);
#endif

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_ipx);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_ipx);
#endif

	for (i = 0; i < 0x10000u; i++) {
		prev_hist = &root;
		root.next = hist = connect_live[i];

		while (hist) {

			if ((seconds = getConnectTime(hist->sock)) < 0 || seconds < (DIFF_TICK(tick, hist->tick, frequency) / 1000/*Seconds*/) ) {
				prev_hist->next = hist->next;
				free(hist);
				hist = prev_hist->next;
				clear++;
			}else {
				prev_hist = hist;
				hist = hist->next;
			}
			list++;
		}
		connect_live[i] = root.next;
	}

	// Libera Critical Section
#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_ipx);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_ipx);
#endif

	return 0;
};

int socket::connect_check_is_live() {
	
	unsigned int i, clear = 0, list = 0;
	ConnectLive root;
	PConnectLive prev_hist;
	PConnectLive hist;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_ipx);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_ipx);
#endif

	for (i = 0; i < 0x10000u; i++) {
		prev_hist = &root;
		root.next = hist = connect_live[i];
		while (hist) {
#if defined(_WIN32)
			if (hist->sock == INVALID_SOCKET || getConnectTime(hist->sock) < 0) 
#elif defined(__linux__)
			if (hist->sock.fd == INVALID_SOCKET || getConnectTime(hist->sock) < 0)
#endif
			{
				prev_hist->next = hist->next;
				free(hist);
				hist = prev_hist->next;
				clear++;
			}else {
				prev_hist = hist;
				hist = hist->next;
			}
			list++;
		}
		connect_live[i] = root.next;
	}

	// Libera Critical Section
#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_ipx);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_ipx);
#endif

	return 0;
};

// passa de ip str para ip mask
int socket::access_ipmask(const char* str, PAccessControl acc) {
	UINT32 ip, mask;

	if (myStrCaseCmp(str, "all") == 0)
		ip = mask = 0;
	else {
		unsigned int a[4];
		unsigned int m[4];
		int n;

#if defined(_WIN32)
		if (((n = sscanf_s(str, "%3u.%3u.%3u.%3u/%3u.%3u.%3u.%3u", a, a + 1, a + 2, a + 3, m, m + 1, m + 2, m + 3)) != 8 &&
			(n = sscanf_s(str, "%3u.%3u.%3u.%3u/%3u", a, a + 1, a + 2, a + 3, m)) != 5 &&
			(n = sscanf_s(str, "%3u.%3u.%3u.%3u", a, a + 1, a + 2, a + 3)) != 4) ||
			a[0] > 255 || a[1] > 255 || a[2] > 255 || a[3] > 255 ||
			(n == 8 && (m[0] > 255 || m[1] > 255 || m[2] > 255 || m[3] > 255)) ||
			(n == 5 && m[0] > 32))
#elif defined(__linux__)
		if (((n = sscanf(str, "%3u.%3u.%3u.%3u/%3u.%3u.%3u.%3u", a, a + 1, a + 2, a + 3, m, m + 1, m + 2, m + 3)) != 8 &&
			(n = sscanf(str, "%3u.%3u.%3u.%3u/%3u", a, a + 1, a + 2, a + 3, m)) != 5 &&
			(n = sscanf(str, "%3u.%3u.%3u.%3u", a, a + 1, a + 2, a + 3)) != 4) ||
			a[0] > 255 || a[1] > 255 || a[2] > 255 || a[3] > 255 ||
			(n == 8 && (m[0] > 255 || m[1] > 255 || m[2] > 255 || m[3] > 255)) ||
			(n == 5 && m[0] > 32))
#endif
			return 0;

		ip = (a[0] << 24) | (a[1] << 16) | (a[2] << 8) | a[3];

		if (n == 8)
			mask = (m[0] << 24) | (m[1] << 16) | (m[2] << 8) | m[3];
		else if (n == 5) {
			mask = 0;
			while (m[0]) {
				mask = (mask >> 1) | 0x80000000;
				m[0]--;
			}
		}else
			mask = 0xFFFFFFFF;
	}

	acc->ip = ip;
	acc->mask = mask;

	return 1;
};

// Ler as configuraçao do socket
void socket::config_read(const char* file_name) {

	char line[1024], w1[1024], w2[1024];
	std::ifstream fopen(file_name);

	if (fopen.is_open()) {
		while (!fopen.getline(line, sizeof(line)).eof() || fopen.gcount() > 0) {

			if (line[0] == '/' && line[1] == '/')
				continue;

#if defined(_WIN32)
			if (sscanf_s(line, "%1023[^:]: %1023[^\r\n]", w1, (uint32_t)sizeof(w1), w2, (uint32_t)sizeof(w1)) != 2)
#elif defined(__linux__)
			if (sscanf(line, "%1023[^:]: %1023[^\r\n]", w1, w2) != 2)
#endif
				continue;

			if (myStrCaseCmp(w1, "enable_ip_rules") == 0)
				ip_rules = (atoi(w2)) ? true : false;
			else if (!myStrCaseCmp(w1, "order")) {
				if (!myStrCaseCmp(w2, "deny,allow"))
					access_order = ACO_DENY_ALLOW;
				else if (!myStrCaseCmp(w2, "allow,deny"))
					access_order = ACO_ALLOW_DENY;
				else if (!myStrCaseCmp(w2, "mutual-failure"))
					access_order = ACO_MUTUAL_FAILURE;
			}else if (!myStrCaseCmp(w1, "allow")) {
				access_allow.push_back(AccessControl());
				if (!access_ipmask(w2, &access_allow.back()))
					access_allow.pop_back();
			}else if (!myStrCaseCmp(w1, "deny")) {
				access_deny.push_back(AccessControl());
				if (!access_ipmask(w2, &access_deny.back()))
					access_deny.pop_back();
			}else if (!myStrCaseCmp(w1, "ddos_interval"))
				ddos_interval = atoi(w2);
			else if (!myStrCaseCmp(w1, "ddos_count"))
				ddos_count = atoi(w2);
			else if (myStrCaseCmp(w1, "ddos_autoreset") == 0)
				ddos_autoreset = atoi(w2);
			else if (myStrCaseCmp(w1, "limit_connection_per_ip") == 0)
				limit_connection_per_ip = (UINT32)atoi(w2);
		}
	}else
		throw exception("[socket::config_read][Error] Nao consiguiu abrir o arquivo " + std::string(file_name) + ".", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 13, errno));

	fopen.close();
};

// limpa e fecha o histórico de conexão
void socket::close_connect_history() {
	PConnectHistory hist, next_hist;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_ip);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_ip);
#endif

	for (unsigned int i = 0; i < 0x10000u; i++) {
		hist = connect_history[i];
		while (hist) {
			next_hist = hist->next;
			free(hist);
			hist = next_hist;
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_ip);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_ip);
#endif
};

void socket::close_connect_live() {
	PConnectLive hist, next_hist;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_ipx);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_ipx);
#endif

	for (unsigned int i = 0; i < 0x10000u; i++) {
		hist = connect_live[i];
		while (hist) {
			next_hist = hist->next;
			free(hist);
			hist = next_hist;
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_ipx);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_ipx);
#endif
};

// limpa o vector de access allow
void socket::close_access_allow() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_ip);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_ip);
#endif

	if (access_allow.size() > 0) {
		access_allow.clear();
		access_allow.shrink_to_fit();
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_ip);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_ip);
#endif
};

// limpa o vector de access deny
void socket::close_acces_deny() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs_ip);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_ip);
#endif

	if (access_deny.size() > 0) {
		access_deny.clear();
		access_deny.shrink_to_fit();
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_ip);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_ip);
#endif
};

socket::socket(int option) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs_ip);
	InitializeCriticalSection(&m_cs_ipx);

	m_socket = INVALID_SOCKET;

#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs_ip);
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs_ipx);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;

	m_socket.fd = INVALID_SOCKET;
	m_socket.connect_time.tv_sec = 0;
	m_socket.connect_time.tv_nsec = 0;
#endif

	// ip rules e DDoS Protection
	ip_rules = true;

	// conexão histórico
	memset(connect_history, 0, sizeof(connect_history));

	// conexão live
	memset(connect_live, 0, sizeof(connect_live));

	// Controle de acesso
	access_order = ACO_DENY_ALLOW;

	// DDoS Proteção
	ddos_count = 10;
	ddos_interval = 3 * 1000;			// 3 segundos
	ddos_autoreset = 10 * 60 * 1000;	// 10 Minutos

	// Limite de conexão por IP
	limit_connection_per_ip = 3;	// 3 Conexões

#if defined(_WIN32)
	hThread = INVALID_HANDLE_VALUE;
	dwThread = 0;
#elif defined(__linux__)
	hThread = nullptr;
#endif

	if (option == 1 /*Server*/) {
		// Ler o arquivo de configuração
		config_read(STDA_SOCKET_CONFIG_FILE_PATH);

#if defined(_WIN32)
		InterlockedExchange(&m_thread_check_on, 1);		// thread connect_check on

		if ((hThread = CreateThread(NULL, 0, pThread_Timer, (LPVOID)this, 0, &dwThread)) == INVALID_HANDLE_VALUE)
			throw exception("[socket::socket][Error] ao criar a thread timer.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 12, GetLastError()));
#elif defined(__linux__)
		__atomic_store_n(&m_thread_check_on, 1, __ATOMIC_RELAXED);	// thread connect_check on

		hThread = new thread(0, socket::pThread_Timer, this, 0, 0);
#endif
	}
};

socket::socket(SOCKET socket, int option) {
	
#if defined(_WIN32)
	InitializeCriticalSection(&m_cs_ip);
	InitializeCriticalSection(&m_cs_ipx);

	if (socket == INVALID_SOCKET)
		throw exception("[socket::socket][Error] Invalid socket", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 1, WSAENOTSOCK));
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs_ip);
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs_ipx);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;

	if (socket.fd == INVALID_SOCKET || (socket.connect_time.tv_sec == 0 && socket.connect_time.tv_nsec == 0))
		throw exception("[socket::socket][Error] Invalid socket", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 1, ENONET));
#endif

	m_socket = socket;

	// ip rules e DDoS Protection
	ip_rules = true;

	// conexão histórico
	memset(connect_history, 0, sizeof(connect_history));

	// conexão live
	memset(connect_live, 0, sizeof(connect_live));

	// Controle de acesso
	access_order = ACO_DENY_ALLOW;

	// DDoS Proteção
	ddos_count = 10;
	ddos_interval = 3 * 1000;			// 3 segundos
	ddos_autoreset = 10 * 60 * 1000;	// 10 Minutos

	// Limite de conexão por IP
	limit_connection_per_ip = 3;	// 3 Conexões

#if defined(_WIN32)
	hThread = INVALID_HANDLE_VALUE;
	dwThread = 0;
#elif defined(__linux__)
	hThread = nullptr;
#endif

	if (option == 1 /*Server*/) {
		// Ler o arquivo de configuração
		config_read(STDA_SOCKET_CONFIG_FILE_PATH);

#if defined(_WIN32)
		InterlockedExchange(&m_thread_check_on, 1);		// thread connect_check on

		if ((hThread = CreateThread(NULL, 0, pThread_Timer, (LPVOID)this, 0, &dwThread)) == INVALID_HANDLE_VALUE)
			throw exception("[socket::socket][Error] ao criar a thread timer.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 12, GetLastError()));
#elif defined(__linux__)

		__atomic_store_n(&m_thread_check_on, 1, __ATOMIC_RELAXED);

		hThread = new thread(0, socket::pThread_Timer, this, 0, 0);
#endif
	}
};

socket::~socket() {

	// Finaliza a thread timer
#if defined(_WIN32)
	if (hThread != INVALID_HANDLE_VALUE) {
		thread_connect_check_off();	// Desliga thread connect_check
		
		if (WaitForSingleObject(hThread, INFINITE) != WAIT_OBJECT_0)
			TerminateThread(hThread, 0);
	}
#elif defined(__linux__)
	if (hThread != nullptr) {
		thread_connect_check_off();	// Desliga thread connect_check
		
		try {
			hThread->waitThreadFinish(INFINITE);
		}catch (...) {
			hThread->exit_thread(); // Force
		}

		delete hThread;

		hThread = nullptr;
	}
#endif

	close_connect_history();

	close_connect_live();

	close_access_allow();

	close_acces_deny();

#if defined(_WIN32)
	if (m_socket != INVALID_SOCKET) 
#elif defined(__linux__)
	if (m_socket.fd != INVALID_SOCKET)
#endif
	{
		try {
			abortiveClose();
		}catch (...) {	}
	}
	
#if defined(_WIN32)
	DeleteCriticalSection(&m_cs_ip);
	DeleteCriticalSection(&m_cs_ipx);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs_ip);
	pthread_mutex_destroy(&m_cs_ipx);
#endif
};

SOCKET socket::detatch() {
	SOCKET ret_sock = m_socket;

#if defined(_WIN32)
	m_socket = INVALID_SOCKET;
#elif defined(__linux__)
	m_socket.fd = INVALID_SOCKET;
	m_socket.connect_time.tv_sec = 0;
	m_socket.connect_time.tv_nsec = 0;
#endif

	return ret_sock;
};

void socket::attach(SOCKET socket) {
	abortiveClose();

	m_socket = socket;
};

void socket::close() {
#if defined(_WIN32)
	if (0 != closesocket(m_socket))
		throw exception("[socket::close][Error] ao fechar o socket.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 2, WSAGetLastError()));
#elif defined(__linux__)
	if (0 != ::closesocket(m_socket.fd))
		throw exception("[socket::close][Error] ao fechar o socket.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 2, errno));
#endif
};

void socket::abortiveClose() {
#if defined(_WIN32)
	if (m_socket != INVALID_SOCKET) {
		LINGER li;

		li.l_onoff = 1;
		li.l_linger = 0;

		if (SOCKET_ERROR == setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char*)&li, sizeof li))
			throw exception("[socket::abortiveClose][Error] ao setsockopt.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 3, WSAGetLastError()));

		close();
	}
#elif defined(__linux__)
	if (m_socket.fd != INVALID_SOCKET) {
		linger li;

		li.l_onoff = 1;
		li.l_linger = 0;

		if (-1 == setsockopt(m_socket.fd, SOL_SOCKET, SO_LINGER, (char*)&li, sizeof(li)))
			throw exception("[socket::abortiveClose][Error] ao setsockopt.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 3, errno));

		close();
	}
#endif
};

void socket::shutdown(int como) {
#if defined(_WIN32)
	if (m_socket != INVALID_SOCKET && 0 != ::shutdown(m_socket, como))
		throw exception("[socket::shutdown][Error] ao desligar o socket.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 4, WSAGetLastError()));
#elif defined(__linux__)
	if (m_socket.fd != INVALID_SOCKET && 0 != ::shutdown(m_socket.fd, como))
		throw exception("[socket::shutdown][Error] ao desligar o socket.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 4, errno));
#endif
};

void socket::listen(int max_listen_same_time) {
#if defined(_WIN32)
	if (SOCKET_ERROR == ::listen(m_socket, max_listen_same_time))
		throw exception("[socket::shutdown][Error] ao colocar o socket para escutar.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 5, WSAGetLastError()));
#elif defined(__linux__)
	if (-1 == ::listen(m_socket.fd, max_listen_same_time))
		throw exception("[socket::shutdown][Error] ao colocar o socket para escutar.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 5, errno));
#endif
};	

void socket::bind(const SOCKADDR_IN &address) {
#if defined(_WIN32)
	if (SOCKET_ERROR == ::bind(m_socket, reinterpret_cast<struct sockaddr* >(const_cast<SOCKADDR_IN* >(&address)), sizeof SOCKADDR_IN))
		throw exception("[socket::bind][Error] ao binder o socket.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 6, WSAGetLastError()));
#elif defined(__linux__)
	if (-1 == ::bind(m_socket.fd, reinterpret_cast<struct sockaddr* >(const_cast<SOCKADDR_IN* >(&address)), sizeof(SOCKADDR_IN)))
		throw exception("[socket::bind][Error] ao binder o socket.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 6, errno));
#endif
};

void socket::connect(std::string host, size_t port) {
	
	if (host.empty())
		throw exception("[socket::connect][Error] host have empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 7, 0));
	
	sockaddr_in sa = { 0 };

	if (isalpha(host.c_str()[0])) {
		addrinfo hinst = { 0 }, *pAddrInfo = nullptr;

		char name[1024];

		hinst.ai_family = AF_INET;
		hinst.ai_protocol = IPPROTO_TCP;
		hinst.ai_socktype = SOCK_STREAM;

#if defined(_WIN32)
		if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hinst, &pAddrInfo) != 0)
			throw exception("[socket::connect][Error] Nao conseguiu resolver o nome de host.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 10, WSAGetLastError()));
#elif defined(__linux__)
		int error = 0;

		if ((error = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hinst, &pAddrInfo)) != 0)
			throw exception("[socket::connect][Error] Nao conseguiu resolver o nome de host.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 10, (error == EAI_SYSTEM ? errno : error)));
#endif

		if (pAddrInfo == nullptr)
			throw exception("[socket::connect][Error] Nao conseguiu pegar o host pelo nome", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 11, 0));

		//for (addrinfo *ptr = pAddrInfo; ptr != nullptr; ptr = ptr->ai_next);

		inet_ntop(AF_INET, &((sockaddr_in*)pAddrInfo->ai_addr)->sin_addr, name, 1024);

		if (pAddrInfo != nullptr)
			freeaddrinfo(pAddrInfo);

		if (!inet_pton(AF_INET, name, &sa.sin_addr.s_addr)) {

#if defined(_WIN32)
			auto last_error = WSAGetLastError();
#elif defined(__linux__)
			auto last_error = errno;
#endif

			throw exception("[socket::connect][Error] ao converter o ip host string doted for binary mode.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 8, last_error));
		}

	}
	else if (!inet_pton(AF_INET, host.c_str(), &sa.sin_addr.s_addr)) {

#if defined(_WIN32)
		auto last_error = WSAGetLastError();
#elif defined(__linux__)
		auto last_error = errno;
#endif

		throw exception("[socket::connect][Error] ao converter o ip host string doted for binary mode.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 8, last_error));
	}

	sa.sin_family = AF_INET;
	sa.sin_port = htons((u_short)port);
	
	connect(&sa);
};

void socket::connect(const SOCKADDR_IN* _s_addr) {
#if defined(_WIN32)
	if (SOCKET_ERROR == ::connect(m_socket, (const SOCKADDR*)_s_addr, sizeof SOCKADDR))
		throw exception("[socket::connect][Error] ao connectar com o host.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 9, WSAGetLastError()));
#elif defined(__linux__)
	if (-1 == ::connect(m_socket.fd, (const sockaddr*)_s_addr, sizeof(sockaddr))) {

		// epoll, poll and select, nonblocking socket
		if (errno != EAGAIN && errno != EINPROGRESS)
			throw exception("[socket::connect][Error] ao connectar com o host.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::_SOCKET, 9, errno));
	}
#endif
};
