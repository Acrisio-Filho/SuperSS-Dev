// Arquivo unit_connect.cpp
// Criado em 02/12/2018 as 14:36 por Acrisio
// Implementação da classe unit_connect

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <signal.h>
#endif

#include "unit_connect.hpp"
#include "../SOCKET/socket.h"
#include "../UTIL/exception.h"
#include "../UTIL/message_pool.h"
#include "../SOCKET/session.h"
#include "../UTIL/hex_util.h"
#include <ctime>
#include "../TYPE/stda_error.h"

#include "../PACKET/packet_func.h"

#include "../Server/server.h"

#if defined(_WIN32)
#include <DbgHelp.h>
#endif

#define GET_FUNC_SEND getPacketS
#define SET_FUNC_SEND setPacketS

#define GET_FUNC_RECV getPacketR
#define SET_FUNC_RECV setPacketR

#define BEGIN_LOOP_TRANSLATE_BUFFER_TO_PACKET(_ph, _sz_ph, _get_func, _set_func) if (_session->_get_func() != nullptr) { \
													if (_session->_get_func()->getSizeMaked() == 0) { \
														if (_session->_get_func()->getMakedBuf().len < (_sz_ph)) { \
															left = (_sz_ph) - _session->_get_func()->getMakedBuf().len; \
															if (left <= (int)dwIOsize) { \
																_session->_get_func()->add_maked(lpBuffer->getBuffer(), left); \
																lpBuffer->consume(left); \
																dwIOsize -= left; \
\
																memcpy(&(_ph), _session->_get_func()->getMakedBuf().buf, (_sz_ph)); \
\
																_session->_get_func()->init_maked((_ph).size + (_sz_ph)); \
															}else { \
																_smp::message_pool::getInstance().push(new message("IP: " + std::string(_session->getIP()) + " Nao era pra entra aqui as chances sao pequenas de entrar aqui (2).", CL_FILE_LOG_AND_CONSOLE)); \
																_session->_get_func()->add_maked(lpBuffer->getBuffer(), dwIOsize); \
																lpBuffer->consume(dwIOsize); \
																dwIOsize = 0; \
															} \
														}else { \
															memcpy(&(_ph), _session->_get_func()->getMakedBuf().buf, (_sz_ph)); \
\
															_session->_get_func()->init_maked((_ph).size + (_sz_ph)); \
														} \
													} \
\
													left = (uint32_t)_session->_get_func()->getSizeMaked() - _session->_get_func()->getMakedBuf().len; \
													if (left > (int)dwIOsize) { \
														_session->_get_func()->add_maked(lpBuffer->getBuffer(), dwIOsize); \
														lpBuffer->consume(dwIOsize); \
														dwIOsize = 0; \
													}else { \
														_session->_get_func()->add_maked(lpBuffer->getBuffer(), left); \
														lpBuffer->consume(left); \
														dwIOsize -= left; \
\
														_packet = new packet((size_t)_session->_get_func()->getSizeMaked()/*, _session*/); \
														_packet->add_maked(_session->_get_func()->getMakedBuf().buf, _session->_get_func()->getMakedBuf().len); \

#define MED_LOOP_TRANSLATE_BUFFER_TO_PACKET(_ph, _sz_ph, STDA_OT, _get_func, _set_func) _packet->init_maked(); \
\
														delete _session->_get_func(); \
\
														_session->_set_func(nullptr); \
\
														/* trata */ \
														postIoOperationL(_session, (Buffer*)_packet, (STDA_OT)); \
\
														_packet = nullptr; \
													} \
												}else { \
													if (dwIOsize >= (_sz_ph)) {	/* >= o certo, vou colocar so ">" para testar os exceptions */ \
														lpBuffer->peek(&(_ph), (_sz_ph)); \
\
														if (((_ph).size + (_sz_ph)) <= dwIOsize) { \
															_packet = new packet((size_t)((_ph).size + (_sz_ph))/*, _session*/); \
															_packet->add_maked(lpBuffer->getBuffer(), (_ph).size + (_sz_ph)); \
\
															lpBuffer->consume((_ph).size + (_sz_ph)); \
															dwIOsize -= ((_ph).size + (_sz_ph)); \

#define END_LOOP_TRANSLATE_BUFFER_TO_PACKET(_ph, _sz_ph, STDA_OT, _get_func, _set_func) _packet->init_maked(); \
\
															/* trata */ \
															postIoOperationL(_session, (Buffer*)_packet, (STDA_OT)); \
\
															_packet = nullptr; \
														}else { \
															_session->_set_func(new packet((size_t)((_ph).size + (_sz_ph))/*, _session*/)); \
															_session->_get_func()->add_maked(lpBuffer->getBuffer(), dwIOsize); \
															lpBuffer->consume(dwIOsize); \
															dwIOsize = 0; \
														} \
													}else { \
														_session->_set_func(new packet((size_t)0/*, _session*/)); \
														_session->_get_func()->add_maked(lpBuffer->getBuffer(), dwIOsize); \
														lpBuffer->consume(dwIOsize); \
														dwIOsize = 0; \
													} \
												} \

#define LOOP_TRANSLATE_BUFFER_TO_PACKET_SERVER_UNITC(_ph, _sz_ph, STDA_OT) \
					BEGIN_LOOP_TRANSLATE_BUFFER_TO_PACKET((_ph), (_sz_ph), GET_FUNC_SEND, SET_FUNC_SEND); \
						\
					_packet->unMakeFull(_session->m_key); \
						\
					MED_LOOP_TRANSLATE_BUFFER_TO_PACKET((_ph), (_sz_ph), (STDA_OT), GET_FUNC_SEND, SET_FUNC_SEND); \
						\
					_packet->unMakeFull(_session->m_key); \
						\
					END_LOOP_TRANSLATE_BUFFER_TO_PACKET((_ph), (_sz_ph), (STDA_OT), GET_FUNC_SEND, SET_FUNC_SEND); \

#define LOOP_TRANSLATE_BUFFER_TO_PACKET_CLIENT_UNITC(_phc, _sz_phc, STDA_OT) \
					BEGIN_LOOP_TRANSLATE_BUFFER_TO_PACKET((_phc), (_sz_phc), GET_FUNC_RECV, SET_FUNC_RECV); \
						\
					_packet->unMakeFull(_session->m_key); \
						\
					MED_LOOP_TRANSLATE_BUFFER_TO_PACKET((_phc), (_sz_phc), (STDA_OT), GET_FUNC_RECV, SET_FUNC_RECV); \
						\
					_packet->unMakeFull(_session->m_key); \
						\
					END_LOOP_TRANSLATE_BUFFER_TO_PACKET((_phc), (_sz_phc), (STDA_OT), GET_FUNC_RECV, SET_FUNC_RECV); \

#define CLEAR_BUFFER_LOOP_CLIENT \
	if (lpBuffer != nullptr && lpBuffer->getUsed() > 0) \
		lpBuffer->consume(lpBuffer->getUsed()); \
\
	dwIOsize = 0; \

#define CLEAR_BUFFER_LOOP_SERVER \
	if (lpBuffer != nullptr) { \
		if (lpBuffer->getUsed() > dwIOsize) \
			lpBuffer->consume(dwIOsize); \
		else if (lpBuffer->getUsed() > 0) \
			lpBuffer->consume(lpBuffer->getUsed()); \
	} \
\
	dwIOsize = 0; \

#define CLEAR_PACKET_LOOP_SIMPLE \
	if (_packet != nullptr) { \
		delete _packet; \
		_packet = nullptr; \
	} \

