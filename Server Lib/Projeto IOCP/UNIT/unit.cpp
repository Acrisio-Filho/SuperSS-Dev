// Arquivo unit.cpp
// Criado em 02/12/2018 as 12:32 por Acrisio
// Implementação da classe unit

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#include <WS2tcpip.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/fcntl.h>
#include <signal.h>
#endif

#include "unit.hpp"
#include "../UTIL/exception.h"
#include "../UTIL/message_pool.h"
#include "../SOCKET/socketserver.h"
#include "../SOCKET/socket.h"
#include "../UTIL/hex_util.h"
#include "../DATABASE/mssql.h"
#include "../DATABASE/mysql_db.h"

#include "../TYPE/stda_error.h"

#include "../PANGYA_DB/cmd_register_server.hpp"

#include "../DATABASE/normal_manager_db.hpp"

#if defined(_WIN32)
#include <conio.h>
#elif defined(__linux__)
#include "../UTIL/ConioPort.h"
#endif

#if defined(_WIN32)
#include <DbgHelp.h>
#endif

using namespace stdA;

unit::unit(session_manager& _session_manager, uint32_t _db_instance_num, uint32_t _job_thread_num)
	: m_session_manager(_session_manager), threadpl_unit(16, 16, _job_thread_num), m_si{}, m_server_list(), m_reader_ini(_INI_PATH), m_state(UNINITIALIZED), m_ctx_db{0},
#if defined(_WIN32)
	  EventShutdownServer(INVALID_HANDLE_VALUE), 
	  EventAcceptConnection(INVALID_HANDLE_VALUE), 
#elif defined(__linux__)
	  EventShutdownServer(nullptr), 
	  EventAcceptConnection(nullptr), 
