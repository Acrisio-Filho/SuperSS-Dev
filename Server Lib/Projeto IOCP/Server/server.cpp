// Arquivo server.cpp
// Criado em 17/12/2017 por Acrisio
// Implementação da classe server

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mstcpip.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <signal.h>
#endif

#include "server.h"
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

#include "../Smart Calculator/Smart Calculator.hpp"

#include "../../Projeto IOCP/PANGYA_DB/cmd_list_ip_ban.hpp"
#include "../../Projeto IOCP/PANGYA_DB/cmd_list_mac_ban.hpp"

#if defined(_WIN32)
#include <conio.h>

#include <DbgHelp.h>
#elif defined(__linux__)
#include "../UTIL/ConioPort.h"
#endif

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[server::" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 1, 0)); \

using namespace stdA;

server* ssv::sv = nullptr;

server::server(session_manager& _session_manager, uint32_t _accept_thread_num, uint32_t _db_instance_num, uint32_t _job_thread_num)
	: m_session_manager(_session_manager), threadpl_server(16, 16, _job_thread_num), m_si{}, m_server_list(), m_reader_ini(_INI_PATH), m_state(UNINITIALIZED), 
	  m_ctx_db{0}, m_unit_connect(nullptr), 
#if defined(_WIN32)
	  EventShutdownServer(INVALID_HANDLE_VALUE), 
	  EventMoreAccept(INVALID_HANDLE_VALUE), 
	  EventShutdown(INVALID_HANDLE_VALUE), 
	  EventAcceptConnection(INVALID_HANDLE_VALUE),
#elif defined(__linux__)
	  EventShutdownServer(nullptr), 
	  EventMoreAccept(nullptr), 
	  EventShutdown(nullptr), 
	  EventAcceptConnection(nullptr),
#endif
	  m_shutdown(nullptr), m_Bot_TTL(10000u), m_chat_discord(true)/*Padrão ativado*/ {
	
	try {

		// Initializa o Server Static
		ssv::sv = this;

		// Inicializa o Unit_Connect, que conecta com o Auth Server
		m_unit_connect = new unit_auth_server_connect(*this, m_si);

		// Config Initialize, INI FILE
		config_init();

		size_t i = 0;

#if defined(_WIN32)
		InterlockedExchange(&m_acceptsPendents, 0);
#elif defined(__linux__)
		__atomic_store_n(&m_acceptsPendents, 0, __ATOMIC_RELAXED);
#endif

		m_accept_sock = nullptr;

#if defined(_WIN32)
		m_lpfnAcceptEx = nullptr;
		m_lpfnGetAcceptExSockaddrs = nullptr;
#endif

#if defined(_WIN32)
		if ((EventShutdownServer = CreateEvent(NULL, true, false, NULL)) == INVALID_HANDLE_VALUE)
			throw exception("[server::server][Error] ao criar o evento ShutdownServer.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 54, GetLastError()));

		if ((EventShutdown = CreateEvent(NULL, true, false, NULL)) == INVALID_HANDLE_VALUE)
			throw exception("[server::server][Error] ao criar o evento shutdown.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 54, GetLastError()));

		if ((EventMoreAccept = CreateEvent(NULL, true, false, NULL)) == INVALID_HANDLE_VALUE)
			throw exception("[server::server][Error] ao criar o evento more accept.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 54, GetLastError()));

		if ((EventAcceptConnection = CreateEvent(NULL, true, false, NULL)) == INVALID_HANDLE_VALUE)
			throw exception("[server::server][Error] ao criar o evento accept connections.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 54, GetLastError()));
#elif defined(__linux__)

		EventShutdownServer = new Event(true, 0u);

		if (!EventShutdownServer->is_good()) {

			delete EventShutdownServer;

			EventShutdownServer = nullptr;

			throw exception("[server::server][Error] ao criar o evento ShutdownServer.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 54, errno));
		}

		EventShutdown = new Event(true, 0u);

		if (!EventShutdown->is_good()) {

			delete EventShutdown;

			EventShutdown = nullptr;

			throw exception("[server::server][Error] ao criar o evento shutdown.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 54, errno));
		}

		EventMoreAccept = new Event(true, 0u);

		if (!EventMoreAccept->is_good()) {

			delete EventMoreAccept;

			EventMoreAccept = nullptr;

			throw exception("[server::server][Error] ao criar o evento more accept.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 54, errno));
		}

		EventAcceptConnection = new Event(true, 0u);

		if (!EventAcceptConnection->is_good()) {

			delete EventAcceptConnection;

			EventAcceptConnection = nullptr;

			throw exception("[server::server][Error] ao criar o evento accept connections.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 54, errno));
		}
#endif

		// chama aqui, que quando o game_server estiver initializado(o construtor das classe base terminar e criar essa classe)
		// pode chamar essas funções puras

		// AcceptEx, 2 para aceitar conexão mais rápido
		for (auto i = 0u; i < _accept_thread_num; ++i)
			m_threads.push_back(new thread(TT_ACCEPTEX_IO, server::_worker_io_accept, (LPVOID)this, THREAD_PRIORITY_ABOVE_NORMAL));

		// Accept(s)
		m_threads.push_back(new thread(TT_ACCEPTEX, server::_acceptEx, (LPVOID)this, THREAD_PRIORITY_ABOVE_NORMAL/*1 = Primeiro, 2 = segundo core acho*/));		// Estava NORMAL

#if defined(_WIN32)
		InterlockedExchange(&m_continue_accept, 1);
#elif defined(__linux__)
		__atomic_store_n(&m_continue_accept, 1, __ATOMIC_RELAXED);
#endif

		// DB_NORMAL
		NormalManagerDB::create(_db_instance_num);

#if defined(_WIN32)
		InterlockedExchange(&m_continue_monitor, 1);
#elif defined(__linux__)
		__atomic_store_n(&m_continue_monitor, 1, __ATOMIC_RELAXED);
#endif

		// Monitor Thread
		m_thread_monitor = new thread(TT_MONITOR, server::_monitor, (LPVOID)this/*, THREAD_PRIORITY_BELOW_NORMAL*/);

#if defined(_WIN32)
		InterlockedExchange(&m_continue_register_server, 1);
#elif defined(__linux__)
		__atomic_store_n(&m_continue_register_server, 1, __ATOMIC_RELAXED);
#endif

		// Register Server
		m_threads.push_back(new thread(TT_REGISTER_SERVER, server::_registerServer, (LPVOID)this, THREAD_PRIORITY_BELOW_NORMAL));

#if defined(_WIN32)
		InterlockedExchange(&m_atomic_disconnect_session, 1);
#elif defined(__linux__)
		__atomic_store_n(&m_atomic_disconnect_session, 1, __ATOMIC_RELAXED);
#endif

		// Disconnect Session
		m_threads.push_back(new thread(TT_DISCONNECT_SESSION, server::_disconnect_session, (LPVOID)this));

		m_state = GOOD;

	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		m_state = FAILURE;
	}
};

server::~server() {

	if (m_thread_monitor != nullptr)
		delete m_thread_monitor;

	m_thread_monitor = nullptr;

	if (!m_server_list.empty()) {
		m_server_list.clear();
		m_server_list.shrink_to_fit();
	}

	if (!v_ip_ban_list.empty()) {
		v_ip_ban_list.clear();
		v_ip_ban_list.shrink_to_fit();
	}

	if (!v_mac_ban_list.empty()) {
		v_mac_ban_list.clear();
		v_mac_ban_list.shrink_to_fit();
	}

	// Destroy o Unit Connect que conecta com o Auth Server
	destroy_unit();

	// Fecha as Handle de Evento que usa na classe Server
	closeHandles();

	if (m_shutdown != nullptr && !m_timer_mgr.isEmpty()) 
		m_timer_mgr.deleteTimer(m_shutdown);

	m_shutdown = nullptr;

	NormalManagerDB::destroy();
};

#if defined(_WIN32)
DWORD server::_accept(LPVOID lpParameter) 
#elif defined(__linux__)
void* server::_accept(LPVOID lpParameter) 
#endif
{
	BEGIN_THREAD_SETUP(server);

	result = pTP->accept();

	END_THREAD_SETUP("Accept()");
};

#if defined(_WIN32)
DWORD server::_acceptEx(LPVOID lpParameter) 
#elif defined(__linux__)
void* server::_acceptEx(LPVOID lpParameter) 
#endif
{
	BEGIN_THREAD_SETUP(server);

	result = pTP->acceptEx();

	END_THREAD_SETUP("Accept()");
};

#if defined(_WIN32)
DWORD server::_monitor(LPVOID lpParameter) 
#elif defined(__linux__)
void* server::_monitor(LPVOID lpParameter) 
#endif
{
	BEGIN_THREAD_SETUP(server);

	result = pTP->monitor();

	END_THREAD_SETUP("monitor()");
};

#if defined(_WIN32)
DWORD server::_registerServer(LPVOID lpParameter) 
#elif defined(__linux__)
void* server::_registerServer(LPVOID lpParameter) 
#endif
{
	BEGIN_THREAD_SETUP(server);

	result = pTP->registerServer();

	END_THREAD_SETUP("registerServer()");
};

#if defined(_WIN32)
DWORD server::_disconnect_session(LPVOID lpParameter) 
#elif defined(__linux__)
void* server::_disconnect_session(LPVOID lpParameter) 
#endif
{
	BEGIN_THREAD_SETUP(server);

	result = pTP->disconnect_session();

	END_THREAD_SETUP("disconnect_session()");
};

#if defined(_WIN32)
DWORD server::monitor() 
#elif defined(__linux__)
void* server::monitor() 
#endif
{
	
	try {

		_smp::message_pool::getInstance().push(new message("[server::monitor][Log] monitor iniciado com sucesso!"));
		
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
						_smp::message_pool::getInstance().push(new message("[server::monitor::UpdateLogFiles][Log] Atualizou os arquivos de Log por que trocou de dia.", CL_FILE_LOG_AND_CONSOLE));

				}catch (exception& e) {

					_smp::message_pool::getInstance().push(new message("[server::monitor::UpdateLogFiles][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}

				if (m_thread_console != nullptr && !m_thread_console->isLive())
					m_thread_console->init_thread();
		
				for (size_t i = 0; i < m_threads.size(); i++)
					if (m_threads[i] != nullptr && !m_threads[i]->isLive())
						m_threads[i]->init_thread();
				
				try {

					m_session_manager.checkSessionLive();

				}catch (exception& e) {
					_smp::message_pool::getInstance().push(new message("[server::Monitor][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}

				NormalManagerDB::checkIsDeadAndRevive();
		
				cmdUpdateServerList();	// Pega a Lista de servidores online

				cmdUpdateListBlock_IP_MAC();	// Pega a List de IP e MAC Address que estão bloqueadas

				onHeartBeat();
		
#if defined(_WIN32)
				Sleep(1000); // 1 second para próxima verificação
#elif defined(__linux__)
				usleep(1000000); // 1 second para próxima verificação
#endif

			}catch (exception& e) {

				_smp::message_pool::getInstance().push(new message("[server::monitor][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

				if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::THREAD)
					throw;
			}
		}

	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[server::monitor][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (std::exception& e) {
		_smp::message_pool::getInstance().push(new message(std::string("[server::monitor][ErrorSystem] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "[server::monitor][ErrorSystem] monitor() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}
		
	_smp::message_pool::getInstance().push(new message("Saindo de monitor()..."));
		
#if defined(_WIN32)
	return 0u;
#elif defined(__linux__)
	return (void*)0u;
#endif
};

void server::waitAllThreadFinish(DWORD dwMilleseconds) {
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
			// Libera ele se não estiver liberado ainda
			setAcceptConnection();
#if defined(_WIN32)
			InterlockedDecrement(&m_continue_accept);
			SetEvent(EventShutdown);
#elif defined(__linux__)
			__atomic_sub_fetch(&m_continue_accept, 1, __ATOMIC_RELAXED);
			if (EventShutdown != nullptr)
				EventShutdown->set();
#endif
			break;
		case TT_ACCEPTEX:
			// Libera ele se não estiver liberado ainda
			setAcceptConnection();
#if defined(_WIN32)
			InterlockedDecrement(&m_continue_accept);
			SetEvent(EventShutdown);
#elif defined(__linux__)
			__atomic_sub_fetch(&m_continue_accept, 1, __ATOMIC_RELAXED);
			if (EventShutdown != nullptr)
				EventShutdown->set();
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
		case TT_REGISTER_SERVER:
#if defined(_WIN32)
			InterlockedDecrement(&m_continue_register_server);
#elif defined(__linux__)
			__atomic_sub_fetch(&m_continue_register_server, 1, __ATOMIC_RELAXED);
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
DWORD server::accept() 
#elif defined(__linux__)
void* server::accept() 
#endif
{

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
	
	DWORD ret = 0;

	socketserver *sv = nullptr;
	socket *sock = nullptr;
	
	try {
	
		_smp::message_pool::getInstance().push(new message("accept iniciado com sucesso!"));
	
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
	
		_smp::message_pool::getInstance().push(new message("[server::accept][Log] Esperando pelo Evento Accept Connection to listen on socket"));

#if defined(_WIN32)
		if (WaitForSingleObject(EventAcceptConnection, INFINITE) != WAIT_OBJECT_0) {
#elif defined(__linux__)
		if (EventAcceptConnection == nullptr)
			return (void*)(-1);

		if (EventAcceptConnection->wait(INFINITE) != WAIT_OBJECT_0) {
#endif

			_smp::message_pool::getInstance().push(new message("[server::accept][Error] nao conseguiu esperar pelo evento de aceitar coneccoes. ErrCode: " + std::to_string(
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

		_smp::message_pool::getInstance().push(new message("[server::accept][Log] Escutando na porta: " + std::to_string(m_si.port), CL_FILE_LOG_AND_CONSOLE));
	
		mHost = sock->detatch();
	
#if defined(_WIN32)
		if ((_event = WSACreateEvent()) == WSA_INVALID_EVENT)
			throw exception("[server::accept][Error] nao conseguiu criar WSAEvent.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 10, WSAGetLastError()));
	
		if (WSAEventSelect(mHost, _event, FD_ACCEPT) == SOCKET_ERROR)
			throw exception("[server::accept][Error] nao conseguiu selecionar WSAEvent FD_ACCEPT.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 11, WSAGetLastError()));
	
		WSANETWORKEVENTS events;
	
		while (InterlockedCompareExchange(&m_continue_accept, 1, 1)) {
			try {
	
				ret = WSAWaitForMultipleEvents(1, &_event, FALSE, 100, FALSE);
					
				if (!InterlockedCompareExchange(&m_continue_accept, 1, 1))
					break;
	
				if (ret == WSA_WAIT_TIMEOUT)
					continue;
	
				if ((ret = WSAEnumNetworkEvents(mHost, _event, &events)) == SOCKET_ERROR)
					throw exception("[server::accept][Error] nao conseguiu pegar os wsa net work events.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 12, WSAGetLastError()));
	
				if (events.lNetworkEvents & FD_ACCEPT) {
	
					if (events.iErrorCode[FD_ACCEPT_BIT] == 0 && InterlockedCompareExchange(&m_continue_accept, 1, 1)) {
	
						if ((accpt = WSAAccept(mHost, (SOCKADDR*)&remote, &length, NULL, NULL)) != INVALID_SOCKET) {

							int err_accept_sock = -1;

							if (sock->ip_rules && (err_accept_sock = sock->connect_check(accpt, ntohl(remote.sin_addr.s_addr))) <= 0) {

								char ip[25];

								if (inet_ntop(AF_INET, &remote.sin_addr.s_addr, ip, sizeof(ip)) == nullptr)
									throw exception("[server::accept][Error] ao converter SOCKADDR_IN para string doted mode(IP).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 54, 0));

								_smp::message_pool::getInstance().push(new message("[server::accept][Log] Socket[IP=" + std::string(ip) + "] Negado. " + std::string(err_accept_sock == 0 ? "IP Negado." : "Limite de Conexoes por IP"), CL_FILE_LOG_AND_CONSOLE));

								::shutdown(accpt, SD_BOTH);
								::closesocket(accpt);

								// Desconnectar o socket
								continue;
							}
	
							std::srand((uint32_t)std::time(nullptr) * 5);
								
							// Desable or enable option of accepted socket
							init_option_accepted_socket(accpt);

							// Log
							_smp::message_pool::getInstance().push(new message("[server::accept_completed][Log] Socket Accept[ID=" + std::to_string(accpt)
									+ ", Local ADDR=" + std::to_string(ntohl(addr.sin_addr.s_addr)) + ", Remote ADDR=" + std::to_string(ntohl(remote.sin_addr.s_addr)) + "]", CL_FILE_LOG_AND_CONSOLE));
	
							_session = m_session_manager.addSession(accpt, remote, std::rand() % 16);
							
#ifdef _DEBUG
							_smp::message_pool::getInstance().push(new message("Aceitou uma nova conexao. com IP: " + std::string(_session->getIP()) + ", e com a chave: " + std::to_string((int)_session->m_key) + ", e OID: " + std::to_string(_session->m_oid), CL_FILE_LOG_AND_CONSOLE));
#else
							_smp::message_pool::getInstance().push(new message("Aceitou uma nova conexao. com IP: " + std::string(_session->getIP()) + ", e com a chave: " + std::to_string((int)_session->m_key) + ", e OID: " + std::to_string(_session->m_oid), CL_ONLY_FILE_LOG));
#endif
							_smp::message_pool::getInstance().push(new message("Sessões online: " + std::to_string(m_session_manager.numSessionConnected()), CL_FILE_LOG_AND_CONSOLE));
	
							m_iocp_io[_session->m_key].associaDeviceToPort((ULONG_PTR)_session, (HANDLE)accpt);
	
							onAcceptCompleted(_session);
	
							_session = nullptr;	// reseta aqui para usar ela de novo
						}

					}else
						throw exception("[server::accept][Error] WSA NetWork event unknown or shutdown.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 13, WSAGetLastError()));
				}

			}catch (exception e) {

				_smp::message_pool::getInstance().push(new message("[server::accept][SystemError] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	
				if ((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::SESSION_MANAGER || STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::PLAYER_MANAGER) 
						&& STDA_ERROR_DECODE(e.getCodeError()) != 3/*checou ao limit de sesssion*/)	// Diferente do erro chegou ao limite de usuarios online
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
	
		int32_t check_m = 1; // Compare
		while (__atomic_compare_exchange_n(&m_continue_accept, &check_m, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
			try {

				// Clear
				FD_SET(mHost.fd, &fd_accept);

				// set again, select clean it
				accept_timeout.tv_sec = 0;
				accept_timeout.tv_usec = 100000;

				ret = select(mHost.fd + 1, &fd_accept, nullptr, nullptr, &accept_timeout);
					
				if (!__atomic_compare_exchange_n(&m_continue_accept, &check_m, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
					break;
	
				if (ret == 0/*timeout*/)
					continue;
	
				if (ret == -1/*Error*/)
					throw exception("[server::accept][Error] nao conseguiu selecionar o socket accept com select.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 12, errno));

				if ((accpt.fd = ::accept(mHost.fd, (sockaddr*)&remote, &length)) != INVALID_SOCKET) {

					int err_accept_sock = -1;

					if (sock->ip_rules && (err_accept_sock = sock->connect_check(accpt, ntohl(remote.sin_addr.s_addr))) <= 0) {

						char ip[25];

						if (inet_ntop(AF_INET, &remote.sin_addr.s_addr, ip, sizeof(ip)) == nullptr)
							throw exception("[server::accept][Error] ao converter SOCKADDR_IN para string doted mode(IP).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 54, 0));

						_smp::message_pool::getInstance().push(new message("[server::accept][Log] Socket[IP=" + std::string(ip) + "] Negado. " + std::string(err_accept_sock == 0 ? "IP Negado." : "Limite de Conexoes por IP"), CL_FILE_LOG_AND_CONSOLE));

						::shutdown(accpt.fd, SD_BOTH);
						::closesocket(accpt.fd);

						// Desconnectar o socket
						continue;
					}

					std::srand((uint32_t)std::time(nullptr) * 5);
						
					// Desable or enable option of accepted socket
					init_option_accepted_socket(accpt);

					// Log
					_smp::message_pool::getInstance().push(new message("[server::accept_completed][Log] Socket Accept[ID=" + std::to_string(accpt.fd)
							+ ", Local ADDR=" + std::to_string(ntohl(addr.sin_addr.s_addr)) + ", Remote ADDR=" + std::to_string(ntohl(remote.sin_addr.s_addr)) + "]", CL_FILE_LOG_AND_CONSOLE));

					// time to accept
					clock_gettime(SESSION_CONNECT_TIME_CLOCK, &accpt.connect_time);

					_session = m_session_manager.addSession(accpt, remote, std::rand() % 16);
					
#ifdef _DEBUG
					_smp::message_pool::getInstance().push(new message("Aceitou uma nova conexao. com IP: " + std::string(_session->getIP()) + ", e com a chave: " + std::to_string((int)_session->m_key) + ", e OID: " + std::to_string(_session->m_oid), CL_FILE_LOG_AND_CONSOLE));
#else
					_smp::message_pool::getInstance().push(new message("Aceitou uma nova conexao. com IP: " + std::string(_session->getIP()) + ", e com a chave: " + std::to_string((int)_session->m_key) + ", e OID: " + std::to_string(_session->m_oid), CL_ONLY_FILE_LOG));
#endif
					_smp::message_pool::getInstance().push(new message("Sessões online: " + std::to_string(m_session_manager.numSessionConnected()), CL_FILE_LOG_AND_CONSOLE));

					epoll_event ep_ev;
					ep_ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLHUP | EPOLLRDHUP;
					//ep_ev.data.fd = accpt.fd;
					ep_ev.data.ptr = _session;

					m_iocp_io[_session->m_key].associaDeviceToPort((uint32_t)accpt.fd, ep_ev);

					onAcceptCompleted(_session);

					_session = nullptr;	// reseta aqui para usar ela de novo
				}

			}catch (exception e) {

				_smp::message_pool::getInstance().push(new message("[server::accept][SystemError] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	
				if ((STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::SESSION_MANAGER || STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::PLAYER_MANAGER) 
						&& STDA_ERROR_DECODE(e.getCodeError()) != 3/*checou ao limit de sesssion*/)	// Diferente do erro chegou ao limite de usuarios online
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
		_smp::message_pool::getInstance().push(new message("[server::accept][SystemError] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (std::exception& e) {
		_smp::message_pool::getInstance().push(new message("[server::accept][SystemError] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "[server::accept][SystemError] accept() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
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
		mHost.connect_time = { 0u };
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
DWORD server::acceptEx() 
#elif defined(__linux__)
void* server::acceptEx() 
#endif
{

#if defined(_WIN32)
	SOCKET mHost = INVALID_SOCKET;
	HANDLE _event = INVALID_HANDLE_VALUE;
#elif defined(__linux__)
	SOCKET mHost { INVALID_SOCKET, {0u} };
#endif

	player *_player = nullptr;

	DWORD ret = 0;

	socketserver *sv = nullptr;
	socket *sock = nullptr;

	try {

		_smp::message_pool::getInstance().push(new message("acceptEx iniciado com sucesso!"));

		sv = new socketserver();

		SOCKADDR_IN addr, remote;
		int length = sizeof(remote);

		addr.sin_family = AF_INET;	// depois coloca o host aqui // m_si.m_ip
		addr.sin_port = htons(m_si.port);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		sock = new socket(sv->getsocklistener(), 1);

		// pega a referencia do sock que controla as conexão que pode logar
		m_accept_sock = sock;

		sock->bind(addr);

		_smp::message_pool::getInstance().push(new message("[server::acceptEx][Log] Esperando pelo Evento Accept Connection to listen on socket"));

#if defined(_WIN32)
		if (WaitForSingleObject(EventAcceptConnection, INFINITE) != WAIT_OBJECT_0) {
#elif defined(__linux__)
		if (EventAcceptConnection == nullptr)
			return (void*)(-1);

		if (EventAcceptConnection->wait(INFINITE) != WAIT_OBJECT_0) {
#endif

			_smp::message_pool::getInstance().push(new message("[server::acceptEx][Error] nao conseguiu esperar pelo evento de aceitar coneccoes. ErrCode: " + std::to_string(
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

		_smp::message_pool::getInstance().push(new message("[server::acceptEx][Log] Escutando na porta: " + std::to_string(m_si.port), CL_FILE_LOG_AND_CONSOLE));

		mHost = sock->detatch();

#if defined(_WIN32)
		if ((_event = WSACreateEvent()) == WSA_INVALID_EVENT)
			throw exception("[server::acceptEx][Error] nao conseguiu criar WSAEvent.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 10, WSAGetLastError()));

		if (WSAEventSelect(mHost, _event, FD_ACCEPT) == SOCKET_ERROR)
			throw exception("[server::acceptEx][Error] nao conseguiu selecionar WSAEvent FD_ACCEPT.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 11, WSAGetLastError()));

		m_iocp_io_accept.associaDeviceToPort((ULONG_PTR)mHost, (HANDLE)mHost);

		if (m_lpfnAcceptEx == nullptr)
			init_LpFnAccetEx(mHost);

		DWORD dwBytesReceived = 0u;
		DWORD lastError = 0u;

		myOver *over = nullptr;

		HANDLE events[3];

		uint32_t max_user = (uint32_t)m_si.max_user;

		while (InterlockedCompareExchange(&m_continue_accept, 1, 1)) {
			
			try {

				do {

					while (m_acceptsPendents < max_user) {

						over = new myOver;

						memset(over, 0, sizeof(OVERLAPPED));

						over->tipo = STDA_OT_ACCEPT_COMPLETED;
						over->buffer = new Buffer(0);
						over->buffer->setOperation((DWORD)((SOCKET)sv->getsocklistener()));

						InterlockedIncrement(&m_acceptsPendents);

						if (!m_lpfnAcceptEx(mHost, (SOCKET)over->buffer->getOperation(), (void*)over->buffer->getBuffer(), 0/*sem dados depois de aceitar conexão*/, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytesReceived, over)) {
							
							lastError = WSAGetLastError();

							if (ERROR_IO_PENDING != lastError) {
								
								_smp::message_pool::getInstance().push(new message("[server::acceptEx] Delete Socket alocado. ErrorCode: " + std::to_string(lastError), CL_FILE_LOG_AND_CONSOLE));

								::closesocket((SOCKET)over->buffer->getOperation());

								if (over != nullptr) {
									if (over->buffer != nullptr)
										delete over->buffer;

									delete over;
								}

								InterlockedDecrement(&m_acceptsPendents);
							}

						}else {

							// Post Accepted Socket
							m_iocp_io_accept.postStatus((ULONG_PTR)mHost, dwBytesReceived, over);
						}
					}

					if (ret == WAIT_OBJECT_0 + 2 && m_acceptsPendents < max_user)
						max_user = m_si.max_user;

					ResetEvent(_event);
					ResetEvent(EventMoreAccept);

					// Checkpedents conexões aqui, mas eu não estou usando isso, ele aceita imediatamente, não espera por dados

					events[0] = EventShutdown;
					events[1] = EventMoreAccept;
					events[2] = _event;

					ret = WaitForMultipleObjects(3, events, FALSE, INFINITE);

					if (ret != WAIT_OBJECT_0 && ret != WAIT_OBJECT_0 + 1 && ret != WAIT_OBJECT_0 + 2)
						throw exception("[server::acceptEx] error ao esperar eventos.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 55, GetLastError()));

					if (ret == WAIT_OBJECT_0 + 2)
						max_user += 10;

				} while (ret == WAIT_OBJECT_0 + 1 || ret == WAIT_OBJECT_0 + 2);

				// @![Waring] Essa mensagem foi só para teste para ver se sai do while quando aandava o shutdown event
				//_smp::message_pool::getInstance().push(new message("[server::acceptEx] sai do do while, volto para o while principal.", CL_FILE_LOG_AND_CONSOLE));
			}catch (exception& e) {
				
				if (STDA_ERROR_DECODE(e.getCodeError()) != 3/*limit de contas*/)
					_smp::message_pool::getInstance().push(new message("[server::AcceptEx][SystemError] " + std::string(e.getFullMessageError()), CL_FILE_LOG_AND_CONSOLE));
			
			}catch (std::exception& e) {

				_smp::message_pool::getInstance().push(new message("[server::AcceptEx][SystemError] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
			}
		}
#elif defined(__linux__)

		epoll_event ep_ev;

		ep_ev.data.fd = mHost.fd;
		ep_ev.events = EPOLLIN || EPOLLET;

		// NON BLOCKING
		int flag = O_NONBLOCK;
		if (fcntl(mHost.fd, F_SETFL, flag) != 0)
			throw exception("[server::acceptEx][Error] nao conseguiu habilitar o NONBLOCK(fcntl).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 53, errno));

		m_iocp_io_accept.associaDeviceToPort((uint32_t)mHost.fd, ep_ev);

		DWORD dwBytesReceived = 0u;
		DWORD lastError = 0u;

		myOver *over = nullptr;

		std::vector< Event* > events { EventShutdown, EventMoreAccept };

		uint32_t max_user = (uint32_t)m_si.max_user;

		int32_t check_m = 1; // Compare
		while (__atomic_compare_exchange_n(&m_continue_accept, &check_m, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
			
			try {

				do {

					if (ret == WAIT_OBJECT_0 + 2 && m_acceptsPendents < max_user)
						max_user = m_si.max_user;

					if (EventMoreAccept != nullptr)
						EventMoreAccept->reset();

					// Checkpedents conexões aqui, mas eu não estou usando isso, ele aceita imediatamente, não espera por dados
					ret = Event::waitMultipleEvent(2, events, FALSE, INFINITE);

					if (ret != WAIT_OBJECT_0 && ret != WAIT_OBJECT_0 + 1 && ret != WAIT_OBJECT_0 + 2)
						throw exception("[server::acceptEx] error ao esperar eventos.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 55, errno));

					if (ret == WAIT_OBJECT_0 + 2)
						max_user += 10;

				} while (ret == WAIT_OBJECT_0 + 1 || ret == WAIT_OBJECT_0 + 2);

				// @![Waring] Essa mensagem foi só para teste para ver se sai do while quando aandava o shutdown event
				//_smp::message_pool::getInstance().push(new message("[server::acceptEx] sai do do while, volto para o while principal.", CL_FILE_LOG_AND_CONSOLE));
			}catch (exception& e) {
				
				if (STDA_ERROR_DECODE(e.getCodeError()) != 3/*limit de contas*/)
					_smp::message_pool::getInstance().push(new message("[server::AcceptEx][SystemError] " + std::string(e.getFullMessageError()), CL_FILE_LOG_AND_CONSOLE));
			
			}catch (std::exception& e) {

				_smp::message_pool::getInstance().push(new message("[server::AcceptEx][SystemError] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
			}
		}
#endif

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[server::AcceptEx][SystemError] " + std::string(e.getFullMessageError()), CL_FILE_LOG_AND_CONSOLE));
	
	}catch (std::exception& e) {

		_smp::message_pool::getInstance().push(new message("[server::AcceptEx][SystemError]" + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
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
		mHost.connect_time = {0u};
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

	_smp::message_pool::getInstance().push(new message("Saindo de acceptEx()..."));

#if defined(_WIN32)
	return 0u;
#elif defined(__linux__)
	return (void*)0u;
#endif
};

#if defined(_WIN32)
DWORD server::disconnect_session() 
#elif defined(__linux__)
void* server::disconnect_session() 
#endif
{
	
	try {

		_smp::message_pool::getInstance().push(new message("[server::disconnect_session][Log] disconnect_session iniciado com sucesso!"));
		
#if defined(_WIN32)
		while (InterlockedCompareExchange(&m_atomic_disconnect_session, 1, 1)) {
#elif defined(__linux__)
		int32_t check_m = 1; // Compare
		while (__atomic_compare_exchange_n(&m_atomic_disconnect_session, &check_m, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
#endif
			
			try {
				
				try {

					
					if (session_manager::isInit()) {

						auto s = m_session_manager.getSessionToDelete(1000/*1 second para a liberar o while se não tiver sessions para disconectar*/);

						if (s != nullptr)
							DisconnectSession(s);

					}else
#if defined(_WIN32)
						Sleep(300/*espera 300 miliseconds até o session_manager ser inicializado*/);
#elif defined(__linux__)
						usleep(300000/*espera 300 miliseconds até o session_manager ser inicializado*/);
#endif

				}catch (exception& e) {
					_smp::message_pool::getInstance().push(new message("[server::disconnect_session][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}

			}catch (exception& e) {

				_smp::message_pool::getInstance().push(new message("[server::disconnect_session][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

				if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::THREAD)
					throw;
			}
		}

	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[server::disconnect_session][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (std::exception& e) {
		_smp::message_pool::getInstance().push(new message(std::string("[server::disconnect_session][ErrorSystem] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "[server::disconnect_session][ErrorSystem] disconnect_session() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}
		
	_smp::message_pool::getInstance().push(new message("Saindo de disconnect_session()..."));
		
#if defined(_WIN32)
	return 0u;
#elif defined(__linux__)
	return (void*)0u;
#endif
};

#if defined(_WIN32)
DWORD server::registerServer() 
#elif defined(__linux__)
void* server::registerServer() 
#endif
{

	try {

		_smp::message_pool::getInstance().push(new message("[server::registerServer][Log] Register Server iniciado com sucesso!"));
		
#if defined(_WIN32)
		while (InterlockedCompareExchange(&m_continue_register_server, 1, 1)) {
#elif defined(__linux__)
		int32_t check_m = 1; // Compare
		while (__atomic_compare_exchange_n(&m_continue_register_server, &check_m, 1, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
#endif
			
			// Intervalo de 1 segundo
#if defined(_WIN32)
			Sleep(1000u);
#elif defined(__linux__)
			usleep(1000000u);
#endif

			try {

				// Current user online on server
				m_si.curr_user = m_session_manager.getNumSessionOnline();

				// Register Server
				NormalManagerDB::add(0, new CmdRegisterServer(m_si), server::SQLDBResponse, this);
					
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
		}

	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[server::registerServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (std::exception& e) {
		_smp::message_pool::getInstance().push(new message(std::string("[server::registerServer][ErrorSystem] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "[server::registerServer][ErrorSystem] registerServer() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}
		
	_smp::message_pool::getInstance().push(new message("Saindo de registerServer()..."));

#if defined(_WIN32)
	return 0u;
#elif defined(__linux__)
	return (void*)0u;
#endif
};

void server::accept_completed(SOCKET *_listener, DWORD dwIOsize, myOver *lpBuffer, DWORD _operation) {
	
#if defined(_WIN32)
	try {

		if (dwIOsize == 0/*Não uso o que recebe dados no accept, e uso esse valor para flag de erro para deletar o player que caiu*/) {

			if (*_listener == INVALID_SOCKET)
				throw exception("[server::accept_completed] error socket invalido.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 50, 0));

			SOCKET accept = (SOCKET)lpBuffer->buffer->getOperation();

			InterlockedDecrement(&m_acceptsPendents);

			// Aciona o evento de do accept para colocar mais 1 socket accept no backlog
			SetEvent(EventMoreAccept);

			if (m_session_manager.isFull()) {

				_smp::message_pool::getInstance().push(new message("[server::accept_completed] Negando player, por que chegou no limit do server.", CL_FILE_LOG_AND_CONSOLE));

				::shutdown(accept, SD_BOTH);
				::closesocket(accept);

				SetEvent(EventMoreAccept);

				return;
			}

			if (m_lpfnGetAcceptExSockaddrs == nullptr)
				init_LpFnGetAcceptExSockaddrs(*_listener);

			if (setsockopt(accept, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)_listener, sizeof(*_listener)) == SOCKET_ERROR)
				throw exception("[server::accept_completed] error ao atualizar socket accept context.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 52, WSAGetLastError()));

			// Desable or enable option of accepted socket
			init_option_accepted_socket(accept);

			INT sizeLocal = 0;
			INT sizeRemote = 0;

			SOCKADDR *pLocal = nullptr;
			SOCKADDR *pRemote = nullptr;

			m_lpfnGetAcceptExSockaddrs((void*)lpBuffer->buffer->getBuffer(), 0/*nao recebe dados no acceptEx, por minha opção*/, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &pLocal, &sizeLocal, &pRemote, &sizeRemote);

			if (pLocal == nullptr) {

				_smp::message_pool::getInstance().push(new message("[server::accept_completed][Log] Local IP is nullptr, pegando o ip local com a função getsockname.", CL_FILE_LOG_AND_CONSOLE));

				pLocal = (SOCKADDR*)lpBuffer->buffer->getBuffer();

				if (getsockname(accept, pLocal, &sizeLocal) == SOCKET_ERROR)
					throw exception("[server::accept_completed][Error] nao conseguiu pegar o sockname, do socket=" + std::to_string((uint32_t)accept), 
							STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 55, WSAGetLastError()));
			}

			if (pRemote == nullptr) {

				_smp::message_pool::getInstance().push(new message("[server::accept_completed][Log] Remote IP is nullptr, pegando o ip remoto com a função getpeername.", CL_FILE_LOG_AND_CONSOLE));

				pRemote = (SOCKADDR*)(lpBuffer->buffer->getBuffer() + sizeof(SOCKADDR) + 16);

				if (getpeername(accept, pRemote, &sizeRemote) == SOCKET_ERROR)
					throw exception("[server::accept_completed][Error] nao conseguiu pegar o peername, do socket=" + std::to_string((uint32_t)accept),
							STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 56, WSAGetLastError()));
			}

			// Log
			_smp::message_pool::getInstance().push(new message("[server::accept_completed][Log] Socket Accept[ID=" + std::to_string(accept) 
					+ ", Local ADDR=" + std::to_string(ntohl(((SOCKADDR_IN*)pLocal)->sin_addr.s_addr)) + ", Remote ADDR=" + std::to_string(ntohl(((SOCKADDR_IN*)pRemote)->sin_addr.s_addr)) + "]", CL_FILE_LOG_AND_CONSOLE));

			if (m_accept_sock != nullptr) {
				
				int err_accept_sock = -1;

				if (m_accept_sock->ip_rules && (err_accept_sock = m_accept_sock->connect_check(accept, ntohl(((SOCKADDR_IN*)pRemote)->sin_addr.s_addr))) <= 0) {

					char ip[25];

					if (inet_ntop(AF_INET, &((SOCKADDR_IN*)pRemote)->sin_addr.s_addr, ip, sizeof(ip)) == nullptr)
						throw exception("[server::accept_completed][Error] ao converter SOCKADDR_IN para string doted mode(IP).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 54, 0));

					_smp::message_pool::getInstance().push(new message("[server::accept_completed][Log] Socket[IP=" + std::string(ip) + "] Negado. " + std::string(err_accept_sock == 0 ? "IP Negado." : "Limite de Conexoes por IP"), CL_FILE_LOG_AND_CONSOLE));

					::shutdown(accept, SD_BOTH);
					::closesocket(accept);

					SetEvent(EventMoreAccept);

					return;
				}

			}else
				_smp::message_pool::getInstance().push(new message("[server::accept_completed][WARNING] accpet socket controlador nao foi iniciado, nao consegue bloquear conexoes.", CL_FILE_LOG_AND_CONSOLE));

			std::srand((uint32_t)std::time(nullptr) * 7 * (uint32_t)std::time(nullptr));

			session *_session = m_session_manager.addSession(accept, *(SOCKADDR_IN*)pRemote, std::rand() % 16);

			_smp::message_pool::getInstance().push(new message("[server::acceptEx][Log] Aceitou um player, com IP: " + std::string(_session->getIP()) + "\t Key: " + std::to_string((int)_session->m_key) + "\t OID: " + std::to_string(_session->m_oid), CL_FILE_LOG_AND_CONSOLE));
			_smp::message_pool::getInstance().push(new message("[server::acceptEx][Log] Sessões Online: " + std::to_string(m_session_manager.numSessionConnected()), CL_FILE_LOG_AND_CONSOLE));

			m_iocp_io[_session->m_key].associaDeviceToPort((ULONG_PTR)_session, (HANDLE)_session->m_sock);

			try {
				onAcceptCompleted(_session);
			}catch (exception& e) {
					
				//if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::SERVER) {
					// Error Server onAcceptCompleted
					// 60 Session is null
					// 61 Session is Invalid
					// 62 Session not connected
					uint32_t errCod = STDA_ERROR_DECODE(e.getCodeError());

					if (errCod == 61 || errCod == 62)
						m_session_manager.deleteSession(_session);
				//}

				_smp::message_pool::getInstance().push(new message("[server::accept_completed][ErrorSystem]" + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			}

		}else if (dwIOsize != 0) { // Error do player deleta ele
			
			if (lpBuffer != nullptr && lpBuffer->buffer != nullptr) {
				::shutdown((SOCKET)lpBuffer->buffer->getOperation(), SD_BOTH);
				::closesocket((SOCKET)lpBuffer->buffer->getOperation());
			}

			SetEvent(EventMoreAccept);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[server::accept_completed][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		if (lpBuffer != nullptr && lpBuffer->buffer != nullptr) {
			::shutdown((SOCKET)lpBuffer->buffer->getOperation(), SD_BOTH);
			::closesocket((SOCKET)lpBuffer->buffer->getOperation());
		}

		SetEvent(EventMoreAccept);
	}
#elif defined(__linux__)
	try {

		if (dwIOsize == 0/*Não uso o que recebe dados no accept, e uso esse valor para flag de erro para deletar o player que caiu*/) {

			if (_listener == nullptr || _listener->fd == INVALID_SOCKET)
				throw exception("[server::accept_completed] error socket invalido.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 50, 0));

			SOCKET accept = { 0u };
			SOCKADDR_IN remote;
			SOCKADDR_IN local;
			socklen_t length = sizeof(sockaddr);

			if ((accept.fd = ::accept(_listener->fd, (sockaddr*)&remote, &length)) != INVALID_SOCKET) {

				// local addr
				if (getsockname(accept.fd,(sockaddr*)&local, &length) == SOCKET_ERROR)
					throw exception("[server::accept_completed][Error] nao conseguiu pegar o sockname, do socket=" + std::to_string((uint32_t)accept.fd), 
							STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 55, errno));


				if (m_accept_sock != nullptr) {
					int err_accept_sock = -1;

					if (m_accept_sock->ip_rules && (err_accept_sock = m_accept_sock->connect_check(accept, ntohl(remote.sin_addr.s_addr))) <= 0) {

						char ip[25];

						if (inet_ntop(AF_INET, &((SOCKADDR_IN*)&remote)->sin_addr.s_addr, ip, sizeof(ip)) == nullptr)
							throw exception("[server::accept_completed][Error] ao converter SOCKADDR_IN para string doted mode(IP).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 54, 0));

						_smp::message_pool::getInstance().push(new message("[server::accept_completed][Log] Socket[IP=" + std::string(ip) + "] Negado. " + std::string(err_accept_sock == 0 ? "IP Negado." : "Limite de Conexoes por IP"), CL_FILE_LOG_AND_CONSOLE));

						::shutdown(accept.fd, SD_BOTH);
						::closesocket(accept.fd);

						if (EventMoreAccept != nullptr)
							EventMoreAccept->set();

						return;
					}
				}else
					_smp::message_pool::getInstance().push(new message("[server::accept_completed][WARNING] accpet socket controlador nao foi iniciado, nao consegue bloquear conexoes.", CL_FILE_LOG_AND_CONSOLE));

				// volatile accept pendents
				__atomic_sub_fetch(&m_acceptsPendents, 1, __ATOMIC_RELAXED);

				// Aciona o evento de do accept para colocar mais 1 socket accept no backlog
				if (EventMoreAccept != nullptr)
					EventMoreAccept->set();

				if (m_session_manager.isFull()) {

					_smp::message_pool::getInstance().push(new message("[server::accept_completed] Negando player, por que chegou no limit do server.", CL_FILE_LOG_AND_CONSOLE));

					::shutdown(accept.fd, SD_BOTH);
					::closesocket(accept.fd);

					if (EventMoreAccept != nullptr)
						EventMoreAccept->set();

					return;
				}

				std::srand((uint32_t)std::time(nullptr) * 7 * (uint32_t)std::time(nullptr));
					
				// Desable or enable option of accepted socket
				try {
					init_option_accepted_socket(accept);
				}catch (exception& e) {

					_smp::message_pool::getInstance().push(new message("[server::accept_completed][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

					::shutdown(accept.fd, SD_BOTH);
					::closesocket(accept.fd);

					if (EventMoreAccept != nullptr)
						EventMoreAccept->set();
					
					return;
				}

				// Log
				_smp::message_pool::getInstance().push(new message("[server::accept_completed][Log] Socket Accept[ID=" + std::to_string(accept.fd)
						+ ", Local ADDR=" + std::to_string(ntohl(local.sin_addr.s_addr)) + ", Remote ADDR=" + std::to_string(ntohl(remote.sin_addr.s_addr)) + "]", CL_FILE_LOG_AND_CONSOLE));

				// time to accept
				clock_gettime(SESSION_CONNECT_TIME_CLOCK, &accept.connect_time);

				session *_session = m_session_manager.addSession(accept, *(SOCKADDR_IN*)&remote, std::rand() % 16);
				
				_smp::message_pool::getInstance().push(new message("[server::acceptEx][Log] Aceitou um player, com IP: " + std::string(_session->getIP()) + "\t Key: " + std::to_string((int)_session->m_key) + "\t OID: " + std::to_string(_session->m_oid), CL_FILE_LOG_AND_CONSOLE));
				_smp::message_pool::getInstance().push(new message("[server::acceptEx][Log] Sessões Online: " + std::to_string(m_session_manager.numSessionConnected()), CL_FILE_LOG_AND_CONSOLE));

				epoll_event ep_ev;
				ep_ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLHUP | EPOLLRDHUP;
				//ep_ev.data.fd = accept.fd;
				ep_ev.data.ptr = _session;

				m_iocp_io[_session->m_key].associaDeviceToPort((uint32_t)accept.fd, ep_ev);

				try {
					onAcceptCompleted(_session);
				}catch (exception& e) {
						
					//if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::SERVER) {
						// Error Server onAcceptCompleted
						// 60 Session is null
						// 61 Session is Invalid
						// 62 Session not connected
						uint32_t errCod = STDA_ERROR_DECODE(e.getCodeError());

						if (errCod == 61 || errCod == 62)
							m_session_manager.deleteSession(_session);
					//}

					_smp::message_pool::getInstance().push(new message("[server::accept_completed][ErrorSystem]" + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}
				
			}

		}else if (dwIOsize != 0) {} // Error do player deleta ele

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[server::accept_completed][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
#endif
};

inline void server::dispach_packet_same_thread(session& _session, packet *_packet) {
	CHECK_SESSION_BEGIN("dispach_packet_same_thread");

	//ParamWorker pw = { *this, m_iocp_io, m_job_pool, m_session_pool, m_pangya_db, _packet };
	func_arr::func_arr_ex* func = nullptr;

	try {
		
		func = packet_func_base::funcs.getPacketCall(_packet->getTipo());
	
	}catch (exception& e) {

		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::FUNC_ARR/*Packet_func Erro, Warning e etc*/) {
			
			_smp::message_pool::getInstance().push(new message("[server::dispach_packet_same_thread][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[server::dispach_packet_same_thread][Log] Pacote desconhecido enviado pelo Player[UID=" + std::to_string(_session.getUID()) + ", OID=" + std::to_string(_session.m_oid) + "] size packet: " 
					+ std::to_string(_packet->getSize()) + "\n" + hex_util::BufferToHexString(_packet->getBuffer(), _packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
#else
			_smp::message_pool::getInstance().push(new message("[server::dispach_packet_same_thread][Log] Pacote desconhecido enviado pelo Player[UID=" + std::to_string(_session.getUID()) + ", OID=" + std::to_string(_session.m_oid) + "] size packet: "
					+ std::to_string(_packet->getSize()) + "\n" + hex_util::BufferToHexString(_packet->getBuffer(), _packet->getSize()), CL_ONLY_FILE_LOG));
#endif

			// Já tem quase todos pacotes sendo tratados, se o cliente mandar um pacote desconhecido derruba ele
			DisconnectSession(&_session);

			// Trata o erro aqui
			// e desloga a session que enviou pacote errado
			/// colocar de novo ------> m_pe->_session_pool.deleteSession(_packet->getSession());
			// se for muito grave relança ele para terminar a thread
		}else
			DisconnectSession(&_session);

		// Relança exception
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
				_smp::message_pool::getInstance().push(new message("[server::dispach_packet_same_thread][Error] Ao tratar o pacote. ID: " + std::to_string(_packet->getTipo()) 
						+ "(0x" + hex_util::ltoaToHex(_packet->getTipo()) + ").", CL_FILE_LOG_AND_CONSOLE));
		}

		if (_session.devolve())
			DisconnectSession(&_session);

	}catch (exception& e) {
//#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[server::dispach_packet_same_thread][Error][MY] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#endif

		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*não pode usa session*/))
			if (_session.devolve())
				DisconnectSession(&_session);

	}catch (std::exception& e) {
//#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message(std::string("[server::dispach_packet_same_thread][Error][STD] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));
//#else
	//	UNREFERENCED_PARAMETER(e);
//#endif

		if (_session.devolve())
			DisconnectSession(&_session);

	}catch (...) {
//#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[server::dispach_packet_same_thread][Error] Unknown Error)", CL_FILE_LOG_AND_CONSOLE));
//#endif

		if (_session.devolve())
			DisconnectSession(&_session);
	}
};

inline void server::dispach_packet_sv_same_thread(session& _session, packet *_packet) {
	CHECK_SESSION_BEGIN("dispach_packet_sv_same_thread");
	
	//ParamWorker pw = { *this, m_iocp_io, m_job_pool, m_session_pool, m_pangya_db, _packet };
	func_arr::func_arr_ex* func = nullptr;

	try {
		
		func = packet_func_base::funcs_sv.getPacketCall(_packet->getTipo());
	
	}catch (exception& e) {
		
		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::FUNC_ARR/*Packet_func Erro, Warning e etc*/) {
		
			_smp::message_pool::getInstance().push(new message("[server::dispach_packet_sv_same_thread][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[server::dispach_packet_sv_same_thread][Log] Pacote desconhecido enviado pelo server para o  Player[UID=" + std::to_string(_session.getUID()) 
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
			_smp::message_pool::getInstance().push(new message("[server::dispach_packet_sv_same_thread][Error] Ao tratar o pacote. ID: " + std::to_string(_packet->getTipo()) 
					+ "(0x" + hex_util::ltoaToHex(_packet->getTipo()) + ").", CL_FILE_LOG_AND_CONSOLE));
	
		if (_session.devolve())
			DisconnectSession(&_session);

	}catch (exception& e) {
//#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[server::dispach_packet_sv_same_thread][Error][MY] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#endif

		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*não pode usa session*/))
			if (_session.devolve())
				DisconnectSession(&_session);

	}catch (std::exception& e) {
//#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message(std::string("[server::dispach_packet_sv_same_thread][Error][STD] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));
//#else
	//	UNREFERENCED_PARAMETER(e);
//#endif

		if (_session.devolve())
			DisconnectSession(&_session);

	}catch (...) {
//#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[server::dispach_packet_sv_same_thread][Error] Unknown Error", CL_FILE_LOG_AND_CONSOLE));
//#endif

		if (_session.devolve())
			DisconnectSession(&_session);
	}
};

bool server::DisconnectSession(session *_session) {

	bool ret = true;

	onDisconnected(_session);

	try {
		
		ret = m_session_manager.deleteSession(_session);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[server::DisconnectSession][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		ret = false;
	}

#if defined(_WIN32)
	SetEvent(EventMoreAccept);
#elif defined(__linux__)
	if (EventMoreAccept != nullptr)
		EventMoreAccept->set();
#endif

	return ret;
};

uint32_t server::getBotTTL() {
	return m_Bot_TTL;
};

void server::cmdUpdateServerList() {
	NormalManagerDB::add(1, new CmdServerList(CmdServerList::GAME), server::SQLDBResponse, this);
	//m_server_list = pangya_base_db::getServerList();
};

void server::updateServerList(std::vector< ServerInfo >& _v_si) {
	m_server_list = _v_si;
};

void server::init_option_accepted_socket(SOCKET _accepted) {

	BOOL tcp_nodelay = 1;
	//int so_sndbuf = 0;
	//int so_timeout = 0;

	/// ---------- DESEMPENHO COM OS SOCKOPT -----------
	/// COM NO_TCPDELAY					AVG(MEDIA) 0.552
	/// COM SO_SNDBUF 0					AVG(MEDIA) 0.560
	/// COM SO_RCVBUF 0					AVG(MEDIA) 0.570
	/// COM NO_TCPDELAY e SO_SNDBUF 0	AVG(MEDIA) 0.569
	/// COM NO_TCPDELAY e SO_RCVBUF 0	AVG(MEDIA) 0.566
	/// SEM NENHUM SOCKOPT				AVG(MEDIA) 0.569
	// Não tem muita diferença vou deixar só o NO_TCPDELAY mesmo

	//if (setsockopt(accpt, SOL_SOCKET, SO_SNDBUF, (char*)&so_sndbuf, sizeof(int)) == SOCKET_ERROR)
	//	_smp::message_pool::getInstance().push(new message("nao conseguiu setar SO_SNDBUF para 0, threadpl_server::accept(). error Code: " + std::to_string(STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 2, WSAGetLastError())), CL_FILE_LOG_AND_CONSOLE));

	//if (setsockopt(accpt, SOL_SOCKET, SO_RCVTIMEO, (char*)&so_timeout, sizeof(int)) == SOCKET_ERROR)
	//	_smp::message_pool::getInstance().push(new message("nao conseguiu set o recive time out. server::accept(). error Code: " + std::to_string(STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 2, WSAGetLastError())), CL_FILE_LOG_AND_CONSOLE));

	//if (setsockopt(accpt, SOL_SOCKET, SO_SNDTIMEO, (char*)&so_timeout, sizeof(int)) == SOCKET_ERROR)
	//	_smp::message_pool::getInstance().push(new message("nao conseguiu set o send time out. server::accept(). error Code: " + std::to_string(STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 2, WSAGetLastError())), CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
	tcp_keepalive keep;
	DWORD retk = 0;

	keep.onoff = 1;
	keep.keepalivetime = 20000; // Estava 10000(10 segundos)
	keep.keepaliveinterval = 2000; // Estava 1000(1 segundo)

	if (WSAIoctl(_accepted, SIO_KEEPALIVE_VALS, &keep, sizeof(keep), nullptr, 0, &retk, nullptr, nullptr) == SOCKET_ERROR)
		throw exception("[server::init_option_accepted_socket][Error] nao conseguiu setar o socket option KEEPALIVE[ONOFF=" + std::to_string(keep.onoff)
				+ ", TIME=" + std::to_string(keep.keepalivetime) + ", INTERVAL=" + std::to_string(keep.keepaliveinterval)
				+ "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_SERVER, 53, WSAGetLastError()));

	// KEEPALIVE Log
	_smp::message_pool::getInstance().push(new message("[server::init_option_accepted_socket][Log] socket[ID=" + std::to_string(_accepted)
			+ "] KEEPALIVE[ONOFF=" + std::to_string(keep.onoff) + ", TIME=" + std::to_string(keep.keepalivetime)
			+ ", INTERVAL=" + std::to_string(keep.keepaliveinterval) + "] foi ativado para esse ", CL_FILE_LOG_AND_CONSOLE));

	if (setsockopt(_accepted, IPPROTO_TCP, TCP_NODELAY, (char*)&tcp_nodelay, sizeof(tcp_nodelay)) == SOCKET_ERROR)
		throw exception("[server::init_option_accepted_socket][Error] nao conseguiu desabilitar tcp delay(nagle algorithm).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 53, WSAGetLastError()));
#elif defined(__linux__)

	int flag = O_NONBLOCK;
	if (fcntl(_accepted.fd, F_SETFL, flag) != 0)
		throw exception("[server::init_option_accepted_socket][Error] nao conseguiu habilitar o NONBLOCK(fcntl).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 53, errno));

	// keepalive do linux
	int keepalive = 1; // Enable
	if (setsockopt(_accepted.fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) == -1)
		throw exception("[server::init_option_accepted_socket][error] nao conseguiu habilitaro keepalive(setsockopt).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 53, errno));

	keepalive = 20; // Keepalive 20 segundos
	if (setsockopt(_accepted.fd, SOL_TCP, TCP_KEEPIDLE, &keepalive, sizeof(keepalive)) == -1)
		throw exception("[server::init_option_accepted_socket][Error] nao conseguiu setar o keepalive idl time(setsockopt).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 53, errno));

	keepalive = 2;	// Keepalive interval probs, 2 segundos
	if (setsockopt(_accepted.fd, SOL_TCP, TCP_KEEPINTVL, &keepalive, sizeof(keepalive)) == -1)
		throw exception("[server::init_option_accepted_socket][Error] nao conseguiu setar o keepalive interval pobs(setsockopt).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 53, errno));

	// KEEPALIVE Log
	_smp::message_pool::getInstance().push(new message("[server::init_option_accepted_socket][Log] socket[ID=" + std::to_string(_accepted.fd)
			+ "] KEEPALIVE[ONOFF=1, TIME=20000, INTERVAL=2000] foi ativado para esse ", CL_FILE_LOG_AND_CONSOLE));

	if (setsockopt(_accepted.fd, IPPROTO_TCP, TCP_NODELAY, (char*)&tcp_nodelay, sizeof(tcp_nodelay)) == SOCKET_ERROR)
		throw exception("[server::init_option_accepted_socket][Error] nao conseguiu desabilitar tcp delay(nagle algorithm).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 53, errno));
#endif
};

void server::cmdUpdateListBlock_IP_MAC() {

	// List de IP Address Ban
	CmdListIPBan cmd_lib(true);		// Waiter

	NormalManagerDB::add(0, &cmd_lib, nullptr, nullptr);

	cmd_lib.waitEvent();

	if (cmd_lib.getException().getCodeError() != 0)
		throw cmd_lib.getException();

	v_ip_ban_list = cmd_lib.getListIPBan();

	// List de Mac Address Ban
	CmdListMacBan cmd_lmb(true);	// Waiter

	NormalManagerDB::add(0, &cmd_lmb, nullptr, nullptr);

	cmd_lmb.waitEvent();

	if (cmd_lmb.getException().getCodeError() != 0)
		throw cmd_lmb.getException();

	v_mac_ban_list = cmd_lmb.getList();
}

bool server::haveBanList(std::string _ip_address, std::string _mac_address, bool _check_mac) {

	// Verifica se é para verificar o MAC Address também
	if (_check_mac) {
		// Verifica primeiro se o MAC Address foi bloqueado

		// Cliente não enviou um MAC Address válido, bloquea essa conexão que é hacker que mudou o ProjectG
		if (_mac_address.empty())
			return true;	// Cliente não enviou um MAC Address válido, bloquea essa conexão que é hacker que mudou o ProjectG

		for (auto& el : v_mac_ban_list)
			if (!el.empty() && /*el.compare(_mac_address) == 0*/
#if defined(_WIN32)
				_stricmp(el.c_str(), _mac_address.c_str()) == 0
#elif defined(__linux__)
				strcasecmp(el.c_str(), _mac_address.c_str()) == 0
#endif
			)
				return true;	// MAC Address foi bloqueado
	}


	// IP Address inválido, bloquea essa conexão que é Hacker ou Bug
	if (_ip_address.empty())
		return true;

	// Verifica se o IP Address foi bloqueado
	uint32_t ip = 0u;

	inet_pton(AF_INET, _ip_address.c_str(), &ip);

	ip = ntohl(ip);

	for (auto el : v_ip_ban_list) {

		if (el.type == IPBan::_TYPE::IP_BLOCK_NORMAL) {

			if ((ip & el.mask) == (el.ip & el.mask))
				return true;

		}else if (el.type == IPBan::_TYPE::IP_BLOCK_RANGE) {

			// Range
			if (el.ip <= ip && ip <= el.mask)
				return true;
		}
	}

	return false;
}

session* server::HasLoggedWithOuterSocket(session& _session) {
	
	auto s = m_session_manager.findAllSessionByUID(_session.getUID());

	for (auto& el : s)
		if (el->m_oid != _session.m_oid && el->isConnected())
			return el;

	return nullptr;
}

void server::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		_smp::message_pool::getInstance().push(new message("[server::SQLDBResponse][WARNING] _arg is nullptr, na msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// Por Hora só sai, depois faço outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[server::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto *_server = reinterpret_cast< server* >(_arg);

	switch (_msg_id) {
	case 1:	// updateServerList
		_server->updateServerList(reinterpret_cast< CmdServerList* >(&_pangya_db)->getServerList());
		break;
	case 0:
	default:
		break;
	}
};

int server::end_time_shutdown(void* _arg1, void* _arg2) {

	server *s = reinterpret_cast<server*>(_arg1);
	uint32_t time_sec = (uint32_t)(size_t)_arg2;

	try {

		s->shutdown_time(time_sec);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[server::end_time_shutdown][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

	return 0;
};

void server::start() {

	if (m_state != FAILURE) {

		try {

			// Start Unit Connect for Try Connection with Auth Server
			if (m_unit_connect != nullptr)
				m_unit_connect->start();

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
			_smp::message_pool::getInstance().push(new message("[server::start][Error] desconhecido.", CL_FILE_LOG_AND_CONSOLE));
		}
	}else {
		_smp::message_pool::getInstance().push(new message("[server::start][Error] Server Inicializado com falha, fechando o server.", CL_FILE_LOG_AND_CONSOLE));
		
		waitAllThreadFinish(INFINITE);
	}
};

uint32_t server::getUID() {
	return m_si.uid;
};

session* server::findSession(uint32_t _uid, bool _oid) {
	return (_oid ? m_session_manager.findSessionByOID(_uid) : m_session_manager.findSessionByUID(_uid));
};

void server::destroy_unit() {

	if (m_unit_connect != nullptr)
		delete m_unit_connect;

	m_unit_connect = nullptr;
}

void server::commandScan() {
	
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
				_smp::message_pool::getInstance().push(new message("[server::commandScan][Log] Event Shutdown Set, Desligando o server.", CL_FILE_LOG_AND_CONSOLE));

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

				_smp::message_pool::getInstance().push(new message("[server::commandScan][Log] Event Shutdown Set, Desligando o server.", CL_FILE_LOG_AND_CONSOLE));

				shutdown = true;	// Shutdown
			}
		}
	}
#endif
};

void server::config_init() {

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

	try {

		m_Bot_TTL = m_reader_ini.readInt("OPTION", "ANTIBOTTTL");

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[server::config_init][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Usa o valor padrão do anti bot TTL
		m_Bot_TTL = 1000u;
	}
	
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

void server::setAcceptConnection() {

#if defined(_WIN32)
	if (EventAcceptConnection != INVALID_HANDLE_VALUE)
		SetEvent(EventAcceptConnection);
#elif defined(__linux__)
	if (EventAcceptConnection != nullptr)
		EventAcceptConnection->set();
#endif
};

void server::closeHandles() {

#if defined(_WIN32)
	if (EventMoreAccept != INVALID_HANDLE_VALUE)
		CloseHandle(EventMoreAccept);

	if (EventShutdown != INVALID_HANDLE_VALUE)
		CloseHandle(EventShutdown);

	if (EventShutdownServer != INVALID_HANDLE_VALUE)
		CloseHandle(EventShutdownServer);

	if (EventAcceptConnection != INVALID_HANDLE_VALUE)
		CloseHandle(EventAcceptConnection);

	EventMoreAccept = INVALID_HANDLE_VALUE;
	EventShutdown = INVALID_HANDLE_VALUE;
	EventShutdownServer = INVALID_HANDLE_VALUE;
	EventAcceptConnection = INVALID_HANDLE_VALUE;
#elif defined(__linux__)
	if (EventMoreAccept != nullptr)
		delete EventMoreAccept;

	if (EventShutdown != nullptr)
		delete EventShutdown;

	if (EventShutdownServer != nullptr)
		delete EventShutdownServer;

	if (EventAcceptConnection != nullptr)
		delete EventAcceptConnection;

	EventMoreAccept = nullptr;
	EventShutdown = nullptr;
	EventShutdownServer = nullptr;
	EventAcceptConnection = nullptr;
#endif
}

void server::shutdown() {

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

#if defined(_WIN32)
void server::init_LpFnAccetEx(SOCKET _listener) {

	m_GuidAcceptEx = WSAID_ACCEPTEX;
	DWORD ret = 0u;

	if (WSAIoctl(_listener, SIO_GET_EXTENSION_FUNCTION_POINTER, &m_GuidAcceptEx, sizeof(m_GuidAcceptEx), &m_lpfnAcceptEx, sizeof(m_lpfnAcceptEx), &ret, NULL, NULL) == SOCKET_ERROR)
		throw exception("[server::init_LpFnAcceptEx] Error ao pegar AcceptEx Addrs.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::LOGIN_SERVER, 1, WSAGetLastError()));
};
#endif

#if defined(_WIN32)
void server::init_LpFnGetAcceptExSockaddrs(SOCKET _listener) {

	m_GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
	DWORD ret = 0u;

	if (WSAIoctl(_listener, SIO_GET_EXTENSION_FUNCTION_POINTER, &m_GuidGetAcceptExSockaddrs, sizeof(m_GuidGetAcceptExSockaddrs), &m_lpfnGetAcceptExSockaddrs, sizeof(m_lpfnGetAcceptExSockaddrs), &ret, NULL, NULL) == SOCKET_ERROR)
		throw exception("[server::ini_LpFnGetAcceptExSockaddrs] error nao conseguiu pegar o addr da funcao getAcceptExAddrs.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 51, WSAGetLastError()));
};
#endif

void server::authCmdInfoPlayerOnline(uint32_t _req_server_uid, uint32_t _player_uid) {

	try {

		auto s = m_session_manager.findSessionByUID(_player_uid);

		if (s != nullptr) {

			// Log
			_smp::message_pool::getInstance().push(new message("[server::authCmdInfoPlayerOnline][log] Comando do Auth Server, Server[UID=" + std::to_string(_req_server_uid)
					+ "] pediu o info do Player[UID=" + std::to_string(s->getUID()) + "]", CL_FILE_LOG_AND_CONSOLE));

			AuthServerPlayerInfo aspi(s->getUID(), s->getID(), s->getIP());

			// UPDATE ON Auth Server
			m_unit_connect->sendInfoPlayerOnline(_req_server_uid, aspi);

		}else {

			// Log
			_smp::message_pool::getInstance().push(new message("[server::authCmdInfoPlayerOnline][Log] Comando do Auth Server, Server[UID=" + std::to_string(_req_server_uid)
					+ "] pediu info do Player[UID=" + std::to_string(_player_uid) + "], mas nao encontrou ele no server.", CL_FILE_LOG_AND_CONSOLE));

			// UPDATE ON Auth Server
			m_unit_connect->sendInfoPlayerOnline(_req_server_uid, AuthServerPlayerInfo(_player_uid));
		}

	}catch (exception& e) {

		// UPDATE ON Auth Server - Error reply
		m_unit_connect->sendInfoPlayerOnline(_req_server_uid, AuthServerPlayerInfo(_player_uid));

		_smp::message_pool::getInstance().push(new message("[server::authCmdInfoPlayerOnline][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void server::authCmdSendCommandToOtherServer(packet& _packet) {

	try {

		func_arr::func_arr_ex* func = nullptr;

		uint32_t req_server_uid = _packet.readUint32();
		unsigned short command_id = _packet.readUint16();

		try {
		
			func = packet_func_base::funcs_as.getPacketCall(command_id);

			if (func != nullptr && func->execCmd(&_packet))
				throw exception("[server::authCmdSendCommandToOtherServer][Error] Ao tratar o Comando. ID: " + std::to_string(command_id)
						+ "(0x" + hex_util::ltoaToHex(command_id) + ").", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME_SERVER, 5000, 0));
	
		}catch (exception& e) {

			if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::FUNC_ARR/*Packet_func Erro, Warning e etc*/) {
			
				_smp::message_pool::getInstance().push(new message("[server::authCmdSendCommandToOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[server::authCmdSendCommandToOtherServer][Log] Comando ID desconhecido enviado pelo Auth Server[SERVER_UID=" + std::to_string(req_server_uid) + "] size packet: " 
						+ std::to_string(_packet.getSize()) + "\n" + hex_util::BufferToHexString(_packet.getBuffer(), _packet.getSize()), CL_FILE_LOG_AND_CONSOLE));
	#else
				_smp::message_pool::getInstance().push(new message("[server::authCmdSendCommandToOtherServer][Log] Comando ID desconhecido enviado pelo Auth Server[SERVER_UID=" + std::to_string(req_server_uid) + "] size packet: "
						+ std::to_string(_packet.getSize()) + "\n" + hex_util::BufferToHexString(_packet.getBuffer(), _packet.getSize()), CL_ONLY_FILE_LOG));
	#endif
			}else
				throw;
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[server::authCmdSendCommandToOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void server::authCmdSendReplyToOtherServer(packet& _packet) {

	try {

		func_arr::func_arr_ex* func = nullptr;

		uint32_t req_server_uid = _packet.readUint32();
		unsigned short command_id = _packet.readUint16();

		try {
		
			func = packet_func_base::funcs_as.getPacketCall(command_id);

			if (func != nullptr && func->execCmd(&_packet))
				throw exception("[server::authCmdSendReplyToOtherServer][Error] Ao tratar o Comando. ID: " + std::to_string(command_id)
						+ "(0x" + hex_util::ltoaToHex(command_id) + ").", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GAME_SERVER, 5001, 0));
	
		}catch (exception& e) {

			if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::FUNC_ARR/*Packet_func Erro, Warning e etc*/) {
			
				_smp::message_pool::getInstance().push(new message("[server::authCmdSendReplyToOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

	#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[server::authCmdSendReplyToOtherServer][Log] Reply ID desconhecido enviado pelo Auth Server[SERVER_UID=" + std::to_string(req_server_uid) + "] size packet: " 
						+ std::to_string(_packet.getSize()) + "\n" + hex_util::BufferToHexString(_packet.getBuffer(), _packet.getSize()), CL_FILE_LOG_AND_CONSOLE));
	#else
				_smp::message_pool::getInstance().push(new message("[server::authCmdSendReplyToOtherServer][Log] Reply ID desconhecido enviado pelo Auth Server[SERVER_UID=" + std::to_string(req_server_uid) + "] size packet: "
						+ std::to_string(_packet.getSize()) + "\n" + hex_util::BufferToHexString(_packet.getBuffer(), _packet.getSize()), CL_ONLY_FILE_LOG));
	#endif
			}else
				throw;
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[server::authCmdSendReplyToOtherServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void server::sendCommandToOtherServerWithAuthServer(packet& _packet, uint32_t _send_server_uid_or_type) {

	try {

		// Envia o comando para o outro server com o Auth Server
		m_unit_connect->sendCommandToOtherServer(_send_server_uid_or_type, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[server::sendCommandToOtherServerWithAuthServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void server::sendReplyToOtherServerWithAuthServer(packet& _packet, uint32_t _send_server_uid_or_type) {

	try {

		// Envia a resposta para o outro server com o Auth Server
		m_unit_connect->sendReplyToOtherServer(_send_server_uid_or_type, _packet);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[server::sendReplyToOtherServerWithAuthServer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

// Chat Discord
bool server::getChatDiscord() {
	return m_chat_discord;
};

void server::setChatDiscord(bool _chat_discord) {
	m_chat_discord = _chat_discord;
};

// Quem usa é só o Game Server(Smart Calculator e Chat History) e Message Server(Chat History)
void server::sendSmartCalculatorReplyToPlayer(const uint32_t _uid, std::string _from, std::string _msg) {
	UNREFERENCED_PARAMETER(_uid);
	UNREFERENCED_PARAMETER(_from);
	UNREFERENCED_PARAMETER(_msg);
}

// Quem usa é só o Game Server(Smart Calculator e Chat History) e Message Server(Chat History)
void server::sendNoticeGMFromDiscordCmd(std::string& _notice) {
	UNREFERENCED_PARAMETER(_notice);
};

void server::sendMessageToDiscordChatHistory(std::string _nickname, std::string _msg) {

	try {

		// Invalid msg or nickname
		if (_nickname.empty() || _msg.empty())
			return;

		// Verifica se o m_chat_discod flag está ativo para enviar o chat para o discord
		if (m_si.rate.smart_calculator && m_chat_discord)
			sSmartCalculator::getInstance().sendCommandServer(_nickname + " - " + _msg);
		
	}catch (exception& e) {

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[server::sendMessageToDiscordChatHistory][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[server::sendMessageToDiscordChatHistory][ErrorSystem] " + e.getFullMessageError(), CL_ONLY_FILE_LOG));
#endif // _DEBUG
	}
};