#define CLEAR_PACKET_LOOP_WITH_MSG \
	if (_packet != nullptr) { \
		_smp::message_pool::getInstance().push(new message("Key: " + std::to_string((int)_session->m_key), CL_ONLY_FILE_LOG)); \
		_smp::message_pool::getInstance().push(new message(hex_util::BufferToHexString((unsigned char*)_packet->getMakedBuf().buf, _packet->getMakedBuf().len), CL_ONLY_FILE_LOG)); \
		delete _packet; \
		_packet = nullptr; \
	} \

#define CLEAR_PACKET_LOOP_SESSION(_get_func, _set_func) \
	if (_session->_get_func() != nullptr) { \
		delete _session->_get_func(); \
		_session->_set_func(nullptr); \
	} \

#define STDA_OT_INVALID (uint32_t)(~0)

#define MAKE_SEND_BUFFER(_packet, _session) (_packet).makeFull((_session)->m_key); \
						 WSABUF mb = (_packet).getMakedBuf(); \
						 try { \
							(_session)->requestSendBuffer(mb.buf, mb.len); \
						 }catch (exception& e) { \
							if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 1)) \
								throw; \
						 } \

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[unit_connect_base::" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_CONNECT, 1, 0)); \

using namespace stdA;

unit_connect_base::unit_connect_base(ServerInfoEx& _si) 
	: m_iocp_io(0), 
#if defined(_WIN32)
	m_iocp_logical(0), 
	m_iocp_send(0)
#elif defined(__linux__)
	m_iocp_logical(), 
	m_iocp_send()
#endif
	, m_reader_ini(_INI_PATH), m_session(*this, _si) 
#if defined(_WIN32)
	, hEventContinueMonitor(INVALID_HANDLE_VALUE), 
	hEventTryConnect(INVALID_HANDLE_VALUE),
#elif defined(__linux__)
	, hEventContinueMonitor(nullptr), 
	hEventTryConnect(nullptr),
#endif 
	m_state(STATE::UNINITIALIZED) {

	try {

		size_t i = 0;

		m_transfer_bytes_io = 0ull;
		m_1sec = 0;

#if defined(_WIN32)
		InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
		INIT_PTHREAD_MUTEXATTR_RECURSIVE;
		INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
		DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

		// Workers IO Threads
		m_threads.push_back(new thread(TT_WORKER_IO, unit_connect_base::_worker_io, (LPVOID)this, THREAD_PRIORITY_ABOVE_NORMAL));

		// Workers Logical Threads
		for (auto i = 0u; i < 2u; ++i)	// 2 Work Logical para ficar mais fácil
			m_threads.push_back(new thread(TT_WORKER_LOGICAL, unit_connect_base::_worker_logical, (LPVOID)this, THREAD_PRIORITY_ABOVE_NORMAL));

		// Workers Send Threads
		m_threads.push_back(new thread(TT_WORKER_SEND, unit_connect_base::_worker_send, (LPVOID)this, THREAD_PRIORITY_ABOVE_NORMAL));	// Estava ABOVE

#if defined(_WIN32)
		if ((hEventContinueMonitor = CreateEvent(NULL, true, false, NULL)) == INVALID_HANDLE_VALUE)
			throw exception("[unit_connect::unit_connect][Error] ao criar o evento Continue Monitor.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_CONNECT, 54, GetLastError()));

		if ((hEventTryConnect = CreateEvent(NULL, true, false, NULL)) == INVALID_HANDLE_VALUE)
			throw exception("[unit_connect::unit_connect][Error] ao criar o evento try connect.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_CONNECT, 54, GetLastError()));
#elif defined(__linux__)

		hEventContinueMonitor = new Event(true, 0u);

		if (!hEventContinueMonitor->is_good()) {

			delete hEventContinueMonitor;

			hEventContinueMonitor = nullptr;

			throw exception("[unit_connect::unit_connect][Error] ao criar o evento Continue Monitor.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_CONNECT, 54, errno));
		}

		hEventTryConnect = new Event(true, 0u);

		if (!hEventTryConnect->is_good()) {

			delete hEventTryConnect;

			hEventTryConnect = nullptr;

			throw exception("[unit_connect::unit_connect][Error] ao criar o evento try connect.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_CONNECT, 54, errno));
		}
#endif

		// Monitor Thread
		m_thread_monitor = new thread(TT_MONITOR, unit_connect_base::_monitor, (LPVOID)this/*, THREAD_PRIORITY_BELOW_NORMAL*/);

		// Good
		m_state = STATE::GOOD;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[unit_connect::unit_connect][ErroSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		m_state = STATE::FAILURE;
	}
}

unit_connect_base::~unit_connect_base() {

	waitAllThreadFinish(INFINITE);

	if (m_thread_monitor != nullptr)
		delete m_thread_monitor;

	m_thread_monitor = nullptr;

	if (!m_threads.empty()) {

		for (size_t i = 0; i < m_threads.size(); i++) {
			if (m_threads[i] != nullptr) {
				try {
					delete m_threads[i];
				}catch (exception& e) {
					if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::THREAD)
						_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}
			}
		}

		m_threads.clear();
		m_threads.shrink_to_fit();
	}

#if defined(_WIN32)
	if (hEventContinueMonitor != INVALID_HANDLE_VALUE)
		CloseHandle(hEventContinueMonitor);

	if (hEventTryConnect != INVALID_HANDLE_VALUE)
		CloseHandle(hEventTryConnect);

	hEventContinueMonitor = INVALID_HANDLE_VALUE;
	hEventTryConnect = INVALID_HANDLE_VALUE;

	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	if (hEventContinueMonitor != nullptr)
		delete hEventContinueMonitor;

	if (hEventTryConnect != nullptr)
		delete hEventTryConnect;

	hEventContinueMonitor = nullptr;
	hEventTryConnect = nullptr;

	pthread_mutex_destroy(&m_cs);
#endif

	m_state = STATE::UNINITIALIZED;
}

#if defined(_WIN32)
DWORD __stdcall unit_connect_base::_worker_io(LPVOID lpParameter) {
#elif defined(__linux__)
void* unit_connect_base::_worker_io(LPVOID lpParameter) {
#endif
	BEGIN_THREAD_SETUP(unit_connect_base);

	pTP->worker_io();

	END_THREAD_SETUP("worker_io()")
}

#if defined(_WIN32)
DWORD __stdcall unit_connect_base::_worker_logical(LPVOID lpParameter) {
#elif defined(__linux__)
void* unit_connect_base::_worker_logical(LPVOID lpParameter) {
#endif
	BEGIN_THREAD_SETUP(unit_connect_base);

	pTP->worker_logical();

	END_THREAD_SETUP("worker_logical()")
}

#if defined(_WIN32)
DWORD __stdcall unit_connect_base::_worker_send(LPVOID lpParameter) {
#elif defined(__linux__)
void* unit_connect_base::_worker_send(LPVOID lpParameter) {
#endif
	BEGIN_THREAD_SETUP(unit_connect_base);

	pTP->worker_send();

	END_THREAD_SETUP("worker_send()")
}

#if defined(_WIN32)
DWORD __stdcall unit_connect_base::_monitor(LPVOID lpParameter) {
#elif defined(__linux__)
void* unit_connect_base::_monitor(LPVOID lpParameter) {
#endif
	BEGIN_THREAD_SETUP(unit_connect_base);

	pTP->monitor();

	END_THREAD_SETUP("monitor()")
}

void unit_connect_base::waitAllThreadFinish(DWORD dwMilleseconds) {

	// Monitor Thread
#if defined(_WIN32)
	if (hEventContinueMonitor != INVALID_HANDLE_VALUE)
		SetEvent(hEventContinueMonitor);
#elif defined(__linux__)
	if (hEventContinueMonitor != nullptr)
		hEventContinueMonitor->set();
#endif

	if (m_thread_monitor != nullptr)
		m_thread_monitor->waitThreadFinish(dwMilleseconds);

	for (size_t i = 0; i < m_threads.size(); i++) {

		switch (m_threads[i]->getTipo()) {
		case TT_WORKER_IO:
#if defined(_WIN32)
			m_iocp_io.postStatus((ULONG_PTR)nullptr, 0, nullptr);
#elif defined(__linux__)
			m_iocp_io.postStatus();
#endif
			break;
		case TT_WORKER_LOGICAL:
#if defined(_WIN32)
			m_iocp_logical.postStatus((ULONG_PTR)nullptr, 0, nullptr);
#elif defined(__linux__)
			m_iocp_logical.push(nullptr);
#endif
			break;
		case TT_WORKER_SEND:
#if defined(_WIN32)
			m_iocp_send.postStatus((ULONG_PTR)nullptr, 0, nullptr);
#elif defined(__linux__)
			m_iocp_send.push(nullptr);
#endif
			break;
		case TT_MONITOR:
#if defined(_WIN32)
			if (hEventContinueMonitor != INVALID_HANDLE_VALUE)
				SetEvent(hEventContinueMonitor);
#elif defined(__linux__)
			if (hEventContinueMonitor != nullptr)
				hEventContinueMonitor->set();
#endif
			break;
		}
	}

	for (size_t i = 0; i < m_threads.size(); i++)
		if (m_threads[i] != nullptr)
			m_threads[i]->waitThreadFinish(dwMilleseconds);
}

void unit_connect_base::postIoOperation(session* _session, Buffer* lpBuffer, DWORD dwIOsize, DWORD operation) {

	myOver *tmp = new myOver;

	tmp->clear();
	tmp->tipo = operation;
	tmp->buffer = lpBuffer;

#if defined(_WIN32)
	UNREFERENCED_PARAMETER(dwIOsize);
	m_iocp_send.postStatus((ULONG_PTR)_session, 0, (LPOVERLAPPED)tmp);		// Esse tinha Index[session->m_key], era um array de 16 iocp
#elif defined(__linux__)
	m_iocp_send.push(new stThreadpoolMessage(_session, (Buffer*)tmp, dwIOsize));	// Esse tinha Index[session->m_key], era um array de 16 iocp
#endif
}

void unit_connect_base::DisconnectSession(session *_session) {

	if (_session != nullptr && _session->isConnected()) {

		// on Disconnect
		onDisconnect();

		_session->lock();

		_session->clear();

		_session->setConnected(false);

		_session->unlock();
	}
}

void unit_connect_base::start() {

#if defined(_WIN32)
	if (hEventTryConnect != INVALID_HANDLE_VALUE)
		SetEvent(hEventTryConnect);
#elif defined(__linux__)
	if (hEventTryConnect != nullptr)
		hEventTryConnect->set();
#endif
}

bool unit_connect_base::isLive() {
	return (m_session.getState() && m_session.isConnected());
}

#if defined(_WIN32)
DWORD unit_connect_base::worker_io() {
#elif defined(__linux__)
void* unit_connect_base::worker_io() {
#endif
	
	try {

		_smp::message_pool::getInstance().push(new message("Ola console, here is thread worker IO : " + std::to_string(
#if defined(_WIN32)
			GetCurrentThreadId()
#elif defined(__linux__)
			gettid()
#endif
		) + "."));

		Buffer *lpBuffer = nullptr;
		session *_session = nullptr;
#if defined(_WIN32)
		DWORD dwIOsize = 0, operation = ~0;
#elif defined(__linux__)
		epoll_event ep_ev = { 0u };
		int32_t ret_epoll_wait = 0;
#endif

		while (1) {
			try {
#if defined(_WIN32)
				try {
					operation = ~0;

					m_iocp_io.getStatus((PULONG_PTR)&_session, &dwIOsize, (LPOVERLAPPED*)&lpBuffer);

				}catch (exception& e) {
					
					if (STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_SEM_TIMEOUT) {
						DWORD flags_over = 0u;
						DWORD last_err = 0u;

						if (!WSAGetOverlappedResult(_session->m_sock, (LPOVERLAPPED)lpBuffer, &dwIOsize, FALSE, &flags_over)
								&& (last_err = WSAGetLastError()) != WSA_IO_INCOMPLETE && last_err != WSAETIMEDOUT)
							throw exception("[unit_connect_base::worker_io][Error] exception ERROR_SEM_TIMEOUT dwIOsize=" + std::to_string(dwIOsize) + ", Flags_Over="
									+ std::to_string(flags_over) + " que retornou da operacao no iocp. session[IP=" + _session->getIP() + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 255, last_err));

						// Marca na session que o socket, levou DC, chegou ao limit de retramission do TCP para transmitir os dados
						// TCP sockets is that the maximum retransmission count and timeout have been reached on a bad(or broken) link
						_session->m_connection_timeout = true;

						// Se passar tenho que colocar o dwIOsize para zero para ele ser disconnectado
						_smp::message_pool::getInstance().push(new message("[unit_connect_base::worker_io][WARNING] exception ERROR_SEM_TIMEOUT dwIOsize=" + std::to_string(dwIOsize) + ", Flags_Over="
								+ std::to_string(flags_over) + " que retornou da operacao no iocp[WSAError=" + std::to_string(last_err) + "]. session[IP=" + _session->getIP() + "]", CL_FILE_LOG_AND_CONSOLE));

						// Se for STDA_OT_SEND_COMPLETED reseta o send
						if (lpBuffer != nullptr && (lpBuffer->getOperation() == STDA_OT_SEND_COMPLETED || lpBuffer->getOperation() == STDA_OT_SEND_RAW_COMPLETED))
							_session->releaseSend();

						// Zera para desconectar a session
						// e post no recv para ele desconnectar a session
						dwIOsize = 0u;
						
						if (lpBuffer != nullptr)
							lpBuffer->setOperation(STDA_OT_RECV_COMPLETED);

					}else if (!(STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_NETNAME_DELETED || STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_CONNECTION_ABORTED
						|| STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_OPERATION_ABORTED))
						throw;
				}

				if (_session == nullptr)
					break;

				if (lpBuffer != nullptr)
					operation = lpBuffer->getOperation();

#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("Operation type: " + std::to_string(operation) + "\tlen: " + std::to_string(dwIOsize), CL_ONLY_FILE_LOG));
#endif

				EnterCriticalSection(&m_cs);

				if (((std::clock() - m_1sec) / (double)CLOCKS_PER_SEC) > 1.0f) {
					_smp::message_pool::getInstance().push_back(new message("IO Transfer Bytes for second: " + std::to_string(m_transfer_bytes_io), CL_ONLY_FILE_LOG_IO_DATA));
					m_transfer_bytes_io = 0ull;
					m_1sec = std::clock();
				}else
					m_transfer_bytes_io += dwIOsize;

				LeaveCriticalSection(&m_cs);

				translate_operation(_session, dwIOsize, lpBuffer, operation);
#elif defined(__linux__)
				// Clear
				_session = nullptr;
				lpBuffer = nullptr;
				
				try {

					// Clear
					ep_ev = { 0u };

					ret_epoll_wait = m_iocp_io.getStatus(&ep_ev, 1, INFINITE);

				}catch (exception& e) {

					// check erro de interrupição do epoll_wait com sinal
					if (STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) != EINTR)
						throw;
					else
						_smp::message_pool::getInstance().push(new message("[unit_connect_base::worker_io][ErrorSystem] epoll interrupted by signal. " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}

				// Shutdown thread
				if (m_iocp_io.isExitEpollEvent(ep_ev))
					break;

				// timeout
				if (ret_epoll_wait == 0)
					continue;

				// Invalid session
				if (ep_ev.data.ptr == nullptr)
					continue;

				_session = reinterpret_cast< session* >(ep_ev.data.ptr);

				// Pega esse só para excluir a session se tiver erro
				lpBuffer = &_session->m_buff_r.buff;

				// error
				if ((ep_ev.events & EPOLLERR) == EPOLLERR) {

					int err_socket = 1234; // Padrão error
					socklen_t len_err_sock = sizeof(int);

					// Ignora retorno
					int rt_err = ::getsockopt(_session->m_sock.fd, SOL_SOCKET, SO_ERROR, &err_socket, &len_err_sock);

					// KeepAlive DC(disconnect)
					if (err_socket == ETIMEDOUT) {
						
						// Marca na session que o socket, levou DC, chegou ao limit de retramission do TCP para transmitir os dados
						// TCP sockets is that the maximum retransmission count and timeout have been reached on a bad(or broken) link
						_session->m_connection_timeout = true;

						// Se passar tenho que colocar o dwIOsize para zero para ele ser disconnectado
						_smp::message_pool::getInstance().push(new message("[unit_connect_base::worker_io][WARNING] exception ERROR_SEM_TIMEOUT no iocp[socket_error=" 
								+ std::to_string(err_socket) + "]. session[IP=" + _session->getIP() + "]", CL_FILE_LOG_AND_CONSOLE));

						// Zera para desconectar a session
						// e post no recv para ele desconnectar a session
						translate_operation(_session, 0/*disconnect*/, lpBuffer, STDA_OT_RECV_COMPLETED);

						// volta para o começo do while
						continue;
					
					}else	// Relança qualquer outro error
						throw exception("[unit_connect_base::worker_io][Error] ep_ev events[EVENTS=" + std::to_string(ep_ev.events) 
							+ ", SESSION=" + std::to_string((uintptr_t)_session) + ", IP=" + _session->getIP() + "] err_socket: " + std::to_string(err_socket) 
							+ ", ret_epoll_wait: " + std::to_string(ret_epoll_wait), STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_CONNECT, 5000, err_socket));
				}

				// Shutdown
				if ((ep_ev.events & EPOLLHUP) == EPOLLHUP) {

					// disconnect session
					translate_operation(_session, 0/*disconnect*/, lpBuffer, STDA_OT_RECV_COMPLETED);

					// volta para o começo do while
					continue;
				}

				// Disponível para read
				if ((ep_ev.events & EPOLLIN) == EPOLLIN) {
					
					lpBuffer = &_session->m_buff_r.buff;

					_session->requestRecvBuffer();
				}

				// Disponível para escrever no socket
				if ((ep_ev.events & EPOLLOUT) == EPOLLOUT) {

					lpBuffer = &_session->m_buff_s.buff;

					// se usar o postIoOperation pode dá dead lock, por que vai ficar esperando enviar, 
					// mas não tem thread para enviar, por que todas estão esperando ser enviado para poder enviar de novo
					send_new(_session, &_session->m_buff_s.buff, STDA_OT_SEND_REQUEST);
				}
#endif

			}catch (exception& e) {
//#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[unit_connect_base::worker_io][ErrorSystem][MY] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#else
	//			if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 2))
		//			_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#endif

				// Se for STDA_OT_SEND_COMPLETED reseta o send
				if (lpBuffer != nullptr && (lpBuffer->getOperation() == STDA_OT_SEND_COMPLETED || lpBuffer->getOperation() == STDA_OT_SEND_RAW_COMPLETED))
					_session->releaseSend();

				translate_operation(_session, 0/*disconnect*/, lpBuffer, STDA_OT_RECV_COMPLETED);

			}catch (std::exception& e) {
				
				_smp::message_pool::getInstance().push(new message(std::string("[unit_connect_base::worker_io][ErrorSystem][STD] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));

				// Se for STDA_OT_SEND_COMPLETED reseta o send
				if (lpBuffer != nullptr && (lpBuffer->getOperation() == STDA_OT_SEND_COMPLETED || lpBuffer->getOperation() == STDA_OT_SEND_RAW_COMPLETED))
					_session->releaseSend();

				translate_operation(_session, 0/*disconnect*/, lpBuffer, STDA_OT_RECV_COMPLETED);

			}catch (...) {

				std::cout << "worker_io() ->while Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";

				// Se for STDA_OT_SEND_COMPLETED reseta o send
				if (lpBuffer != nullptr && (lpBuffer->getOperation() == STDA_OT_SEND_COMPLETED || lpBuffer->getOperation() == STDA_OT_SEND_RAW_COMPLETED))
					_session->releaseSend();

				translate_operation(_session, 0/*disconnect*/, lpBuffer, STDA_OT_RECV_COMPLETED);
			}
		}
	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "worker_io() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}

	_smp::message_pool::getInstance().push(new message("Saindo de worker_io()..."));
	
#if defined(_WIN32)
	return 0u;
#elif defined(__linux__)
	return (void*)0u;
#endif
}

#if defined(_WIN32)
DWORD unit_connect_base::worker_logical() {
#elif defined(__linux__)
void* unit_connect_base::worker_logical() {
#endif
	
	try {

		_smp::message_pool::getInstance().push(new message("Ola console, here is thread worker logical : " + std::to_string(
#if defined(_WIN32)
			GetCurrentThreadId()
#elif defined(__linux__)
			gettid()
#endif
		) + "."));
		
		myOver* lpMyOver = nullptr;
#if defined(_WIN32)
		session *_session = nullptr;
		DWORD dwIOsize = 0;
#elif defined(__linux__)
		stThreadpoolMessage *tpMessage = nullptr;
#endif	
		DWORD operation = ~0;

		while (1) {
			try {
#if defined(_WIN32)
				operation = ~0;

				m_iocp_logical.getStatus((PULONG_PTR)&_session, &dwIOsize, (LPOVERLAPPED*)&lpMyOver);		// Esse tinha Index, era um array de 16 iocp

				if (_session == nullptr)
					break;

				if (lpMyOver != nullptr)
					operation = lpMyOver->tipo;

#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("Operation typeL: " + std::to_string(operation) + "\tlen: " + std::to_string(dwIOsize), CL_ONLY_FILE_LOG));
#endif

				if (lpMyOver != nullptr) {
					translate_operation(_session, dwIOsize, lpMyOver->buffer, operation);

					delete lpMyOver;
				}
#elif defined(__linux__)
				operation = ~0;

				tpMessage = m_iocp_logical.get(INFINITE);		// Esse tinha Index, era um array de 16 iocp

				if (tpMessage == nullptr)
					break;

				lpMyOver = (myOver*)tpMessage->buffer;

				if (lpMyOver != nullptr)
					operation = lpMyOver->tipo;

#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("Operation typeL: " + std::to_string(operation) + "\tlen: " + std::to_string(tpMessage->dwIOsize), CL_ONLY_FILE_LOG));
#endif

				if (lpMyOver != nullptr) {
					translate_operation(tpMessage->_session, tpMessage->dwIOsize, lpMyOver->buffer, operation);

					delete lpMyOver;
				}

				// free memory
				if (tpMessage != nullptr)
					delete tpMessage;
#endif

			}catch (exception& e) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#else
				if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 2))
					_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif
			}catch (std::exception& e) {
				_smp::message_pool::getInstance().push(new message(e.what(), CL_FILE_LOG_AND_CONSOLE));
			}catch (...) {
				std::cout << "worker_logical() ->while Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
			}
		}
	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "worker_logical() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}

	_smp::message_pool::getInstance().push(new message("Saindo de worker_logical()..."));

#if defined(_WIN32)
	return 0u;
#elif defined(__linux__)
	return (void*)0u;
#endif
}

#if defined(_WIN32)
DWORD unit_connect_base::worker_send() {
#elif defined(__linux__)
void* unit_connect_base::worker_send() {
#endif
	
	try {

		_smp::message_pool::getInstance().push(new message("Ola console, here is thread worker send : " + std::to_string(
#if defined(_WIN32)
			GetCurrentThreadId()
#elif defined(__linux__)
			gettid()
#endif
		) + "."));
		
		myOver* lpMyOver = nullptr;
#if defined(_WIN32)
		session *_session = nullptr;
		DWORD dwIOsize = 0;
#elif defined(__linux__)
		stThreadpoolMessage *tpMessage = nullptr;
#endif
		DWORD operation = ~0;

		while (1) {
			try {
#if defined(_WIN32)
				operation = ~0;

				m_iocp_send.getStatus((PULONG_PTR)&_session, &dwIOsize, (LPOVERLAPPED*)&lpMyOver);		// Esse tinha Index, era um array de 16 iocp

				if (_session == nullptr)
					break;
				
				if (lpMyOver != nullptr)
					operation = lpMyOver->tipo;

#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("Operation typeS: " + std::to_string(operation) + "\tlen: " + std::to_string(dwIOsize), CL_ONLY_FILE_LOG));
#endif

				if (lpMyOver != nullptr) {
					translate_operation(_session, dwIOsize, lpMyOver->buffer, operation);

					delete lpMyOver;
				}
#elif defined(__linux__)
				operation = ~0;

				tpMessage = m_iocp_send.get(INFINITE);		// Esse tinha Index, era um array de 16 iocp

				if (tpMessage == nullptr)
					break;

				lpMyOver = (myOver*)tpMessage->buffer;
				
				if (lpMyOver != nullptr)
					operation = lpMyOver->tipo;

#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("Operation typeS: " + std::to_string(operation) + "\tlen: " + std::to_string(tpMessage->dwIOsize), CL_ONLY_FILE_LOG));
#endif

				if (lpMyOver != nullptr) {
					translate_operation(tpMessage->_session, tpMessage->dwIOsize, lpMyOver->buffer, operation);

					delete lpMyOver;
				}

				// free memory
				if (tpMessage != nullptr)
					delete tpMessage;
#endif

			}catch (exception& e) {
#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#else
				if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 2))
					_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif
			}catch (std::exception& e) {
				_smp::message_pool::getInstance().push(new message(e.what(), CL_FILE_LOG_AND_CONSOLE));
			}catch (...) {
				std::cout << "worker_send() ->while Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
			}
		}
	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "worker_send() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}

	_smp::message_pool::getInstance().push(new message("Saindo de worker_send()..."));

#if defined(_WIN32)
	return 0u;
#elif defined(__linux__)
	return (void*)0u;
#endif
}

#if defined(_WIN32)
DWORD unit_connect_base::monitor() {
#elif defined(__linux__)
void* unit_connect_base::monitor() {
#endif
	
	try {
		
		_smp::message_pool::getInstance().push(new message("monitor iniciado com sucesso!"));

#if defined(_WIN32)
		if (hEventContinueMonitor == INVALID_HANDLE_VALUE)
			throw exception("[unit_connect_base::monitor][Error] hEventContinueMonitor invalid handle value. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_CONNECT, 55, 0));
#elif defined(__linux__)
		if (hEventContinueMonitor == nullptr)
			throw exception("[unit_connect_base::monitor][Error] hEventContinueMonitor invalid handle value. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_CONNECT, 55, 0));
#endif
		
		do {
			
			try {
		
				for (size_t i = 0; i < m_threads.size(); i++)
					if (m_threads[i] != nullptr && !m_threads[i]->isLive())
						m_threads[i]->init_thread();

				onHeartBeat();

			}catch (exception& e) {
				if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::THREAD)
					throw;
			}

#if defined(_WIN32)
		} while (WaitForSingleObject(hEventContinueMonitor, 5000/*5 Segundos*/) == WAIT_TIMEOUT);

		auto last_error = GetLastError();
#elif defined(__linux__)
		} while (hEventContinueMonitor != nullptr && hEventContinueMonitor->wait(5000/*5 Segundos*/) == WAIT_TIMEOUT);

		auto last_error = errno;
#endif

		if (last_error != 0)
			throw exception("[unit_connect_base::monitor][Error] no wait for single object.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_CONNECT, 56, last_error));

	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (std::exception& e) {
		_smp::message_pool::getInstance().push(new message(e.what(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "monitor() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}
		
	_smp::message_pool::getInstance().push(new message("Saindo de monitor()..."));

#if defined(_WIN32)
	return 0u;
#elif defined(__linux__)
	return (void*)0u;
#endif
}

inline void unit_connect_base::dispach_packet_same_thread(session& _session, packet *_packet) {
	CHECK_SESSION_BEGIN("dispach_packet_same_thread");

	func_arr::func_arr_ex* func = nullptr;

	try {
		func = funcs.getPacketCall(_packet->getTipo());
	}catch (exception& e) {
		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::FUNC_ARR/*Packet_func Erro, Warning e etc*/) {
			_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("size packet: " + std::to_string(_packet->getSize()) + "\n" + hex_util::BufferToHexString(_packet->getBuffer(), _packet->getSize()), CL_FILE_LOG_AND_CONSOLE));
//#endif

		}else
			throw;
	}

	try {

		_session.usa();

		// só pode contar o tick se for um pacote conhecido pelo, server, se contar por dados recebido, nunca vai derrubar o cliente mau intensionado
		_session.m_tick = std::clock();	// Tick time client para o TTL(time to live)

		ParamDispatchAS pd = { *(UnitPlayer*)&_session, _packet };

		if (func != nullptr && func->execCmd(&pd) != 0)
			_smp::message_pool::getInstance().push(new message("[unit_connect_base::dispach_packet_same_thread][Error] ao tratar o pacote. ID: " + std::to_string(_packet->getTipo()) + "(0x" + hex_util::ltoaToHex(_packet->getTipo()) + ").", CL_FILE_LOG_AND_CONSOLE));

		if (_session.devolve())
			DisconnectSession(&_session);

	}catch (exception& e) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*não pode usa session*/))
			if (_session.devolve())
				DisconnectSession(&_session);

	}catch (std::exception& e) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message(e.what(), CL_FILE_LOG_AND_CONSOLE));
#else
		UNREFERENCED_PARAMETER(e);
#endif

		if (_session.devolve())
			DisconnectSession(&_session);

	}catch (...) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("Unknown Error unit_connect_base::dispach_packet_same_thread()", CL_FILE_LOG_AND_CONSOLE));
#endif

		if (_session.devolve())
			DisconnectSession(&_session);
	}
};

inline void unit_connect_base::dispach_packet_sv_same_thread(session& _session, packet *_packet) {
	CHECK_SESSION_BEGIN("dispach_packet_sv_same_thread");

	func_arr::func_arr_ex* func = nullptr;

	try {
		func = funcs_sv.getPacketCall(_packet->getTipo());
	}catch (exception& e) {
		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::FUNC_ARR/*Packet_func Erro, Warning e etc*/) {
			_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#ifdef _DEBUG
			//_smp::message_pool::getInstance().push(new message("size packet: " + std::to_string(_pd._packet->getSize()) + "\n" + hex_util::BufferToHexString(_pd._packet->getBuffer(), _pd._packet->getSize()), CL_ONLY_FILE_LOG));
#endif
		}else
			throw;
	}

	try {

		_session.usa();

		ParamDispatchAS pd = { *(UnitPlayer*)&_session, _packet };

		if (func != nullptr && func->execCmd(&pd) != 0)
			_smp::message_pool::getInstance().push(new message("[unit_connect_base::dispach_packet_sv_same_thread][Error] ao tratar o pacote. ID: " + std::to_string(_packet->getTipo()) + "(0x" + hex_util::ltoaToHex(_packet->getTipo()) + ").", CL_FILE_LOG_AND_CONSOLE));
	
		if (_session.devolve())
			DisconnectSession(&_session);

	}catch (exception& e) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#endif

		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*não pode usa session*/))
			if (_session.devolve())
				DisconnectSession(&_session);

	}catch (std::exception& e) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message(e.what(), CL_FILE_LOG_AND_CONSOLE));
#else
		UNREFERENCED_PARAMETER(e);
#endif

		if (_session.devolve())
			DisconnectSession(&_session);

	}catch (...) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("Unknown Error unit_connect_base::dispach_packet_sv_same_thread()", CL_FILE_LOG_AND_CONSOLE));
#endif

		if (_session.devolve())
			DisconnectSession(&_session);
	}
};

void unit_connect_base::translate_operation(session* _session, DWORD dwIOsize, Buffer* lpBuffer, DWORD _operation) {

	switch (_operation) {
	case STDA_OT_SEND_RAW_REQUEST:
	case STDA_OT_SEND_REQUEST:
		send_new(_session, lpBuffer, _operation + 1);
		break;
	case STDA_OT_RECV_REQUEST:
		recv_new(_session, lpBuffer, _operation + 1);
		break;
	case STDA_OT_SEND_COMPLETED:
	case STDA_OT_SEND_RAW_COMPLETED:
	case STDA_OT_RECV_COMPLETED:
		translate_packet(_session, lpBuffer, dwIOsize, _operation);
		break;
	case STDA_OT_DISPACH_PACKET_CLIENT:
		dispach_packet_same_thread(*_session, (packet*)lpBuffer);

		if (lpBuffer != nullptr)
			delete (packet*)lpBuffer;
		break;
	case STDA_OT_DISPACH_PACKET_SERVER:
		dispach_packet_sv_same_thread(*_session, (packet*)lpBuffer);

		if (lpBuffer != nullptr)
			delete (packet*)lpBuffer;
		break;
	}
}

void unit_connect_base::send_new(session* _session, Buffer* lpBuffer, DWORD operation) {

	DWORD dwFlag = 0;

	try {

		_session->usa();

		_session->setSend();

		// Seta operação
		lpBuffer->setOperation(operation);

		if (_session->devolve()) {

			_session->releaseSend();

			if (_session->isCreated())
				// Send 0 Bytes, Session desconnected
				postIoOperation(_session, lpBuffer, 0, STDA_OT_RECV_COMPLETED);

			return;
		}

	}catch (exception& e) {
		
		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*Error ao usar session*/)) {
			
			if (_session->devolve()) {

				if (_session->isCreated())
					// Send 0 Bytes, Session desconnected
					postIoOperation(_session, lpBuffer, 0, STDA_OT_RECV_COMPLETED);

				return;
			}
		}

		if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 2))
			throw;

		return;
	}