#endif
	  m_shutdown(nullptr) {
	
	try {

		// Config Initialize, INI FILE
		config_init();

		size_t i = 0;

		m_accept_sock = nullptr;

		// chama aqui, que quando o auth_server estiver initializado(o construtor das classe base terminar e criar essa classe)
		// pode chamar essas funções puras

#if defined(_WIN32)
		if ((EventShutdownServer = CreateEvent(NULL, true, false, NULL)) == INVALID_HANDLE_VALUE)
			throw exception("[unit::unit][Error] ao criar o evento ShutdownServer.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT, 54, GetLastError()));

		if ((EventAcceptConnection = CreateEvent(NULL, true, false, NULL)) == INVALID_HANDLE_VALUE)
			throw exception("[unit::unit][Error] ao criar o evento Accept Connections.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT, 54, GetLastError()));

		InterlockedExchange(&m_continue_accpet, 1);
#elif defined(__linux__)
		EventShutdownServer = new Event(true, 0u);

		if (!EventShutdownServer->is_good()) {

			delete EventShutdownServer;

			EventShutdownServer = nullptr;

			throw exception("[unit::unit][Error] ao criar o evento ShutdownServer.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT, 54, errno));
		}

		EventAcceptConnection = new Event(true, 0u);

		if (!EventAcceptConnection->is_good()) {

			delete EventAcceptConnection;

			EventAcceptConnection = nullptr;

			throw exception("[unit::unit][Error] ao criar o evento Accept Connections.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT, 54, errno));
		}

		__atomic_store_n(&m_continue_accpet, 1, __ATOMIC_RELAXED);
#endif

		// DB_NORMAL
		NormalManagerDB::create(_db_instance_num);

#if defined(_WIN32)
		InterlockedExchange(&m_continue_monitor, 1);
#elif defined(__linux__)
		__atomic_store_n(&m_continue_monitor, 1, __ATOMIC_RELAXED);
#endif

		// Monitor Thread
		m_thread_monitor = new thread(TT_MONITOR, unit::_monitor, (LPVOID)this/*, THREAD_PRIORITY_BELOW_NORMAL*/);

#if defined(_WIN32)
		InterlockedExchange(&m_atomic_disconnect_session, 1);
#elif defined(__linux__)
		__atomic_store_n(&m_atomic_disconnect_session, 1, __ATOMIC_RELAXED);
#endif

		// Disconnect Session
		m_threads.push_back(new thread(TT_DISCONNECT_SESSION, unit::_disconnect_session, (LPVOID)this));

		// Accept(s)
		m_threads.push_back(new thread(TT_ACCEPT, unit::_accept, (LPVOID)this, THREAD_PRIORITY_ABOVE_NORMAL/*1 = Primeiro, 2 = segundo core acho*/));		// Estava NORMAL

		m_state = GOOD;

	}catch (exception& e) {
		
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		m_state = FAILURE;
	}
};

unit::~unit() {

	if (m_thread_monitor != nullptr)
		delete m_thread_monitor;

	m_thread_monitor = nullptr;

	if (!m_server_list.empty()) {
		m_server_list.clear();
		m_server_list.shrink_to_fit();
	}

	if (m_shutdown != nullptr && !m_timer_mgr.isEmpty())
		m_timer_mgr.deleteTimer(m_shutdown);

	m_shutdown = nullptr;

#if defined(_WIN32)
	if (EventShutdownServer != INVALID_HANDLE_VALUE)
		CloseHandle(EventShutdownServer);

	if (EventAcceptConnection != INVALID_HANDLE_VALUE)
		SetEvent(EventAcceptConnection);

	EventShutdownServer = INVALID_HANDLE_VALUE;
	EventAcceptConnection = INVALID_HANDLE_VALUE;
#elif defined(__linux__)
	if (EventShutdownServer != nullptr)
		delete EventShutdownServer;

	if (EventAcceptConnection != nullptr)
		delete EventAcceptConnection;

	EventShutdownServer = nullptr;
	EventAcceptConnection = nullptr;
#endif

	NormalManagerDB::destroy();
};

#if defined(_WIN32)
DWORD unit::_accept(LPVOID lpParameter) {
#elif defined(__linux__)
void* unit::_accept(LPVOID lpParameter) {
#endif
	BEGIN_THREAD_SETUP(unit);

	result = pTP->accept();

	END_THREAD_SETUP("Accept()");
};

#if defined(_WIN32)
DWORD unit::_monitor(LPVOID lpParameter) {
#elif defined(__linux__)
void* unit::_monitor(LPVOID lpParameter) {
#endif
	BEGIN_THREAD_SETUP(unit);

	result = pTP->monitor();

	END_THREAD_SETUP("monitor()");
};

#if defined(_WIN32)
DWORD unit::_disconnect_session(LPVOID lpParameter) {
#elif defined(__linux__)
void* unit::_disconnect_session(LPVOID lpParameter) {
#endif
	BEGIN_THREAD_SETUP(unit);

	result = pTP->disconnect_session();

	END_THREAD_SETUP("disconnect_session()");
};

#if defined(_WIN32)
DWORD unit::monitor() {
#elif defined(__linux__)
void* unit::monitor() {
#endif
	
	try {
		
		_smp::message_pool::getInstance().push(new message("[unit::monitor][Log] monitor iniciado com sucesso!"));
		
#if defined(_WIN32)
		while (InterlockedCompareExchange(&m_continue_monitor, 1, 1)) {
#elif defined(__linux__)
		int32_t check_m = 1; // Compare
		while (__atomic_compare_exchange_n(&m_continue_monitor, &check_m, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
#endif
			
			try {

				try {

					// Verifica se já virou o dia e cria novos arquivos de log
					// Todo dia 00:00 horas cria um novo arquivo de log
					if (_smp::message_pool::getInstance().checkUpdateDayLog())
						_smp::message_pool::getInstance().push(new message("[unit::monitor::UpdateLogFiles][Log] Atualizou os arquivos de Log por que trocou de dia.", CL_FILE_LOG_AND_CONSOLE));

				}catch (exception& e) {

					_smp::message_pool::getInstance().push(new message("[unit::monitor::UpdateLogFiles][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}
				
				if (m_thread_console != nullptr && !m_thread_console->isLive())
					m_thread_console->init_thread();
		
				for (size_t i = 0; i < m_threads.size(); i++)
					if (m_threads[i] != nullptr && !m_threads[i]->isLive())
						m_threads[i]->init_thread();
				
				try {
				
					m_session_manager.checkSessionLive();
				
				}catch (exception& e) {
					_smp::message_pool::getInstance().push(new message("[unit::Monitor][ErrorSystem]" + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}
		
				try {

					m_si.curr_user = m_session_manager.getNumSessionOnline();

					// Register Server
					NormalManagerDB::add(0, new CmdRegisterServer(m_si), unit::SQLDBResponse, this);
					
				}catch (exception& e) {
					
					// Query UPDATE não retorna nada ai retorna essa exception
					if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PANGYA_DB, 2)
						&& STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) != 3) {
						
						_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
					
					}else if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::EXEC_QUERY, 3)) {
					
						_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
					
					}else if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PANGYA_DB, 2)
						&& !STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::EXEC_QUERY, 7)) {
						
						throw;
					}
				}

				NormalManagerDB::checkIsDeadAndRevive();
		
				cmdUpdateServerList();	// Pega a Lista de servidores online

				onHeartBeat();
		
#if defined(_WIN32)
				Sleep(1000); // 1 second para próxima verificação
#elif defined(__linux__)
				usleep(1000000); // 1 second para próxima verificação
#endif

			}catch (exception& e) {
				
				_smp::message_pool::getInstance().push(new message("[unit::monitor][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				
				if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::THREAD)
					throw;
			}
		}

	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[unit::monitor][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (std::exception& e) {
		_smp::message_pool::getInstance().push(new message(std::string("[unit::monitor][ErrorSystem] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "[unit::monitor][ErrorSystem] monitor() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}
		
	_smp::message_pool::getInstance().push(new message("Saindo de monitor()..."));
	
#if defined(_WIN32)
	return 0u;
#elif defined(__linux__)
	return (void*)0u;
#endif
};

void unit::waitAllThreadFinish(DWORD dwMilleseconds) {
	// Monitor Thread
#if defined(_WIN32)
	InterlockedDecrement(&m_continue_monitor);
#elif defined(__linux__)
	__atomic_sub_fetch(&m_continue_monitor, 1, __ATOMIC_RELAXED);
#endif

	if (m_thread_monitor != nullptr)
		m_thread_monitor->waitThreadFinish(dwMilleseconds);

	for (size_t i = 0; i < m_threads.size(); i++) {

		switch (m_threads[i]->getTipo()) {
		case TT_WORKER_IO:
			for (auto io = 0u; io < (sizeof(m_iocp_io) / sizeof(iocp)); ++io)
#if defined(_WIN32)
				m_iocp_io[io].postStatus((ULONG_PTR)nullptr, 0, nullptr);
#elif defined(__linux__)
				m_iocp_io[io].postStatus();
#endif
			break;
		case TT_WORKER_IO_SEND:
#if defined(_WIN32)
			for (auto io = 0u; io < (sizeof(m_iocp_io_send) / sizeof(iocp)); ++io)
				m_iocp_io_send[io].postStatus((ULONG_PTR)nullptr, 0, nullptr);
#elif defined(__linux__)
			for (auto io = 0u; io < 16u; ++io)
				m_iocp_io_send[io].push(nullptr);
#endif
			break;
		case TT_WORKER_IO_RECV:
#if defined(_WIN32)
			for (auto io = 0u; io < (sizeof(m_iocp_io_recv) / sizeof(iocp)); ++io)
				m_iocp_io_recv[io].postStatus((ULONG_PTR)nullptr, 0, nullptr);
#elif defined(__linux__)
			for (auto io = 0u; io < 16u; ++io)
				m_iocp_io_recv[io].push(nullptr);
#endif
			break;
		case TT_WORKER_LOGICAL:
			//for (auto io = 0u; io < (sizeof(m_iocp_logical) / sizeof(iocp)); ++io)
#if defined(_WIN32)
			m_iocp_logical.postStatus((ULONG_PTR)nullptr, 0, nullptr);			// Esse tinha Index[io], era um array de 16 iocp
#elif defined(__linux__)
			m_iocp_logical.push(nullptr);										// Esse tinha Index[io], era um array de 16 iocp
#endif
			break;
		case TT_WORKER_SEND:
			//for (auto io = 0u; io < (sizeof(m_iocp_send) / sizeof(iocp)); ++io)
#if defined(_WIN32)
			m_iocp_send.postStatus((ULONG_PTR)nullptr, 0, nullptr);				// Esse tinha Index[io], era um array de 16 iocp
#elif defined(__linux__)
			m_iocp_send.push(nullptr);											// Esse tinha Index[io], era um array de 16 iocp
#endif
			break;
		case TT_ACCEPT:
			// Libera o accept se ele ainda não estiver liberado
			setAcceptConnection();
#if defined(_WIN32)
			InterlockedDecrement(&m_continue_accpet);
#elif defined(__linux__)
			__atomic_sub_fetch(&m_continue_accpet, 1, __ATOMIC_RELAXED);
#endif
			break;
		case TT_ACCEPTEX_IO:
#if defined(_WIN32)
			m_iocp_io_accept.postStatus((ULONG_PTR)INVALID_SOCKET, 0, nullptr);
#elif defined(__linux__)
			m_iocp_io_accept.postStatus();
#endif
			break;
		case TT_JOB:
			m_job_pool.push(nullptr);			// Sai do job
			break;
		case TT_CONSOLE:
			_smp::message_pool::getInstance().push(nullptr);	// Sai do console
			break;
		case TT_DB_NORMAL:
			pangya_db::m_query_pool.push(nullptr);	// sai do db query pool
			break;
		case TT_MONITOR:
#if defined(_WIN32)
			InterlockedDecrement(&m_continue_monitor);
			SetEvent(EventShutdownServer);
#elif defined(__linux__)
			__atomic_sub_fetch(&m_continue_monitor, 1, __ATOMIC_RELAXED);
			
			if (EventShutdownServer != nullptr)
				EventShutdownServer->set();
#endif
			break;
		case TT_DISCONNECT_SESSION:
#if defined(_WIN32)
			InterlockedDecrement(&m_atomic_disconnect_session);
#elif defined(__linux__)
			__atomic_sub_fetch(&m_atomic_disconnect_session, 1, __ATOMIC_RELAXED);
#endif
			break;
		}
	}

	for (size_t i = 0; i < m_threads.size(); i++)
		if (m_threads[i] != nullptr)
			m_threads[i]->waitThreadFinish(dwMilleseconds);

	_smp::message_pool::getInstance().push(nullptr);		// Sai console;

	if (m_thread_console != nullptr)
		m_thread_console->waitThreadFinish(dwMilleseconds);
};

#if defined(_WIN32)
DWORD unit::accept() {
#elif defined(__linux__)
void* unit::accept() {
#endif
	
#if defined(_WIN32)
	SOCKET mHost = INVALID_SOCKET;
	SOCKET accpt = INVALID_SOCKET;
	HANDLE _event = INVALID_HANDLE_VALUE;
#elif defined(__linux__)
	SOCKET mHost = { INVALID_SOCKET, 0u };
	SOCKET accpt = { INVALID_SOCKET, 0u };
	//Event *_event = nullptr;
#endif
	
	session *_session = nullptr;
		
	BOOL tcp_nodelay = 1;
	//int so_sndbuf = 0;
	//int so_timeout = 0;

	socketserver *sv = nullptr;
	socket *sock = nullptr;
	
	DWORD ret = 0;
	
	try {
	
		_smp::message_pool::getInstance().push(new message("[unit::accept][Log] accept iniciado com sucesso!"));
	
		sv = new socketserver();
	
		SOCKADDR_IN addr, remote;
#if defined(_WIN32)
		int length = sizeof(remote);
#elif defined(__linux__)
		socklen_t length = sizeof(remote);
#endif
	
		addr.sin_family = AF_INET;	// depois coloca o host aqui // m_si.m_ip
		addr.sin_port = htons(m_si.port);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
		sock = new socket(sv->getsocklistener(), 1);
		
		// pega a referencia do sock que controla as conexão que pode logar
		m_accept_sock = sock;

		sock->bind(addr);
	
		_smp::message_pool::getInstance().push(new message("[unit::accept][Log] Esperando pelo Evento Accept Connection to listen on socket"));

#if defined(_WIN32)
		if (WaitForSingleObject(EventAcceptConnection, INFINITE) != WAIT_OBJECT_0) {
#elif defined(__linux__)
		if (EventAcceptConnection == nullptr)
			return (void*)-1;
		
		if (EventAcceptConnection->wait(INFINITE) != WAIT_OBJECT_0) {
#endif

			_smp::message_pool::getInstance().push(new message("[unit::accept][Error] nao conseguiu esperar pelo evento de aceitar coneccoes. ErrCode: " + std::to_string(
#if defined(_WIN32)
				GetLastError()
#elif defined(__linux__)
				errno
#endif
			), CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
			return (DWORD)(-1);	// Error on Wait Event Accept Connection
#elif defined(__linux__)
			return (void*)(-1);	// Error on Wait Event Accept Connection
#endif
		}

		sock->listen(10000);

		_smp::message_pool::getInstance().push(new message("[unit::accept][Log] Escutando na porta: " + std::to_string(m_si.port), CL_FILE_LOG_AND_CONSOLE));
	
		mHost = sock->detatch();
	
#if defined(_WIN32)
		if ((_event = WSACreateEvent()) == WSA_INVALID_EVENT)
			throw exception("[unit::accept][Error] Nao conseguiu criar WSAEvent. server::accept()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 10, WSAGetLastError()));
	
		if (WSAEventSelect(mHost, _event, FD_ACCEPT) == SOCKET_ERROR)
			throw exception("[unit::accept][Error] Nao conseguiu selecionar WSAEvent FD_ACCEPT. server::accept()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 11, WSAGetLastError()));
	
		WSANETWORKEVENTS events;
	
		while (InterlockedCompareExchange(&m_continue_accpet, 1, 1)) {
			try {
	
				ret = WSAWaitForMultipleEvents(1, &_event, FALSE, 100, FALSE);
					
				if (!InterlockedCompareExchange(&m_continue_accpet, 1, 1))
					break;
	
				if (ret == WSA_WAIT_TIMEOUT)
					continue;
	
				if ((ret = WSAEnumNetworkEvents(mHost, _event, &events)) == SOCKET_ERROR)
					throw exception("[unit::accept][Error] Nao conseguiu pegar os wsa net work events. server::accept()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 12, WSAGetLastError()));
	
				if (events.lNetworkEvents & FD_ACCEPT) {
	
					if (events.iErrorCode[FD_ACCEPT_BIT] == 0 && InterlockedCompareExchange(&m_continue_accpet, 1, 1)) {
	
						if ((accpt = WSAAccept(mHost, (SOCKADDR*)&remote, &length, NULL, NULL)) != INVALID_SOCKET) {

							int err_accept_sock = -1;

							if (sock->ip_rules && (err_accept_sock = sock->connect_check(accpt, ntohl(remote.sin_addr.s_addr))) <= 0) {

								char ip[25];

								if (inet_ntop(AF_INET, &remote.sin_addr.s_addr, ip, sizeof(ip)) == nullptr)
									throw exception("[unit::accept][Error] ao converter SOCKADDR_IN para string doted mode(IP).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT, 54, 0));

								_smp::message_pool::getInstance().push(new message("[unit::accept][Log] Socket[IP=" + std::string(ip) + "] Negado. " + std::string(err_accept_sock == 0 ? "IP Negado." : "Limite de Conexoes por IP"), CL_FILE_LOG_AND_CONSOLE));

								::shutdown(accpt, SD_BOTH);
								::closesocket(accpt);

								// Desconnectar o socket
								continue;
							}
	
							//_smp::message_pool.push(new message("Player Aceito com sucesso! #LS", CL_FILE_TIME_LOG_AND_CONSOLE));
	
							std::srand(std::clock() * 5);
								
							/// ---------- DESEMPENHO COM OS SOCKOPT -----------
							/// COM NO_TCPDELAY					AVG(MEDIA) 0.552
							/// COM SO_SNDBUF 0					AVG(MEDIA) 0.560
							/// COM SO_RCVBUF 0					AVG(MEDIA) 0.570
							/// COM NO_TCPDELAY e SO_SNDBUF 0	AVG(MEDIA) 0.569
							/// COM NO_TCPDELAY e SO_RCVBUF 0	AVG(MEDIA) 0.566
							/// SEM NENHUM SOCKOPT				AVG(MEDIA) 0.569
							// Não tem muita diferença vou deixar só o NO_TCPDELAY mesmo
	
							//if (setsockopt(accpt, SOL_SOCKET, SO_SNDBUF, (char*)&so_sndbuf, sizeof(int)) == SOCKET_ERROR)
							//	_smp::message_pool.push(new message("nao conseguiu setar SO_SNDBUF para 0, threadpl_server::accept(). error Code: " + std::to_string(STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 2, WSAGetLastError())), CL_FILE_LOG_AND_CONSOLE));
	
							//if (setsockopt(accpt, SOL_SOCKET, SO_RCVTIMEO, (char*)&so_timeout, sizeof(int)) == SOCKET_ERROR)
							//	_smp::message_pool.push(new message("nao conseguiu set o recive time out. server::accept(). error Code: " + std::to_string(STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 2, WSAGetLastError())), CL_FILE_LOG_AND_CONSOLE));
	
							//if (setsockopt(accpt, SOL_SOCKET, SO_SNDTIMEO, (char*)&so_timeout, sizeof(int)) == SOCKET_ERROR)
							//	_smp::message_pool.push(new message("nao conseguiu set o send time out. server::accept(). error Code: " + std::to_string(STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 2, WSAGetLastError())), CL_FILE_LOG_AND_CONSOLE));
	
							if (setsockopt(accpt, IPPROTO_TCP, TCP_NODELAY, (char*)&tcp_nodelay, sizeof(tcp_nodelay)) == SOCKET_ERROR)
								_smp::message_pool::getInstance().push(new message("[unit::accept][WARNIG] nao conseguiu desabilitar tcp delay(nagle algorithm). error Code: " + std::to_string(STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 2, WSAGetLastError())), CL_FILE_LOG_AND_CONSOLE));
	
							// Log
							_smp::message_pool::getInstance().push(new message("[unit::accept][Log] Socket Accept[ID=" + std::to_string(accpt)
									+ ", Local ADDR=" + std::to_string(ntohl(addr.sin_addr.s_addr)) + ", Remote ADDR=" + std::to_string(ntohl(remote.sin_addr.s_addr)) + "]", CL_FILE_LOG_AND_CONSOLE));

							_session = m_session_manager.addSession(accpt, remote, std::rand() % 16);
							
#ifdef _DEBUG
							_smp::message_pool::getInstance().push(new message("[unit::accept][Log] Aceitou uma nova conexao. com IP: " + std::string(_session->getIP()) + ", e com a chave: " + std::to_string((int)_session->m_key) + ", e OID: " + std::to_string(_session->m_oid), CL_FILE_LOG_AND_CONSOLE));
#else
							_smp::message_pool::getInstance().push(new message("[unit::accept][Log] Aceitou uma nova conexao. com IP: " + std::string(_session->getIP()) + ", e com a chave: " + std::to_string((int)_session->m_key) + ", e OID: " + std::to_string(_session->m_oid), CL_ONLY_FILE_LOG));
#endif
	
							m_iocp_io[_session->m_key].associaDeviceToPort((ULONG_PTR)_session, (HANDLE)accpt);
	
							onAcceptCompleted(_session);
	
							_session = nullptr;	// reseta aqui para usar ela de novo
						}

					}else
						throw exception("[unit::accept][Error] WSA NetWork event unknown or shutdown. server::accept()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 13, WSAGetLastError()));
				}

			}catch (exception e) {
				
				_smp::message_pool::getInstance().push(new message("[unit::accept][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	
				if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::PLAYER_MANAGER || STDA_ERROR_DECODE(e.getCodeError()) != 3/*checou ao limit de sesssion*/)	// Diferente do erro chegou ao limite de usuarios online
					throw;
				else {
					// aqui envia algo ou so deleta o sock
					if (_session != nullptr)
						m_session_manager.deleteSession(_session);
					else if (accpt != INVALID_SOCKET) {
						::shutdown(accpt, SD_BOTH);
						::closesocket(accpt);
					}
				}
			}
		}
#elif defined(__linux__)
		fd_set fd_accept;
		FD_ZERO(&fd_accept);
		FD_SET(mHost.fd, &fd_accept);

		struct timeval accept_timeout;
		accept_timeout.tv_sec = 0;
		accept_timeout.tv_usec = 100000;
	
		int32_t check_m = 1; // Compare
		while (__atomic_compare_exchange_n(&m_continue_accpet, &check_m, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
			try {

				// Clear
				FD_SET(mHost.fd, &fd_accept);

				// set again, select clean it
				accept_timeout.tv_sec = 0;
				accept_timeout.tv_usec = 100000;

				ret = select(mHost.fd + 1, &fd_accept, nullptr, nullptr, &accept_timeout);
					
				if (!__atomic_compare_exchange_n(&m_continue_accpet, &check_m, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
					break;
	
				if (ret == 0/*timeout*/)
					continue;
	
				if (ret == -1/*Error*/)
					throw exception("[unit::accept][Error] nao conseguiu selecionar o socket accept com select.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 12, errno));
	
				if ((accpt.fd = ::accept(mHost.fd, (sockaddr*)&remote, &length)) != INVALID_SOCKET) {

					int err_accept_sock = -1;

					if (sock->ip_rules && (err_accept_sock = sock->connect_check(accpt, ntohl(remote.sin_addr.s_addr))) <= 0) {

						char ip[25];

						if (inet_ntop(AF_INET, &remote.sin_addr.s_addr, ip, sizeof(ip)) == nullptr)
							throw exception("[unit::accept][Error] ao converter SOCKADDR_IN para string doted mode(IP).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT, 54, 0));

						_smp::message_pool::getInstance().push(new message("[unit::accept][Log] Socket[IP=" + std::string(ip) + "] Negado. " + std::string(err_accept_sock == 0 ? "IP Negado." : "Limite de Conexoes por IP"), CL_FILE_LOG_AND_CONSOLE));

						::shutdown(accpt.fd, SD_BOTH);
						::closesocket(accpt.fd);

						// Desconnectar o socket
						continue;
					}

					//_smp::message_pool.push(new message("Player Aceito com sucesso! #LS", CL_FILE_TIME_LOG_AND_CONSOLE));

					std::srand(std::clock() * 5);
						
					/// ---------- DESEMPENHO COM OS SOCKOPT -----------
					/// COM NO_TCPDELAY					AVG(MEDIA) 0.552
					/// COM SO_SNDBUF 0					AVG(MEDIA) 0.560
					/// COM SO_RCVBUF 0					AVG(MEDIA) 0.570
					/// COM NO_TCPDELAY e SO_SNDBUF 0	AVG(MEDIA) 0.569
					/// COM NO_TCPDELAY e SO_RCVBUF 0	AVG(MEDIA) 0.566
					/// SEM NENHUM SOCKOPT				AVG(MEDIA) 0.569
					// Não tem muita diferença vou deixar só o NO_TCPDELAY mesmo

					//if (setsockopt(accpt, SOL_SOCKET, SO_SNDBUF, (char*)&so_sndbuf, sizeof(int)) == SOCKET_ERROR)
					//	_smp::message_pool.push(new message("nao conseguiu setar SO_SNDBUF para 0, threadpl_server::accept(). error Code: " + std::to_string(STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 2, WSAGetLastError())), CL_FILE_LOG_AND_CONSOLE));

					//if (setsockopt(accpt, SOL_SOCKET, SO_RCVTIMEO, (char*)&so_timeout, sizeof(int)) == SOCKET_ERROR)
					//	_smp::message_pool.push(new message("nao conseguiu set o recive time out. server::accept(). error Code: " + std::to_string(STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 2, WSAGetLastError())), CL_FILE_LOG_AND_CONSOLE));

					//if (setsockopt(accpt, SOL_SOCKET, SO_SNDTIMEO, (char*)&so_timeout, sizeof(int)) == SOCKET_ERROR)
					//	_smp::message_pool.push(new message("nao conseguiu set o send time out. server::accept(). error Code: " + std::to_string(STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 2, WSAGetLastError())), CL_FILE_LOG_AND_CONSOLE));

					int flag = O_NONBLOCK;
					if (fcntl(accpt.fd, F_SETFL, flag) != 0)
						throw exception("[unit::accept][Error] nao conseguiu habilitar o NONBLOCK(fcntl).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 53, errno));

					if (setsockopt(accpt.fd, IPPROTO_TCP, TCP_NODELAY, (char*)&tcp_nodelay, sizeof(tcp_nodelay)) == SOCKET_ERROR)
						_smp::message_pool::getInstance().push(new message("[unit::accept][WARNIG] nao conseguiu desabilitar tcp delay(nagle algorithm). error Code: " 
								+ std::to_string(STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 2, errno)), CL_FILE_LOG_AND_CONSOLE));

					// Log
					_smp::message_pool::getInstance().push(new message("[unit::accept][Log] Socket Accept[ID=" + std::to_string(accpt.fd)
							+ ", Local ADDR=" + std::to_string(ntohl(addr.sin_addr.s_addr)) + ", Remote ADDR=" + std::to_string(ntohl(remote.sin_addr.s_addr)) + "]", CL_FILE_LOG_AND_CONSOLE));

					// Set time conncetion accepted
					clock_gettime(SESSION_CONNECT_TIME_CLOCK, &accpt.connect_time);

					_session = m_session_manager.addSession(accpt, remote, std::rand() % 16);
					
#ifdef _DEBUG
					_smp::message_pool::getInstance().push(new message("[unit::accept][Log] Aceitou uma nova conexao. com IP: " + std::string(_session->getIP()) + ", e com a chave: " + std::to_string((int)_session->m_key) + ", e OID: " + std::to_string(_session->m_oid), CL_FILE_LOG_AND_CONSOLE));
#else
					_smp::message_pool::getInstance().push(new message("[unit::accept][Log] Aceitou uma nova conexao. com IP: " + std::string(_session->getIP()) + ", e com a chave: " + std::to_string((int)_session->m_key) + ", e OID: " + std::to_string(_session->m_oid), CL_ONLY_FILE_LOG));
#endif

					epoll_event ep_ev;
					ep_ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLHUP | EPOLLRDHUP;
					//ep_ev.data.fd = accpt.fd;
					ep_ev.data.ptr = _session;

					m_iocp_io[_session->m_key].associaDeviceToPort((uint32_t)accpt.fd, ep_ev);

					onAcceptCompleted(_session);

					_session = nullptr;	// reseta aqui para usar ela de novo
				}

			}catch (exception e) {
				
				_smp::message_pool::getInstance().push(new message("[unit::accept][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	
				if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::PLAYER_MANAGER || STDA_ERROR_DECODE(e.getCodeError()) != 3/*checou ao limit de sesssion*/)	// Diferente do erro chegou ao limite de usuarios online
					throw;
				else {
					// aqui envia algo ou so deleta o sock
					if (_session != nullptr)
						m_session_manager.deleteSession(_session);
					else if (accpt.fd != INVALID_SOCKET) {
						::shutdown(accpt.fd, SD_BOTH);
						::closesocket(accpt.fd);
					}
				}
			}
		}
#endif

	}catch (exception &e) {
		_smp::message_pool::getInstance().push(new message("[unit::accept][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (std::exception& e) {
		_smp::message_pool::getInstance().push(new message("[unit::accept][ErrorSystem] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "accept() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}
	
	// Clean
#if defined(_WIN32)
	if (mHost != INVALID_SOCKET) {

		::closesocket(mHost);

		mHost = INVALID_SOCKET;
	}
#elif defined(__linux__)
	if (mHost.fd != INVALID_SOCKET) {

		::closesocket(mHost.fd);

		mHost.fd = INVALID_SOCKET;
	}
#endif

	if (sv != nullptr) {

		delete sv;

		sv = nullptr;
	}

	if (sock != nullptr) {

		delete sock;

		sock = nullptr;
	}

	m_accept_sock = nullptr;
	
	_smp::message_pool::getInstance().push(new message("Saindo de accept()..."));
	
#if defined(_WIN32)
	return 0u;
#elif defined(__linux__)
	return (void*)0u;
#endif
};

#if defined(_WIN32)
DWORD unit::disconnect_session() {
#elif defined(__linux__)
void* unit::disconnect_session() {
#endif

	try {
		
		_smp::message_pool::getInstance().push(new message("[unit::disconnect_session][Log] disconnect_session iniciado com sucesso!"));
		
#if defined(_WIN32)
		while (InterlockedCompareExchange(&m_atomic_disconnect_session, 1, 1)) {
#elif defined(__linux__)
		int32_t check_m = 1; // Compare
		while (__atomic_compare_exchange_n(&m_atomic_disconnect_session, &check_m, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
#endif
			
			try {
				
				try {

					if (session_manager::isInit()) {

						auto s = m_session_manager.getSessionToDelete(1000/*1 second para libera o while se não tiver session para desconectar*/);

						if (s != nullptr)
							DisconnectSession(s);

					}else
#if defined(_WIN32)
						Sleep(300/*espera 300 miliseconds até o session_manager ser inicializado*/);
#elif defined(__linux__)
						usleep(300000/*espera 300 miliseconds até o session_manager ser inicializado*/);
#endif
				
				}catch (exception& e) {

					_smp::message_pool::getInstance().push(new message("[unit::disconnect_session][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}
		
			}catch (exception& e) {

				if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::THREAD)
					throw;
			}
		}

	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[unit::disconnect_session][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (std::exception& e) {
		_smp::message_pool::getInstance().push(new message(std::string("[unit::disconnect_session][ErrorSystem] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		_smp::message_pool::getInstance().push(new message("[unit::disconnect_session][ErrorSystem] disconnect_session-> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)", CL_FILE_LOG_AND_CONSOLE));
	}
		
	_smp::message_pool::getInstance().push(new message("Saindo de disconnect_session()..."));

#if defined(_WIN32)
	return 0u;
#elif defined(__linux__)
	return (void*)0u;
#endif
};

inline void unit::dispach_packet_same_thread(session& _session, packet *_packet) {
	//ParamWorker pw = { *this, m_iocp_io, m_job_pool, m_session_pool, m_pangya_db, _packet };
	func_arr::func_arr_ex* func = nullptr;

	try {
		
		func = packet_func_base::funcs.getPacketCall(_packet->getTipo());
	
	}catch (exception& e) {
		
		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::FUNC_ARR/*Packet_func Erro, Warning e etc*/) {
			
			_smp::message_pool::getInstance().push(new message("[unit::dispach_packet_same_thread][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[unit::dispach_packet_same_thread][Log] Pacote desconhecido enviado pelo Player[UID=" + std::to_string(_session.getUID()) + ", OID=" + std::to_string(_session.m_oid) + "] size packet: "
					+ std::to_string(_packet->getSize()) + "\n" + hex_util::BufferToHexString(_packet->getBuffer(), _packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#else
			_smp::message_pool::getInstance().push(new message("[unit::dispach_packet_same_thread][Log] Pacote desconhecido enviado pelo Player[UID=" + std::to_string(_session.getUID()) + ", OID=" + std::to_string(_session.m_oid) + "] size packet: "
					+ std::to_string(_packet->getSize()) + "\n" + hex_util::BufferToHexString(_packet->getBuffer(), _packet->getSize()), CL_ONLY_FILE_LOG));
#endif

			// Auth server só os outros servers que conectam, então não vai ter pacote que ainda não implementei
			DisconnectSession(&_session);

			// Trata o erro aqui
			// e desloga a session que enviou pacote errado
			/// colocar de novo ------> m_pe->_session_pool.deleteSession(_packet->getSession());
			// se for muito grave relança ele para terminar a thread
		}else
			DisconnectSession(&_session);
		
		// Relança a exception
		throw;
	}

	try {

		_session.usa();

		// só pode contar o tick se for um pacote conhecido pelo, server, se contar por dados recebido, nunca vai derrubar o cliente mau intensionado
		_session.m_tick = std::clock();	// Tick time client para o TTL(time to live)

		ParamDispatch pd = { *(player*)&_session, _packet };

		if (checkPacket(_session, _packet)/*Check Packet*/) {
			/*_session.m_check_packet.tick = std::clock();
			
			if (_session.m_check_packet.packet_id != _packet->getTipo())
				_session.m_check_packet.count = 0;
			
			_session.m_check_packet.packet_id = _packet->getTipo();*/

			if (func != nullptr && func->execCmd(&pd) != 0)
				_smp::message_pool::getInstance().push(new message("[unit::dispach_packet_same_thread][Error] Ao tratar o pacote. ID: " + std::to_string(_packet->getTipo()) 
						+ "(0x" + hex_util::ltoaToHex(_packet->getTipo()) + ").", CL_FILE_LOG_AND_CONSOLE));
		}

		if (_session.devolve())
			DisconnectSession(&_session);

	}catch (exception& e) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[unit::dispach_packet_same_thread][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*não pode usa session*/))
			if (_session.devolve())
				DisconnectSession(&_session);

	}catch (std::exception& e) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message(std::string("[unit::dispach_packet_same_thread][Error] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));
#else
		UNREFERENCED_PARAMETER(e);
#endif

		if (_session.devolve())
			DisconnectSession(&_session);

	}catch (...) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[unit::dispach_packet_same_thread][Error] Unknown Error", CL_FILE_LOG_AND_CONSOLE));
#endif

		if (_session.devolve())
			DisconnectSession(&_session);
	}
};

inline void unit::dispach_packet_sv_same_thread(session& _session, packet *_packet) {
	//ParamWorker pw = { *this, m_iocp_io, m_job_pool, m_session_pool, m_pangya_db, _packet };
	func_arr::func_arr_ex* func = nullptr;

	try {
		
		func = packet_func_base::funcs_sv.getPacketCall(_packet->getTipo());
	
	}catch (exception& e) {
		
		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::FUNC_ARR/*Packet_func Erro, Warning e etc*/) {
			
			_smp::message_pool::getInstance().push(new message("[unit::dispach_packet_sv_same_thread][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[unit::dispach_packet_sv_same_thread][Log] Pacote desconhecido enviado pelo server para o  Player[UID=" + std::to_string(_session.getUID())
					+ ", OID=" + std::to_string(_session.m_oid) + "]size packet: " + std::to_string(_packet->getSize()) + "\n" + hex_util::BufferToHexString(_packet->getBuffer(), _packet->getSize()), CL_ONLY_FILE_LOG));
#endif

			// Trata o erro aqui
			// e desloga a session que enviou pacote errado
			/// colocar de novo ------> m_pe->_session_pool.deleteSession(_packet->getSession());
			// se for muito grave relança ele para terminar a thread
		}else
			throw;
	}

	try {

		_session.usa();

		ParamDispatch pd = { *(player*)&_session, _packet };

		if (func != nullptr && func->execCmd(&pd) != 0)
			_smp::message_pool::getInstance().push(new message("[unit::dispach_packet_sv_same_thread][Error] Ao tratar o pacote. ID: " + std::to_string(_packet->getTipo()) 
					+ "(0x" + hex_util::ltoaToHex(_packet->getTipo()) + ").", CL_FILE_LOG_AND_CONSOLE));
	
		if (_session.devolve())
			DisconnectSession(&_session);

	}catch (exception& e) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[unit::dispach_packet_same_thread][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*não pode usa session*/))
			if (_session.devolve())
				DisconnectSession(&_session);

	}catch (std::exception& e) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message(std::string("[unit::dispach_packet_same_thread][Error] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));
#else
		UNREFERENCED_PARAMETER(e);
#endif

		if (_session.devolve())
			DisconnectSession(&_session);

	}catch (...) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[unit::dispach_packet_same_thread][Error] Unknown Error", CL_FILE_LOG_AND_CONSOLE));
#endif

		if (_session.devolve())
			DisconnectSession(&_session);
	}
};

void unit::accept_completed(SOCKET* /*_listener*/, DWORD /*dwIOsize*/, myOver* /*lpBuffer*/, DWORD /*_operation*/) {
};

bool unit::DisconnectSession(session *_session) {

	bool ret = true;

	onDisconnected(_session);

	try {
		
		ret = m_session_manager.deleteSession(_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit::DisconnectSession][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		ret = false;
	}

	return ret;
};

void unit::cmdUpdateServerList() {
	NormalManagerDB::add(1, new CmdServerList(CmdServerList::GAME), unit::SQLDBResponse, this);
	//m_server_list = pangya_base_db::getServerList();
};

void unit::updateServerList(std::vector< ServerInfo >& _v_si) {
	m_server_list = _v_si;
};

void unit::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		_smp::message_pool::getInstance().push(new message("[unit::SQLDBResponse][WARNING] _arg is nullptr, na msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// Por Hora só sai, depois faço outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[unit::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto *_server = reinterpret_cast< unit* >(_arg);

	switch (_msg_id) {
	case 1:	// updateServerList
		_server->updateServerList(reinterpret_cast< CmdServerList* >(&_pangya_db)->getServerList());
		break;
	case 0:
	default:
		break;
	}
};

int unit::end_time_shutdown(void* _arg1, void* _arg2) {

	unit *s = reinterpret_cast<unit*>(_arg1);
	uint32_t time_sec = (uint32_t)(size_t)_arg2;

	try {

		s->shutdown_time(time_sec);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit::end_time_shutdown][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;

	return 0;
};

void unit::start() {

	if (m_state != FAILURE) {

		try {

			// On Start
			onStart();

			// Set Accept Connection for Starting Service
			setAcceptConnection();

			commandScan();

			_smp::message_pool::getInstance().push(new message("Saindo..."));

			waitAllThreadFinish(INFINITE);

		}catch (exception& e) {
			_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}catch (std::exception& e) {
			_smp::message_pool::getInstance().push(new message(e.what(), CL_FILE_LOG_AND_CONSOLE));
		}catch (...) {
			_smp::message_pool::getInstance().push(new message("Error desconhecido. server::start()", CL_FILE_LOG_AND_CONSOLE));
		}
	}else {
		_smp::message_pool::getInstance().push(new message("Error Server Inicializado com falha, fechando o server.", CL_FILE_LOG_AND_CONSOLE));
		
		waitAllThreadFinish(INFINITE);
	}
};

uint32_t unit::getUID() {
	return m_si.uid;
};

void unit::shutdown() {

#if defined(_WIN32)
	if (EventShutdownServer != INVALID_HANDLE_VALUE) {

		SetEvent(EventShutdownServer);

		CloseHandle(EventShutdownServer);
	}

	EventShutdownServer = INVALID_HANDLE_VALUE;
#elif defined(__linux__)
	if (EventShutdownServer != nullptr)
		EventShutdownServer->set();
#endif
}

void unit::setAcceptConnection() {

#if defined(_WIN32)
	if (EventAcceptConnection != INVALID_HANDLE_VALUE)
		SetEvent(EventAcceptConnection);
#elif defined(__linux__)
	if (EventAcceptConnection != nullptr)
		EventAcceptConnection->set();
#endif
};

void unit::commandScan() {

#if defined(_WIN32)
	HANDLE events[] = { EventShutdownServer, GetStdHandle(STD_INPUT_HANDLE) };

	char command[2] = { '\0', '\0' };

	DWORD ret = 0u;
	std::string s;

	bool shutdown = false;

	while (!shutdown) {

		ret = WaitForMultipleObjects((sizeof(events) / sizeof(events[0])), events, FALSE, 1000);

		switch (ret) {
			case (WAIT_OBJECT_0 + 1):	// Std In 
			{

				if (_kbhit()) {
					//std::cin.getline(command, sizeof(command));

					command[0] = _getch();

					// Recebeu um valor que é de 2 bytes, então ignora esse
					if (command[1] < 0) {

						command[1] = '\0';
						
						continue;
					}

					if (command[0] == '\b') {
						
						if (!s.empty()) {
							
							s.pop_back();

							_putch('\b');
							_putch(' ');
							_putch('\b');	// Clear Screen

						}else
							s.shrink_to_fit();

					}

					if (command[0] == '\n' || command[0] == '\r' && !s.empty()) {

						std::stringstream ss(s);

						// Reseta std::string s
						if (!s.empty()) {
							s.clear();
							s.shrink_to_fit();
						}

						while (!ss.eof()) {
							if (checkCommand(ss)) {

								shutdown = true;	// Sai do While Principal

								break;	// Sai do While de comandos
							}
						}

					}else if (command[0] != 0 && isascii(command[0]) && (isalpha(command[0]) && isalnum(command[0])) || (command[0] > 0 && isdigit(command[0]))
							|| command[0] == '_' || command[0] == ' ') {

						s.push_back(command[0]);

						// Show Key on Console
						_putch(command[0]);
						
					}else
						command[1] = command[0];

				}else
					FlushConsoleInputBuffer(events[1]);

				break;
			}
			case WAIT_OBJECT_0:	// Shutdown
			{
				_smp::message_pool::getInstance().push(new message("[unit::commandScan][Log] Event Shutdown Set, Desligando o server.", CL_FILE_LOG_AND_CONSOLE));

				shutdown = true;	// Shutdown

				break;
			}
		}
	}
#elif defined(__linux__)

	char command[2] = { '\0', '\0' };

	DWORD ret = 0u;
	std::string s;

	bool shutdown = false;

	while (!shutdown) {

		if (_kbhit()) {

			command[0] = _getch();

			// Recebeu um valor que é de 2 bytes, então ignora esse
			if (command[1] < 0) {

				command[1] = '\0';
				
				continue;
			}

			if (command[0] == '\b' || command[0] == '\177') {
				
				if (!s.empty()) {
					
					s.pop_back();

					_putch(command[0]);
					_putch(' ');
					_putch(command[0]);	// Clear Screen

				}else
					s.shrink_to_fit();

			}

			if (command[0] == '\n' || command[0] == '\r' && !s.empty()) {

				std::stringstream ss(s);

				// Reseta std::string s
				if (!s.empty()) {
					s.clear();
					s.shrink_to_fit();
				}

				// put break line
				_putch('\n');
				_putch('\r');

				while (!ss.eof()) {
					if (checkCommand(ss)) {

						shutdown = true;	// Sai do While Principal

						break;	// Sai do While de comandos
					}
				}

			}else if (command[0] != 0 && isascii(command[0]) && (isalpha(command[0]) && isalnum(command[0])) || (command[0] > 0 && isdigit(command[0])) 
				|| command[0] == '_' || command[0] == ' ') {

				s.push_back(command[0]);

				// Show Key on Console
				_putch(command[0]);
				
			}else
				command[1] = command[0];

		}else if (EventShutdownServer != nullptr) {

			// Wait shutdown
			if (EventShutdownServer->wait(100) == WAIT_OBJECT_0) {

				_smp::message_pool::getInstance().push(new message("[unit::commandScan][Log] Event Shutdown Set, Desligando o server.", CL_FILE_LOG_AND_CONSOLE));

				shutdown = true;	// Shutdown
			}
		}
	}
#endif
};

void unit::config_init() {

	m_reader_ini.init();

#if defined(_WIN32)
	memcpy_s(m_si.nome, sizeof(m_si.nome), m_reader_ini.readString("SERVERINFO", "NAME").c_str(), sizeof(m_si.nome));
	memcpy_s(m_si.version, sizeof(m_si.version), m_reader_ini.readString("SERVERINFO", "VERSION").c_str(), sizeof(m_si.version));
	memcpy_s(m_si.version_client, sizeof(m_si.version_client), m_reader_ini.readString("SERVERINFO", "CLIENTVERSION").c_str(), sizeof(m_si.version_client));
	memcpy_s(m_si.ip, sizeof(m_si.ip), m_reader_ini.readString("SERVERINFO", "IP").c_str(), sizeof(m_si.ip));
#elif defined(__linux__)
	memcpy(m_si.nome, m_reader_ini.readString("SERVERINFO", "NAME").c_str(), sizeof(m_si.nome));
	memcpy(m_si.version, m_reader_ini.readString("SERVERINFO", "VERSION").c_str(), sizeof(m_si.version));
	memcpy(m_si.version_client, m_reader_ini.readString("SERVERINFO", "CLIENTVERSION").c_str(), sizeof(m_si.version_client));
	memcpy(m_si.ip, m_reader_ini.readString("SERVERINFO", "IP").c_str(), sizeof(m_si.ip));
#endif

	m_si.uid = m_reader_ini.readInt("SERVERINFO", "GUID");
	m_si.port = m_reader_ini.readInt("SERVERINFO", "PORT");
	m_si.max_user = m_reader_ini.readInt("SERVERINFO", "MAXUSER");
	m_si.propriedade.ulProperty = m_reader_ini.readInt("SERVERINFO", "PROPERTY");
	
	// Pega na classe de DB msm, aqui não pega mais
	/*memcpy_s(m_ctx_db.ip, sizeof(m_ctx_db.ip), m_reader_ini.readString("DATABASE", "DBIP").c_str(), sizeof(m_ctx_db.ip));
	memcpy_s(m_ctx_db.name, sizeof(m_ctx_db.name), m_reader_ini.readString("DATABASE", "DBNAME").c_str(), sizeof(m_ctx_db.name));
	memcpy_s(m_ctx_db.user, sizeof(m_ctx_db.user), m_reader_ini.readString("DATABASE", "DBUSER").c_str(), sizeof(m_ctx_db.user));
	memcpy_s(m_ctx_db.pass, sizeof(m_ctx_db.pass), m_reader_ini.readString("DATABASE", "DBPASSWORD").c_str(), sizeof(m_ctx_db.pass));

	try {
		m_ctx_db.port = m_reader_ini.readInt("DATABASE", "DBPORT");
	}catch (exception& e) {
		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::READER_INI, 6))
			throw;

		m_ctx_db.port = 0;
	}*/

	_smp::message_pool::getInstance().push(new message("ServerInfo:[NAME=" + std::string(m_si.nome) + "][GUID=" + std::to_string(m_si.uid)
			+ "][IP=" + std::string(m_si.ip) + "][PORT=" + std::to_string(m_si.port)
			+ "][MAXUSER=" + std::to_string(m_si.max_user) + "][VERSION=" + std::string(m_si.version) + "]", CL_FILE_LOG_AND_CONSOLE));

};