#if defined(_WIN32)
	if (SOCKET_ERROR == WSASend(_session->m_sock, lpBuffer->getWSABufToSend(), 1, NULL, dwFlag, (LPOVERLAPPED)lpBuffer, NULL)) {
		int error = WSAGetLastError();

		if (WSA_IO_PENDING != error) {
			_session->lock();

			//_session->m_sock = INVALID_SOCKET;
			//_session->setState(false);
			_session->setConnectedToSend(false);

			_session->unlock();

			try {
				_session->releaseSend();
			}catch (exception& e) {

				_smp::message_pool::getInstance().push(new message("[unit_connect_base::send_new][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			}

			if (_session->isCreated())
				// Send 0 Bytes, Session desconnected
				postIoOperation(_session, lpBuffer, 0, STDA_OT_RECV_COMPLETED);
			
			if (error != WSAESHUTDOWN && error != WSAECONNRESET)
				throw exception("Erro no WSASend() -> unit_connect_base::send_new()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 1, error));
		}
	}
#elif defined(__linux__)
	// Verifica se tem algo para enviar
	try {

		if ((int)lpBuffer->getWSABufToSend()->len <= 0) {

			_session->usa();

			// release Send to not block send(deadlock)
			_session->releaseSend();

			if (_session->devolve()) {

				if (_session->isCreated())
					// Send 0 Bytes, Session desconnected
					postIoOperation(_session, lpBuffer, 0, STDA_OT_RECV_COMPLETED);
			}

			return;
		}
		
	} catch(exception& e) {

		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*Error ao usar session*/)) {
			
			if (_session->devolve()) {

				if (_session->isCreated())
					// Send 0 Bytes, Session desconnected
					postIoOperation(_session, lpBuffer, 0, STDA_OT_RECV_COMPLETED);

				return;
			}
		}
	}

	int sendlen = 0;
	if ((sendlen = ::send(_session->m_sock.fd, lpBuffer->getWSABufToSend()->buf, lpBuffer->getWSABufToSend()->len, 0)) == SOCKET_ERROR) {
		
		if (errno != EWOULDBLOCK && errno != EAGAIN) {

			_session->lock();

			//_session->m_sock = INVALID_SOCKET;
			//_session->setState(false);
			_session->setConnectedToSend(false);

			_session->unlock();

			try {
				_session->releaseSend();
			}catch (exception& e) {

				_smp::message_pool::getInstance().push(new message("[unit_connect_base::send_new][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			}

			if (_session->isCreated())
				// Send 0 Bytes, Session desconnected
				postIoOperation(_session, lpBuffer, 0, STDA_OT_RECV_COMPLETED);
			
			if (errno != ECONNRESET && errno != EPIPE/*socket invalid*/)
				throw exception("Erro no WSASend() -> unit_connect_base::send_new()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 1, errno));
			
		}else {

			// Not have space in socket tcp stack, release send for send again to late
			try {
				_session->setSendPartial();
			}catch (exception& e) {

				_smp::message_pool::getInstance().push(new message("[unit_connect_base::send_new][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			}
		}

	}else {

		// Log
		pthread_mutex_lock(&m_cs);

		// update
		m_transfer_bytes_io += sendlen;

		if (((std::clock() - m_1sec) / (double)CLOCKS_PER_SEC) > 1.0f) {
			_smp::message_pool::getInstance().push_back(new message("IO Transfer Bytes for second: " + std::to_string(m_transfer_bytes_io), CL_ONLY_FILE_LOG_IO_DATA));
			m_transfer_bytes_io = 0ull;
			m_1sec = std::clock();
		}

		pthread_mutex_unlock(&m_cs);

		// send partial
		if((uint32_t)sendlen != lpBuffer->getWSABufToSend()->len) {

			_smp::message_pool::getInstance().push(new message("[unit_connect_base::send_new][WARNING] Player[UID=" + std::to_string(_session->getUID()) 
					+ "] enviou dados partial[SENDLEN=" + std::to_string(sendlen) + "].", CL_FILE_LOG_AND_CONSOLE));

			_session->setSendPartial();
		}

		// post to translate
		postIoOperation(_session, lpBuffer, sendlen, operation);
	}
#endif
}

void unit_connect_base::recv_new(session* _session, Buffer* lpBuffer, DWORD operation) {

	DWORD dwFlag = 0;

	lpBuffer->setOperation(operation);

	if (!_session->isConnected())
		return;

#if defined(_WIN32)
	if (SOCKET_ERROR == WSARecv(_session->m_sock, lpBuffer->getWSABufToRead(), 1, NULL, &dwFlag, (LPOVERLAPPED)lpBuffer, NULL)) {
		int error = WSAGetLastError();
		
		if (WSA_IO_PENDING != error) {
			
			if (_session->isCreated())
				// Send 0 Bytes, Session desconnected
				postIoOperation(_session, lpBuffer, 0, operation);

			if (error != WSAESHUTDOWN && error != WSAECONNRESET)
				throw exception("Erro no WSARecv() -> unit_connect_base::recv_new()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 2, error));
		}
	}
#elif defined(__linux__)
	int recvlen = 0;
	if ((recvlen = ::recv(_session->m_sock.fd, lpBuffer->getWSABufToRead()->buf, lpBuffer->getWSABufToRead()->len, 0)) == SOCKET_ERROR) {

		if (errno != EWOULDBLOCK && errno != EAGAIN) {
			
			if (_session->isCreated())
				// Send 0 Bytes, Session desconnected
				postIoOperation(_session, lpBuffer, 0, STDA_OT_RECV_COMPLETED/*operation*/);

			if (errno != ECONNRESET)
				throw exception("Erro no WSARecv() -> unit_connect_base::recv_new()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 2, errno));
			
		}else
			_session->releaseRecv();

	}else {

		// Log
		pthread_mutex_lock(&m_cs);

		// update
		m_transfer_bytes_io += recvlen;

		if (((std::clock() - m_1sec) / (double)CLOCKS_PER_SEC) > 1.0f) {
			_smp::message_pool::getInstance().push_back(new message("IO Transfer Bytes for second: " + std::to_string(m_transfer_bytes_io), CL_ONLY_FILE_LOG_IO_DATA));
			m_transfer_bytes_io = 0ull;
			m_1sec = std::clock();
		}

		pthread_mutex_unlock(&m_cs);

		// post to translate
		postIoOperation(_session, lpBuffer, (DWORD)recvlen, STDA_OT_RECV_COMPLETED);
	}
#endif
}

void unit_connect_base::translate_packet(session* _session, Buffer* lpBuffer, DWORD dwIOsize, DWORD operation) {
	switch (operation) {
		case STDA_OT_SEND_RAW_COMPLETED:
			if (dwIOsize > 0 && lpBuffer != nullptr) {
		        lpBuffer->consume(dwIOsize);

				// Libera o send
				_session->releaseSend();
			}else {
				// Libera o send
				_session->releaseSend();

				try {
					
					// getConnectTime pode lançar exception
					if (_session->getConnectTime() <= 0 && _session->getState()) {

						_smp::message_pool::getInstance().push(new message("[unit_connect_base::translate_packet][Error] [STDA_OT_SEND_RAW_COMPLETED] _session[OID=" + std::to_string(_session->m_oid) + "] is not connected.", CL_FILE_LOG_AND_CONSOLE));

						// Ainda não habilitar o disconnect session, vms aguardar as mensagens para ver se vai ter
						//DisconnectSession(_session);
					}
				
				}catch (exception& e) {

					_smp::message_pool::getInstance().push(new message("[unit_connect_base::translate_packet][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}
			}

			break;
		case STDA_OT_SEND_COMPLETED:
			if (dwIOsize > 0 && lpBuffer != nullptr) {
				packet_head ph;
				packet *_packet = nullptr;
				int32_t left = 0;
				
				do {
					try {
						LOOP_TRANSLATE_BUFFER_TO_PACKET_CLIENT_UNITC(ph, (uint32_t)sizeof(ph), STDA_OT_DISPACH_PACKET_SERVER);
					}catch (exception& e) {
						_smp::message_pool::getInstance().push(new message("SEND_COMPLETED MY class exception {SESSION[IP=" + std::string(_session->getIP()) + "]}: " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

						if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::CRYPT || STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::COMPRESS) {

							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

							CLEAR_PACKET_LOOP_WITH_MSG;
						}else if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PACKET, 15) ||
								STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PACKET, 1))  {

							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

							CLEAR_PACKET_LOOP_SIMPLE;

							CLEAR_BUFFER_LOOP_SERVER;

							// Libera o send
							_session->releaseSend();

							break;
						}else {

							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

							CLEAR_PACKET_LOOP_SIMPLE;

							CLEAR_BUFFER_LOOP_SERVER;

							// Libera o send
							_session->releaseSend();

							break;
						}
					}catch (std::exception& e) {
						_smp::message_pool::getInstance().push(new message("SEND_COMPLETED std::class exception {SESSION[IP=" + std::string(_session->getIP()) + "]}: " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));

						CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

						CLEAR_PACKET_LOOP_SIMPLE;

						CLEAR_BUFFER_LOOP_SERVER;

						// Libera o send
						_session->releaseSend();

						break;
					}catch (...) {
						_smp::message_pool::getInstance().push(new message("SEND_COMPLETED Exception desconhecida. {SESSION[IP=" + std::string(_session->getIP()) + "]}", CL_FILE_LOG_AND_CONSOLE));

						CLEAR_PACKET_LOOP_SESSION(GET_FUNC_SEND, SET_FUNC_SEND);

						CLEAR_PACKET_LOOP_SIMPLE;

						CLEAR_BUFFER_LOOP_SERVER;

						// Libera o send
						_session->releaseSend();

						break;
					}
				} while (dwIOsize > 0);

				// Libera o send
				_session->releaseSend();
			}else {

				// Libera o send
				_session->releaseSend();

				try {
					
					// getConnectTime pode lançar exception
					if (_session->getConnectTime() <= 0 && _session->getState()) {

						_smp::message_pool::getInstance().push(new message("[unit_connect_base::translate_packet][Error] [STDA_OT_SEND_COMPLETED] _session[OID=" + std::to_string(_session->m_oid) + "] is not connected.", CL_FILE_LOG_AND_CONSOLE));

						// Ainda não habilitar o disconnect session, vms aguardar as mensagens para ver se vai ter
						//DisconnectSession(_session);
					}
				
				}catch (exception& e) {

					_smp::message_pool::getInstance().push(new message("[unit_connect_base::translate_packet][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}
			}

			break;
		case STDA_OT_RECV_COMPLETED:
			if (dwIOsize > 0 && lpBuffer != nullptr) {

				packet_head ph;
				packet *_packet = nullptr;
				int32_t left = 0;
				
				lpBuffer->addSize(dwIOsize);	// Add o size que recebeu no socket

				do {
					try {
						LOOP_TRANSLATE_BUFFER_TO_PACKET_SERVER_UNITC(ph, (uint32_t)sizeof(ph), STDA_OT_DISPACH_PACKET_CLIENT);
					}catch (exception& e) {
						_smp::message_pool::getInstance().push(new message("RECV_COMPLETED MY class exception {SESSION[IP=" + std::string(_session->getIP()) + "]}: " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

						if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::CRYPT || STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::COMPRESS) {
							
							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_RECV, SET_FUNC_RECV);

							CLEAR_PACKET_LOOP_WITH_MSG;
						}else if (STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PACKET, 15) ||
							STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::PACKET, 1)) {

							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_RECV, SET_FUNC_RECV);

							CLEAR_PACKET_LOOP_SIMPLE;

							CLEAR_BUFFER_LOOP_CLIENT;

							break;
						}else {
							
							CLEAR_PACKET_LOOP_SESSION(GET_FUNC_RECV, SET_FUNC_RECV);

							CLEAR_PACKET_LOOP_SIMPLE;

							CLEAR_BUFFER_LOOP_CLIENT;

							break;
						}
					}catch (std::exception& e) {
						_smp::message_pool::getInstance().push(new message("RECV_COMPLETED std::class exception {SESSION[IP=" + std::string(_session->getIP()) + "]}: " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));

						CLEAR_PACKET_LOOP_SESSION(GET_FUNC_RECV, SET_FUNC_RECV);

						CLEAR_PACKET_LOOP_SIMPLE;

						CLEAR_BUFFER_LOOP_CLIENT;

						break;
					}catch (...) {
						_smp::message_pool::getInstance().push(new message("RECV_COMPLETED Exception desconhecida {SESSION[IP=" + std::string(_session->getIP()) + "]}.", CL_FILE_LOG_AND_CONSOLE));

						CLEAR_PACKET_LOOP_SESSION(GET_FUNC_RECV, SET_FUNC_RECV);

						CLEAR_PACKET_LOOP_SIMPLE;

						CLEAR_BUFFER_LOOP_CLIENT;

						break;
					}
				} while (dwIOsize > 0);

#if defined(_WIN32)
				// Post new IO Operation - RECV REQUEST
				memset(lpBuffer, 0, sizeof(OVERLAPPED));	// Zera o over pra receber mais
#endif

				//_session->requestRecvBuffer();
				_session->releaseRecv();
				//postIoOperation(_session, lpBuffer, operation - 1);
			}else {
				if (_session != nullptr /*&& _session->m_sock != INVALID_SOCKET*/)
					DisconnectSession(_session);
			}

			break;
	}
}

void unit_connect_base::postIoOperationL(session* _session, Buffer* lpBuffer, DWORD operation) {

	myOver *tmp = new myOver;

	tmp->clear();
	tmp->tipo = operation;
	tmp->buffer = lpBuffer;

#if defined(_WIN32)
	m_iocp_logical.postStatus((ULONG_PTR)_session, 0, (LPOVERLAPPED)tmp);
#elif defined(__linux__)
	m_iocp_logical.push(new stThreadpoolMessage(_session, (Buffer*)tmp, 0));
#endif
}

void unit_connect_base::ConnectAndAssoc() {

	if (!m_unit_ctx.state)
		throw exception("[unit_connect_base::ConnectAndAssoc][Error] a config do unit_connect nao foi carregado com sucesso.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_CONNECT, 10, 0));
	
#if defined(_WIN32)
	SOCKET _sock = INVALID_SOCKET;
	socket *_socket = nullptr;
	BOOL tcp_nodelay = 1;

	if ((_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
		throw exception("[unit_connect_base::ConnectAndAssoc][Error] nao conseguiu criar o socket.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_CONNECT, 2, WSAGetLastError()));

	if (setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&tcp_nodelay, sizeof(tcp_nodelay)) == SOCKET_ERROR)
		_smp::message_pool::getInstance().push(new message("[unit_connect_base::ConnectAndAssoc][WARNING] nao conseguiu desabilitar tcp delay(nagle algorithm).", CL_ONLY_CONSOLE));

	m_session.m_sock = _sock;

	std::srand(std::clock());

	m_iocp_io.associaDeviceToPort((ULONG_PTR)&m_session, (HANDLE)_sock);

	_socket = new socket(_sock);
#elif defined(__linux__)
	SOCKET _sock = { INVALID_SOCKET, {0u} };
	socket *_socket = nullptr;
	BOOL tcp_nodelay = 1;

	if ((_sock.fd = ::socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		throw exception("[unit_connect_base::ConnectAndAssoc][Error] nao conseguiu criar o socket.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::UNIT_CONNECT, 2, errno));

	int flag = O_NONBLOCK;
	if (fcntl(_sock.fd, F_SETFL, flag) != 0)
		throw exception("[unit_connect_base::ConnectAndAssoc][Error] nao conseguiu habilitar o NONBLOCK(fcntl).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SERVER, 53, errno));

	if (setsockopt(_sock.fd, IPPROTO_TCP, TCP_NODELAY, (char*)&tcp_nodelay, sizeof(tcp_nodelay)) == SOCKET_ERROR)
		_smp::message_pool::getInstance().push(new message("[unit_connect_base::ConnectAndAssoc][WARNING] nao conseguiu desabilitar tcp delay(nagle algorithm).", CL_ONLY_CONSOLE));

	// New socket creation time
	clock_gettime(SESSION_CONNECT_TIME_CLOCK, &_sock.connect_time);

	m_session.m_sock = _sock;

	std::srand(std::clock());

	epoll_event ep_ev;

	//ep_ev.data.fd = _sock.fd;
	ep_ev.data.ptr = &m_session;

	ep_ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP;

	m_iocp_io.associaDeviceToPort(_sock.fd, ep_ev);

	_socket = new socket(_sock);
#endif
	
	try {

		_socket->connect(m_unit_ctx.ip, m_unit_ctx.port);

		m_session.lock();

		m_session.setState(true);
		m_session.setConnected(true);

		m_session.unlock();

		m_session.requestRecvBuffer();

	}catch (std::exception& e) {
		UNREFERENCED_PARAMETER(e);

#if defined(__linux__)
		// delete socket fd from epoll
		m_iocp_io.deleteFD(m_session.m_sock.fd, ep_ev);
#endif

		m_session.lock();

		m_session.clear();

		m_session.unlock();

		// Clean
		if (_socket != nullptr) {

			delete _socket;

			_socket = nullptr;
		}

		// Relançaa
		throw;
	}

	_socket->detatch();

	// Clean
	if (_socket != nullptr) {

		delete _socket;

		_socket = nullptr;
	}

	// on Connected
	onConnected();
}

// Class player
UnitPlayer::UnitPlayer(threadpool_base& _threadpool, ServerInfoEx& _si) : session(_threadpool), m_si(_si) {

};

UnitPlayer::~UnitPlayer() {
};

bool UnitPlayer::clear() {

	if (session::clear())
		m_pi.clear();
	
	return true;
};

unsigned char UnitPlayer::getStateLogged() {
	return m_pi.m_state;
};

uint32_t UnitPlayer::getUID() {
	return m_pi.uid;
};

uint32_t UnitPlayer::getCapability() {
	return m_pi.tipo;
};

char* UnitPlayer::getNickname() {
	return m_pi.nickname;
};

char* UnitPlayer::getID() {
	return m_pi.id;
};

// Class packet_func_as
void unit_connect_base::packet_func_as::session_send(packet& p, UnitPlayer *s, unsigned char _debug) {
	
	if (s == nullptr)
		throw exception("Error session *s is nullptr, packet_func::session_send()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_LS, 1, 2));

	MAKE_SEND_BUFFER(p, s);

	SHOW_DEBUG_PACKET(p, _debug);

	SHOW_DEBUG_FINAL_PACKET(mb, _debug);

	//delete p;
};

void unit_connect_base::packet_func_as::session_send(std::vector< packet* > v_p, UnitPlayer *s, unsigned char _debug) {
	
	if (s == nullptr)
		throw exception("Error session *s is nullptr, packet_func::session_send()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PACKET_FUNC_LS, 1, 2));

	for (auto i = 0u; i < v_p.size(); ++i) {
		if (v_p[i] != nullptr) {
			MAKE_SEND_BUFFER(*v_p[i], s);

			SHOW_DEBUG_PACKET(*v_p[i], _debug);

			SHOW_DEBUG_FINAL_PACKET(mb, _debug);

			delete v_p[i];
		}else
			_smp::message_pool::getInstance().push(new message("Error packet *p is nullptr, packet_func::session_send()", CL_FILE_LOG_AND_CONSOLE));
	}

	v_p.clear();
	v_p.shrink_to_fit();
};
