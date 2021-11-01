// Arquivo threadpool.cpp
// Criado em 21/05/2017 por Acrisio
// Implementação da classe threadpool

#if defined(_WIN32)
#include <WinSock2.h>
#include <MSWSock.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#endif

#include "threadpool.h"
#include "../SOCKET/socket.h"
#include "../TYPE/stdAType.h"
#include "../UTIL/exception.h"
#include "../UTIL/message_pool.h"
#include "../SOCKET/session.h"
#include "../UTIL/hex_util.h"
#include <ctime>
#include "../TYPE/stda_error.h"

#include <iostream>
#include <fstream>

#if defined(_WIN32)
#include <DbgHelp.h>
#elif defined(__linux__)
	// !@ ainda não pequisei, para ver se tem o Debug dump exception para linux
#endif

using namespace stdA;

threadpool::threadpool(size_t _num_thread_workers_io, size_t _num_thread_workers_logical, uint32_t _job_thread_num) 
		: m_iocp_io_accept(0), 
#if defined(_WIN32)
		m_iocp_io{ iocp(0) }, 
		m_iocp_io_send{ iocp(0) }, 
		m_iocp_io_recv{ iocp(0) }, 
		m_iocp_logical(0) /*{ iocp(0) }*/, 
		m_iocp_send(0)/*{ iocp(0) }*/ 
#elif defined(__linux__)
		m_iocp_io{ iocp(0), iocp(0), iocp(0), iocp(0), iocp(0), iocp(0), iocp(0), iocp(0), iocp(0), iocp(0), iocp(0), iocp(0), iocp(0), iocp(0), iocp(0), iocp(0) }, 
		m_iocp_io_send()/*{ iocp(0) }*/, 
		m_iocp_io_recv()/*{ iocp(0) }*/,
		m_iocp_logical() /*{ iocp(0) }*/, 
		m_iocp_send()/*{ iocp(0) }*/ 
#endif
		{
	
	uint32_t i = 0;

	m_transfer_bytes_io = 0ull;
	m_1sec = 0;

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

	// Console Log Thread
	m_thread_console = new thread(TT_CONSOLE, threadpool::_console, (LPVOID)this, THREAD_PRIORITY_NORMAL/*THREAD_PRIORITY_BELOW_NORMAL*/);

	// Job(s)
	for (i = 0; i < _job_thread_num; i++)
		m_threads.push_back(new thread(TT_JOB, threadpool::__job, (LPVOID)this));

	// Workers IO Threads
	for (i = 0; i < _num_thread_workers_io; i++) {
		m_threads.push_back(new thread(TT_WORKER_IO, threadpool::_worker_io, (LPVOID)new threadpool_and_index{ this, (uint32_t)i }, THREAD_PRIORITY_ABOVE_NORMAL));
		//m_threads.push_back(new thread(TT_WORKER_IO, threadpool::_worker_io, (LPVOID)new threadpool_and_index{ this, i }, THREAD_PRIORITY_ABOVE_NORMAL));
	}

	// Workers IO Send Threads
	for (i = 0; i < _num_thread_workers_io; i++) {
		m_threads.push_back(new thread(TT_WORKER_IO_SEND, threadpool::_worker_io_send, (LPVOID)new threadpool_and_index{ this, (uint32_t)i }, THREAD_PRIORITY_ABOVE_NORMAL));
		//m_threads.push_back(new thread(TT_WORKER_IO_SEND, threadpool::_worker_io_send, (LPVOID)new threadpool_and_index{ this, i }, THREAD_PRIORITY_ABOVE_NORMAL));
	}

	// Workers IO Recv Threads
	for (i = 0; i < _num_thread_workers_io; i++) {
		m_threads.push_back(new thread(TT_WORKER_IO_RECV, threadpool::_worker_io_recv, (LPVOID)new threadpool_and_index{ this, (uint32_t)i }, THREAD_PRIORITY_ABOVE_NORMAL));
		//m_threads.push_back(new thread(TT_WORKER_IO_RECV, threadpool::_worker_io_recv, (LPVOID)new threadpool_and_index{ this, i }, THREAD_PRIORITY_ABOVE_NORMAL));
		//m_threads.push_back(new thread(TT_WORKER_IO_RECV, threadpool::_worker_io_recv, (LPVOID)new threadpool_and_index{ this, i }, THREAD_PRIORITY_ABOVE_NORMAL));
	}

	// Workers Logical Threads
	for (i = 0; i < _num_thread_workers_logical; i++) {
		m_threads.push_back(new thread(TT_WORKER_LOGICAL, threadpool::_worker_logical, (LPVOID)new threadpool_and_index{ this, (uint32_t)i }, THREAD_PRIORITY_ABOVE_NORMAL));
		//m_threads.push_back(new thread(TT_WORKER_LOGICAL, threadpool::_worker_logical, (LPVOID)new threadpool_and_index{ this, i }, THREAD_PRIORITY_ABOVE_NORMAL));
		//m_threads.push_back(new thread(TT_WORKER_LOGICAL, threadpool::_worker_logical, (LPVOID)new threadpool_and_index{ this, i }, THREAD_PRIORITY_ABOVE_NORMAL));
	}

	// Workers Send Threads
	for (i = 0; i < _num_thread_workers_logical; i++) {
		m_threads.push_back(new thread(TT_WORKER_SEND, threadpool::_worker_send, (LPVOID)new threadpool_and_index{ this, (uint32_t)i }, THREAD_PRIORITY_ABOVE_NORMAL));	// Estava ABOVE
		//m_threads.push_back(new thread(TT_WORKER_SEND, threadpool::_worker_send, (LPVOID)new threadpool_and_index{ this, i }, THREAD_PRIORITY_ABOVE_NORMAL));	// Estava ABOVE
	}
};

threadpool::~threadpool() {

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

	if (m_thread_console != nullptr)
		delete m_thread_console;

	m_thread_console = nullptr;

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
};

#if defined(_WIN32)
DWORD threadpool::_worker_io(LPVOID lpParameter) 
#elif defined(__linux__)
void* threadpool::_worker_io(LPVOID lpParameter) 
#endif
{
	BEGIN_THREAD_SETUP(threadpool_and_index)

	result = pTP->m_threadpool->worker_io(pTP->m_index);

	delete pTP;	// Libera memória allocada para threadpool_and_index

	END_THREAD_SETUP("worker_io()")
};

#if defined(_WIN32)
DWORD threadpool::_worker_io_accept(LPVOID lpParameter) 
#elif defined(__linux__)
void* threadpool::_worker_io_accept(LPVOID lpParameter) 
#endif
{
	BEGIN_THREAD_SETUP(threadpool);

	result = pTP->worker_io_accept();

	END_THREAD_SETUP("worker_io_accept()");
}

#if defined(_WIN32)
DWORD threadpool::_worker_io_send(LPVOID lpParameter) 
#elif defined(__linux__)
void* threadpool::_worker_io_send(LPVOID lpParameter) 
#endif
{
	BEGIN_THREAD_SETUP(threadpool_and_index);

	result = pTP->m_threadpool->worker_io_send(pTP->m_index);

	delete pTP;	// Libera memória allocada para threadpool_and_index

	END_THREAD_SETUP("worker_io_send()");
};

#if defined(_WIN32)
DWORD threadpool::_worker_io_recv(LPVOID lpParameter) 
#elif defined(__linux__)
void* threadpool::_worker_io_recv(LPVOID lpParameter) 
#endif
{
	BEGIN_THREAD_SETUP(threadpool_and_index);

	result = pTP->m_threadpool->worker_io_recv(pTP->m_index);

	delete pTP;	// Libera memória allocada para threadpool_and_index

	END_THREAD_SETUP("worker_io_recv()");
};

#if defined(_WIN32)
DWORD threadpool::_worker_logical(LPVOID lpParameter) 
#elif defined(__linux__)
void* threadpool::_worker_logical(LPVOID lpParameter) 
#endif
{
	BEGIN_THREAD_SETUP(threadpool_and_index)

	result = pTP->m_threadpool->worker_logical(pTP->m_index);

	delete pTP;	// Libera memória allocada para threadpool_and_index

	END_THREAD_SETUP("worker_logical()")
};

#if defined(_WIN32)
DWORD threadpool::_worker_send(LPVOID lpParameter) 
#elif defined(__linux__)
void* threadpool::_worker_send(LPVOID lpParameter) 
#endif
{
	BEGIN_THREAD_SETUP(threadpool_and_index)

	result = pTP->m_threadpool->worker_send(pTP->m_index);

	delete pTP;	// Libera memória allocada para threadpool_and_index

	END_THREAD_SETUP("worker_send()")
};

#if defined(_WIN32)
DWORD threadpool::_console(LPVOID lpParameter) 
#elif defined(__linux__)
void* threadpool::_console(LPVOID lpParameter) 
#endif
{
	BEGIN_THREAD_SETUP(threadpool)

	result = pTP->console();

	END_THREAD_SETUP("console()")
};

#if defined(_WIN32)
DWORD threadpool::__job(LPVOID lpParameter) 
#elif defined(__linux__)
void* threadpool::__job(LPVOID lpParameter) 
#endif
{
	BEGIN_THREAD_SETUP(threadpool)

	result = pTP->_job();

	END_THREAD_SETUP("job()")
};

#if defined(_WIN32)
DWORD threadpool::worker_io(DWORD _index)
#elif defined(__linux__)
void* threadpool::worker_io(DWORD _index) 
#endif 
{
	
	try {

		_smp::message_pool::getInstance().push(new message("Ola console, here is thread worker IO : " + std::to_string(
#if defined(_WIN32)
		GetCurrentThreadId()
#elif defined(__linux__)
		getpid()
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

					m_iocp_io[_index].getStatus((PULONG_PTR)&_session, &dwIOsize, (LPOVERLAPPED*)&lpBuffer);

				}catch (exception& e) {

					if (STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_SEM_TIMEOUT) {
						DWORD flags_over = 0u;
						DWORD last_err = 0u;

						if (!WSAGetOverlappedResult(_session->m_sock, (LPOVERLAPPED)lpBuffer, &dwIOsize, FALSE, &flags_over)
								&& (last_err = WSAGetLastError()) != WSA_IO_INCOMPLETE && last_err != WSAETIMEDOUT)
							throw exception("[threadpool::worker_io][Error] exception ERROR_SEM_TIMEOUT dwIOsize=" + std::to_string(dwIOsize) + ", Flags_Over="
									+ std::to_string(flags_over) + " que retornou da operacao no iocp. session[IP=" + _session->getIP() + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 255, last_err));

						// Marca na session que o socket, levou DC, chegou ao limit de retramission do TCP para transmitir os dados
						// TCP sockets is that the maximum retransmission count and timeout have been reached on a bad(or broken) link
						_session->m_connection_timeout = true;

						// Se passar tenho que colocar o dwIOsize para zero para ele ser disconnectado
						_smp::message_pool::getInstance().push(new message("[threadpool::worker_io][WARNING] exception ERROR_SEM_TIMEOUT dwIOsize=" + std::to_string(dwIOsize) + ", Flags_Over="
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

				if (operation == STDA_OT_SEND_COMPLETED)
					postIoOperationS(_session, lpBuffer, dwIOsize, operation);
				else
					postIoOperationR(_session, lpBuffer, dwIOsize, operation);
					//translate_operation(_session, dwIOsize, lpBuffer, operation);
#elif defined(__linux__)
				// Clear
				_session = nullptr;
				lpBuffer = nullptr;

				try {

					// Clear
					ep_ev = { 0u };

					ret_epoll_wait = m_iocp_io[_index].getStatus(&ep_ev, 1, INFINITE);

				}catch (exception& e) {

					// check erro de interrupição do epoll_wait com sinal
					if (STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) != EINTR)
						throw;
					else
						_smp::message_pool::getInstance().push(new message("[threadpool::worker_io][ErrorSystem] epoll interrupted by signal. " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}

				// Shutdown thread
				if (m_iocp_io[_index].isExitEpollEvent(ep_ev))
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
						_smp::message_pool::getInstance().push(new message("[threadpool::worker_io][WARNING] exception ERROR_SEM_TIMEOUT no iocp[socket_error=" 
								+ std::to_string(err_socket) + "]. session[IP=" + _session->getIP() + "]", CL_FILE_LOG_AND_CONSOLE));

						// Zera para desconectar a session
						// e post no recv para ele desconnectar a session
						postIoOperationR(_session, lpBuffer, 0, STDA_OT_RECV_COMPLETED);

						// volta para o começo do while
						continue;
					
					}else	// Relança qualquer outro error
						throw exception("[threadpool::worker_io][Error] ep_ev events[EVENTS=" + std::to_string(ep_ev.events) 
							+ ", SESSION=" + std::to_string((uintptr_t)_session) + ", IP=" + _session->getIP() + "] err_socket: " + std::to_string(err_socket) 
							+ ", ret_epoll_wait: " + std::to_string(ret_epoll_wait), STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 5000, err_socket));
				}

				// Shutdown, post to close connection
				if ((ep_ev.events & EPOLLHUP) == EPOLLHUP) {
					
					// Disconnect session
					postIoOperationR(_session, lpBuffer, 0, STDA_OT_RECV_COMPLETED);

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

					// postIoOperation em vez de postIoOperationS, por que ele ainda vai chamar a função send so socket, 
					// aqui no IOCP já é depois de enviar para o outro peer
					postIoOperation(_session, lpBuffer, 0, STDA_OT_SEND_REQUEST);
				}
#endif

			}catch (exception& e) {
//#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[threadpool::worker_io][ErrorSystem][MY] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#else
	//			if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 2))
		//			_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#endif

				// Se for STDA_OT_SEND_COMPLETED reseta o send
				if (lpBuffer != nullptr && (lpBuffer->getOperation() == STDA_OT_SEND_COMPLETED || lpBuffer->getOperation() == STDA_OT_SEND_RAW_COMPLETED))
					_session->releaseSend();

				postIoOperationR(_session, lpBuffer, 0/*disconnect*/, STDA_OT_RECV_COMPLETED/* Desconecta a session*/);

			}catch (std::exception& e) {
				
				_smp::message_pool::getInstance().push(new message(std::string("[threadpool::worker_io][ErrorSystem][STD] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));

				// Se for STDA_OT_SEND_COMPLETED reseta o send
				if (lpBuffer != nullptr && (lpBuffer->getOperation() == STDA_OT_SEND_COMPLETED || lpBuffer->getOperation() == STDA_OT_SEND_RAW_COMPLETED))
					_session->releaseSend();

				postIoOperationR(_session, lpBuffer, 0/*disconnect*/, STDA_OT_RECV_COMPLETED/* Desconecta a session*/);

			}catch (...) {

				std::cout << "worker_io() ->while Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";

				// Se for STDA_OT_SEND_COMPLETED reseta o send
				if (lpBuffer != nullptr && (lpBuffer->getOperation() == STDA_OT_SEND_COMPLETED || lpBuffer->getOperation() == STDA_OT_SEND_RAW_COMPLETED))
					_session->releaseSend();

				postIoOperationR(_session, lpBuffer, 0/*disconnect*/, STDA_OT_RECV_COMPLETED/* Desconecta a session*/);
			}
		}
	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[threadpool::worker_io][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "worker_io() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}

	_smp::message_pool::getInstance().push(new message("Saindo de worker_io()..."));
	
	return 0u;
};

#if defined(_WIN32)
DWORD threadpool::worker_io_accept() 
#elif defined(__linux__)
void* threadpool::worker_io_accept() 
#endif
{

	try {
		_smp::message_pool::getInstance().push(new message("Ola console, here is thread worker IO Accept: " + std::to_string(
#if defined(_WIN32)
			GetCurrentThreadId()
#elif defined(__linux__)
			gettid()
#endif
			) + "."));
		myOver *lpBuffer = nullptr;
#if defined(_WIN32)
		SOCKET listener = INVALID_SOCKET;
		DWORD dwIOsize = 0, operation = ~0;
#elif defined(__linux__)
		SOCKET listener;
		
		listener.fd = INVALID_SOCKET;
		listener.connect_time.tv_sec = 0;
		listener.connect_time.tv_nsec = 0;
		
		int32_t ret_accept = 0;
		int32_t ret_epoll_wait = 0;

		epoll_event ep_ev{ 0u };
#endif

		while (1) {
			try {
#if defined(_WIN32)
				try {
					operation = ~0;
					lpBuffer = nullptr;

					m_iocp_io_accept.getStatus((PULONG_PTR)&listener, &dwIOsize, (LPOVERLAPPED*)&lpBuffer);

				}catch (exception& e) {

					if (STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_SEM_TIMEOUT) {
						DWORD flags_over = 0u;
						DWORD last_err = 0u;

						if (!WSAGetOverlappedResult(listener, (LPOVERLAPPED)lpBuffer, &dwIOsize, FALSE, &flags_over)
								&& (last_err = WSAGetLastError()) != WSA_IO_INCOMPLETE && last_err != WSAETIMEDOUT)
							throw exception("[threadpool::worker_io_accept][Error] exception ERROR_SEM_TIMEOUT dwIOsize=" + std::to_string(dwIOsize) + ", Flags_Over="
									+ std::to_string(flags_over) + " que retornou da operacao no iocp. sock id=" + std::to_string((uint32_t)listener), STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 255, last_err));

						// Se passar tenho que colocar o dwIOsize para zero para ele ser disconnectado
						_smp::message_pool::getInstance().push(new message("[threadpool::worker_io_accept][WARNING] exception ERROR_SEM_TIMEOUT dwIOsize=" + std::to_string(dwIOsize) + ", Flags_Over="
								+ std::to_string(flags_over) + " que retornou da operacao no iocp[WSAError=" + std::to_string(last_err) + "]. sock id=" + std::to_string((uint32_t)listener), CL_FILE_LOG_AND_CONSOLE));

						// Aqui no accept não faz diferença que ele muda ali em baixo, por que é o accept
						dwIOsize = STDA_SYSTEM_ERROR_DECODE(e.getCodeError());

						// clear myOver Buffer
						if (lpBuffer != nullptr) {

							if (lpBuffer->buffer != nullptr)
								delete lpBuffer->buffer;

							delete lpBuffer;
						}

					}else if (!(STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_NETNAME_DELETED || STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_CONNECTION_ABORTED
						|| STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_OPERATION_ABORTED)) {

						// clear myOver Buffer
						if (lpBuffer != nullptr) {

							if (lpBuffer->buffer != nullptr)
								delete lpBuffer->buffer;

							delete lpBuffer;
						}

						throw;
					}

					// Trata no tranlate dele
					dwIOsize = STDA_SYSTEM_ERROR_DECODE(e.getCodeError());
				}

				if (listener == INVALID_SOCKET)
					break;

				if (lpBuffer != nullptr)
					operation = lpBuffer->tipo;

#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("Operation typeIOS: " + std::to_string(operation) + "\tlen: " + std::to_string(dwIOsize), CL_ONLY_FILE_LOG));
#endif

				translate_operation((session*)&listener, dwIOsize, (Buffer*)lpBuffer, operation);
#elif defined(__linux__)
				try {

					ep_ev = { 0u };

					ret_epoll_wait = m_iocp_io_accept.getStatus(&ep_ev, 1, INFINITE);

				}catch (exception& e) {

					// check erro de interrupição do epoll_wait com sinal
					if (STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) != EINTR)
						throw;
					else
						_smp::message_pool::getInstance().push(new message("[threadpool::worker_io][ErrorSystem] epoll interrupted by signal. " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				}

				// Shutdown thread
				if (m_iocp_io_accept.isExitEpollEvent(ep_ev))
					break;

				if (ret_epoll_wait == 0)
					continue;

				// error
				if ((ep_ev.events & EPOLLERR) == EPOLLERR) {

					int err_socket = 1234; // Padrão error
					socklen_t len_err_sock = sizeof(int);

					// Ignora retorno
					int rt_err = ::getsockopt(ep_ev.data.fd, SOL_SOCKET, SO_ERROR, &err_socket, &len_err_sock);

					throw exception("[threadpool::worker_io_accept][Error] ep_ev events[FD=" + std::to_string(ep_ev.data.fd) + "] err_socket: " + std::to_string(err_socket) 
						+ ", EVENTS=" + std::to_string(ep_ev.events) + ", ret_epoll_wait: " + std::to_string(ret_epoll_wait), STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 5000, err_socket));
				}

				// Shutdown, (mas esse socket está escutando e não conectado) acho que esse erro ele não dá
				if ((ep_ev.events & EPOLLHUP) == EPOLLHUP) {

					int err_socket = 1234; // Padrão error
					socklen_t len_err_sock = sizeof(int);

					// Ignora retorno
					int rt_err = ::getsockopt(ep_ev.data.fd, SOL_SOCKET, SO_ERROR, &err_socket, &len_err_sock);

					throw exception("[threadpool::worker_io_accept][Error] shutdown ep_ev events[FD=" + std::to_string(ep_ev.data.fd) + "] err_socket: " + std::to_string(err_socket)
						+ ", EVENTS=" + std::to_string(ep_ev.events) + ", ret_epoll_wait: " + std::to_string(ret_epoll_wait), STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 5001, err_socket));
				}

				// Socket listener fd
				listener.fd = ep_ev.data.fd;

				translate_operation((session*)&listener, 0, nullptr, STDA_OT_ACCEPT_COMPLETED);
#endif
			}catch (exception& e) {
//#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[threadpool::worker_io_accept][ErrorSystem][MY] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#else
	//			if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 2))
		//			_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#endif

				// Desconnect session
				translate_operation((session*)&listener, 1/*disconnect*/, (Buffer*)lpBuffer, STDA_OT_ACCEPT_COMPLETED);

				// clear myOver Buffer
				if (lpBuffer != nullptr) {

					if (lpBuffer->buffer != nullptr)
						delete lpBuffer->buffer;

					delete lpBuffer;
				}

			}catch (std::exception& e) {

				_smp::message_pool::getInstance().push(new message(std::string("[threadpool::worker_io_accept][ErrorSystem][STD ") + e.what(), CL_FILE_LOG_AND_CONSOLE));

				// Desconnect session
				translate_operation((session*)&listener, 1/*disconnect*/, (Buffer*)lpBuffer, STDA_OT_ACCEPT_COMPLETED);

				// clear myOver Buffer
				if (lpBuffer != nullptr) {

					if (lpBuffer->buffer != nullptr)
						delete lpBuffer->buffer;

					delete lpBuffer;
				}

			}catch (...) {
				
				std::cout << "worker_io_accept() ->while Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";

				// Desconnect session
				translate_operation((session*)&listener, 1/*disconnect*/, (Buffer*)lpBuffer, STDA_OT_ACCEPT_COMPLETED);

				// clear myOver Buffer
				if (lpBuffer != nullptr) {

					if (lpBuffer->buffer != nullptr)
						delete lpBuffer->buffer;

					delete lpBuffer;
				}
			}
		}
	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[threadpool::worker_io_accept][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "worker_io_accept() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}

	_smp::message_pool::getInstance().push(new message("Saindo de worker_io_accept()..."));

	return 0u;
};

#if defined(_WIN32)
DWORD threadpool::worker_io_send(DWORD _index) 
#elif defined(__linux__)
void* threadpool::worker_io_send(DWORD _index) 
#endif
{
	
	try {
		_smp::message_pool::getInstance().push(new message("Ola console, here is thread worker IO Send: " + std::to_string(
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
		stThreadpoolMessage *tpMessage = nullptr;
		DWORD operation = ~0;
#endif

		while (1) {
			try {
#if defined(_WIN32)
				try {
					operation = ~0;

					m_iocp_io_send[_index].getStatus((PULONG_PTR)&_session, &dwIOsize, (LPOVERLAPPED*)&lpBuffer);

				}catch (exception& e) {

					if (STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_SEM_TIMEOUT) {
						DWORD flags_over = 0u;
						DWORD last_err = 0u;

						if (!WSAGetOverlappedResult(_session->m_sock, (LPOVERLAPPED)lpBuffer, &dwIOsize, FALSE, &flags_over) 
								&& (last_err = WSAGetLastError()) != WSA_IO_INCOMPLETE && last_err != WSAETIMEDOUT)
							throw exception("[threadpool::worker_io_send][Error] exception ERROR_SEM_TIMEOUT dwIOsize=" + std::to_string(dwIOsize) + ", Flags_Over="
									+ std::to_string(flags_over) + " que retornou da operacao no iocp. session[IP=" + _session->getIP() + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 255, last_err));

						// Marca na session que o socket, levou DC, chegou ao limit de retramission do TCP para transmitir os dados
						// TCP sockets is that the maximum retransmission count and timeout have been reached on a bad(or broken) link
						_session->m_connection_timeout = true;

						// Se for STDA_OT_SEND_COMPLETED reseta o send
						if (lpBuffer != nullptr && (lpBuffer->getOperation() == STDA_OT_SEND_COMPLETED || lpBuffer->getOperation() == STDA_OT_SEND_RAW_COMPLETED))
							_session->releaseSend();

						// Se passar tenho que colocar o dwIOsize para zero para ele ser disconnectado
						_smp::message_pool::getInstance().push(new message("[threadpool::worker_io_send][WARNING] exception ERROR_SEM_TIMEOUT dwIOsize=" + std::to_string(dwIOsize) + ", Flags_Over=" 
								+ std::to_string(flags_over) + " que retornou da operacao no iocp[WSAError=" + std::to_string(last_err) + "]. session[IP=" + _session->getIP() + "]", CL_FILE_LOG_AND_CONSOLE));

						// Zera para desconectar a session
						dwIOsize = 0u;

					}else if (!(STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_NETNAME_DELETED || STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_CONNECTION_ABORTED
						|| STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_OPERATION_ABORTED))
						throw;
				}

				if (_session == nullptr)
					break;

				if (lpBuffer != nullptr)
					operation = lpBuffer->getOperation();

#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("Operation typeIOS: " + std::to_string(operation) + "\tlen: " + std::to_string(dwIOsize), CL_ONLY_FILE_LOG));
#endif

				translate_operation(_session, dwIOsize, lpBuffer, operation);
#elif defined(__linux__)
				try {
					operation = ~0;

					tpMessage = m_iocp_io_send[_index].get(INFINITE);

				}catch (exception& e) {

					// Diferente de timeout
					if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::LIST_FIFO, 2))
						throw;
				}

				if (tpMessage == nullptr)
					break;

				_session = tpMessage->_session;
				lpBuffer = tpMessage->buffer;

				if (lpBuffer != nullptr)
					operation = lpBuffer->getOperation();

#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("Operation typeIOS: " + std::to_string(operation) + "\tlen: " + std::to_string(tpMessage->dwIOsize), CL_ONLY_FILE_LOG));
#endif

				translate_operation(tpMessage->_session, tpMessage->dwIOsize, lpBuffer, operation);

				// free memory
				if (tpMessage != nullptr)
					delete tpMessage;
#endif

			}catch (exception& e) {
//#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[threadpool::worker_io_send][ErrorSystem][MY] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#else
	//			if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 2))
		//			_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#endif

				// Se for STDA_OT_SEND_COMPLETED reseta o send
				if (lpBuffer != nullptr && (lpBuffer->getOperation() == STDA_OT_SEND_COMPLETED || lpBuffer->getOperation() == STDA_OT_SEND_RAW_COMPLETED))
					if (_session != nullptr)
						_session->releaseSend();

				translate_operation(_session, 0/*disconnect*/, lpBuffer, STDA_OT_RECV_COMPLETED);

			}catch (std::exception& e) {
				
				_smp::message_pool::getInstance().push(new message(std::string("[threadpool::worker_io_send][ErrorSystem][STD] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));

				// Se for STDA_OT_SEND_COMPLETED reseta o send
				if (lpBuffer != nullptr && (lpBuffer->getOperation() == STDA_OT_SEND_COMPLETED || lpBuffer->getOperation() == STDA_OT_SEND_RAW_COMPLETED))
					if (_session != nullptr)
						_session->releaseSend();

				translate_operation(_session, 0/*disconnect*/, lpBuffer, STDA_OT_RECV_COMPLETED);

			}catch (...) {
				
				std::cout << "worker_io_send() ->while Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";

				// Se for STDA_OT_SEND_COMPLETED reseta o send
				if (lpBuffer != nullptr && (lpBuffer->getOperation() == STDA_OT_SEND_COMPLETED || lpBuffer->getOperation() == STDA_OT_SEND_RAW_COMPLETED))
					if (_session != nullptr)
						_session->releaseSend();

				translate_operation(_session, 0/*disconnect*/, lpBuffer, STDA_OT_RECV_COMPLETED);
			}
		}
	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[threadpool::worker_io_send][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "worker_io_send() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}

	_smp::message_pool::getInstance().push(new message("Saindo de worker_io_send()..."));

	return 0u;
};

#if defined(_WIN32)
DWORD threadpool::worker_io_recv(DWORD _index) 
#elif defined(__linux__)
void* threadpool::worker_io_recv(DWORD _index) 
#endif
{
	
	try {
		_smp::message_pool::getInstance().push(new message("Ola console, here is thread worker IO Recv: " + std::to_string(
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
		int32_t ret_recv = 0;
		stThreadpoolMessage *tpMessage = nullptr;
		DWORD operation = ~0;
#endif

		while (1) {
			try {
#if defined(_WIN32)
				try {
					operation = ~0;

					m_iocp_io_recv[_index].getStatus((PULONG_PTR)&_session, &dwIOsize, (LPOVERLAPPED*)&lpBuffer);

				}catch (exception& e) {

					if (STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_SEM_TIMEOUT) {
						DWORD flags_over = 0u;
						DWORD last_err = 0u;

						if (!WSAGetOverlappedResult(_session->m_sock, (LPOVERLAPPED)lpBuffer, &dwIOsize, FALSE, &flags_over)
								&& (last_err = WSAGetLastError()) != WSA_IO_INCOMPLETE && last_err != WSAETIMEDOUT)
							throw exception("[threadpool::worker_io_recv][Error] exception ERROR_SEM_TIMEOUT dwIOsize=" + std::to_string(dwIOsize) + ", Flags_Over="
									+ std::to_string(flags_over) + " que retornou da operacao no iocp. session[IP=" + _session->getIP() + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 255, last_err));

						// Marca na session que o socket, levou DC, chegou ao limit de retramission do TCP para transmitir os dados
						// TCP sockets is that the maximum retransmission count and timeout have been reached on a bad(or broken) link
						_session->m_connection_timeout = true;

						// Se passar tenho que colocar o dwIOsize para zero para ele ser disconnectado
						_smp::message_pool::getInstance().push(new message("[threadpool::worker_io_recv][WARNING] exception ERROR_SEM_TIMEOUT dwIOsize=" + std::to_string(dwIOsize) + ", Flags_Over="
								+ std::to_string(flags_over) + " que retornou da operacao no iocp[WSAError=" + std::to_string(last_err) + "]. session[IP=" + _session->getIP() + "]", CL_FILE_LOG_AND_CONSOLE));

						// Zera para desconectar a session
						dwIOsize = 0u;

					}else if (!(STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_NETNAME_DELETED || STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_CONNECTION_ABORTED
						|| STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) == ERROR_OPERATION_ABORTED))
						throw;
				}

				if (_session == nullptr)
					break;

				if (lpBuffer != nullptr)
					operation = lpBuffer->getOperation();

#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("Operation typeIOS: " + std::to_string(operation) + "\tlen: " + std::to_string(dwIOsize), CL_ONLY_FILE_LOG));
#endif

				translate_operation(_session, dwIOsize, lpBuffer, operation);
#elif defined(__linux__)
				try {
					operation = ~0;

					tpMessage = m_iocp_io_recv[_index].get(INFINITE);

				}catch (exception& e) {

					// Diferente de timeout
					if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::LIST_FIFO, 2))
						throw;
				}

				if (tpMessage == nullptr)
					break;

				_session = tpMessage->_session;
				lpBuffer = tpMessage->buffer;

				if (tpMessage->buffer != nullptr)
					operation = tpMessage->buffer->getOperation();

#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("Operation typeIOS: " + std::to_string(operation) + "\tlen: " + std::to_string(tpMessage->dwIOsize), CL_ONLY_FILE_LOG));
#endif

				translate_operation(_session, tpMessage->dwIOsize, lpBuffer, operation);

				// free memory
				if (tpMessage != nullptr)
					delete tpMessage;
#endif
			}catch (exception& e) {
//#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[threadpool::worker_io_recv][ErrorSystem][MY] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#else
	//			if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 2))
//					_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#endif

				translate_operation(_session, 0/*disconnect*/, lpBuffer, STDA_OT_RECV_COMPLETED);

			}catch (std::exception& e) {

				_smp::message_pool::getInstance().push(new message(std::string("[threadpool::worker_io_recv][ErrorSystem][STD] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));

				translate_operation(_session, 0/*disconnect*/, lpBuffer, STDA_OT_RECV_COMPLETED);

			}catch (...) {

				std::cout << "worker_io_recv() ->while Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";

				translate_operation(_session, 0/*disconnect*/, lpBuffer, STDA_OT_RECV_COMPLETED);
			}
		}
	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[threadpool::worker_io_recv][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "worker_io_recv() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}

	_smp::message_pool::getInstance().push(new message("Saindo de worker_io_recv()..."));

	return 0u;
};

#if defined(_WIN32)
DWORD threadpool::worker_logical(DWORD _index) 
#elif defined(__linux__)
void* threadpool::worker_logical(DWORD _index) 
#endif
{

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

				tpMessage = m_iocp_logical.get(INFINITE);

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
//#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[threadpool::worker_logical][ErrorSystem][MY] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#else
		//		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 2))
			//		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#endif
			}catch (std::exception& e) {
				_smp::message_pool::getInstance().push(new message(std::string("[threadpool::worker_logical][ErrorSystem][STD] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));
			}catch (...) {
				std::cout << "worker_logical() ->while Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
			}
		}
	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[threadpool::worker_logical][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "worker_logical() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}

	_smp::message_pool::getInstance().push(new message("Saindo de worker_logical()..."));

	return 0u;
};

#if defined(_WIN32)
DWORD threadpool::worker_send(DWORD _index) 
#elif defined(__linux__)
void* threadpool::worker_send(DWORD _index) 
#endif
{
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
		stThreadpoolMessage* tpMessage = nullptr;
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

				tpMessage = m_iocp_send.get(INFINITE);

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
//#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[threadpool::worker_send][ErrorSystem][MY] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#else
	//			if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 2))
		//			_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
//#endif
			}catch (std::exception& e) {
				_smp::message_pool::getInstance().push(new message(std::string("[threadpool::worker_send][ErrorSystem][STD] ") + e.what(), CL_FILE_LOG_AND_CONSOLE));
			}catch (...) {
				std::cout << "worker_send() ->while Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
			}
		}
	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[threadpool::worker_send][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "worker_send() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}

	_smp::message_pool::getInstance().push(new message("Saindo de worker_send()..."));

	return 0u;
};

#if defined(_WIN32)
DWORD threadpool::console() 
#elif defined(__linux__)
void* threadpool::console() 
#endif
{

	try {
		while (1)
			_smp::message_pool::getInstance().console_log();
	}catch (exception& e) {
		std::cout << formatDateLocal(0) + "\t" + e.getFullMessageError() << std::endl;
	}catch (std::exception& e) {
		std::cout << formatDateLocal(0) + "\t" + e.what() << std::endl;
	}catch (...) {
		std::cout << formatDateLocal(0) + "\tconsole() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n" << std::endl;
	}

	std::cout << formatDateLocal(0) + "\tSaindo de console()..." << std::endl;
	
	return 0u;
};

#if defined(_WIN32)
DWORD threadpool::_job() 
#elif defined(__linux__)
void* threadpool::_job() 
#endif
{

	call_func func = nullptr;
	stdA::job *_job = nullptr;

	try {

		_smp::message_pool::getInstance().push(new message("job iniciado com sucesso!"));

		while (1) {
			if ((_job = m_job_pool.getJob()) == nullptr)
				break;	// Sai da thread job
			
			try {
				if (_job->execute_job() != 0)
					_smp::message_pool::getInstance().push(new message("[threadpool::_job][Error] Ao executar a tarefa.", CL_ONLY_FILE_LOG));

				delete _job;
			}catch (exception& e) {
				if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::JOB)
					_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
				else
					throw;
			}
		}

	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message("[threadpool::_job][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (std::exception& e) {
		_smp::message_pool::getInstance().push(new message("[threadpool::_job][ErrorSystem] " + std::string(e.what()), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "job() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}

	_smp::message_pool::getInstance().push(new message("Saindo de job()..."));

	return 0u;
};

void threadpool::translate_operation(session *_session, DWORD dwIOsize, Buffer *lpBuffer, DWORD _operation) {

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
		case STDA_OT_ACCEPT_COMPLETED:
			accept_completed((SOCKET*)_session, dwIOsize, (myOver*)lpBuffer, _operation);

			if (lpBuffer != nullptr) {
				if (((myOver*)lpBuffer)->buffer != nullptr)
					delete ((myOver*)lpBuffer)->buffer;

				delete ((myOver*)lpBuffer);
			}
			break;
		default: // Operation invalid disconnect session
		{
			_smp::message_pool::getInstance().push(new message("[threadpool::translate_operation][Error] Operation[VALUE=" + std::to_string(_operation) + "] invalid", CL_FILE_LOG_AND_CONSOLE));
			
			translate_packet(_session, lpBuffer, 0/*disconnect*/, STDA_OT_RECV_COMPLETED);
			
			break;
		}
	}
};

void threadpool::send_new(session *_session, Buffer *lpBuffer, DWORD operation) {
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
				postIoOperationR(_session, lpBuffer, 0, STDA_OT_RECV_COMPLETED);

			return;
		}

	}catch (exception& e) {
		
		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*Error ao usar session*/)) {
			
			if (_session->devolve()) {

				if (_session->isCreated())
					// Send 0 Bytes, Session desconnected
					postIoOperationR(_session, lpBuffer, 0, STDA_OT_RECV_COMPLETED);

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

				_smp::message_pool::getInstance().push(new message("[threadpool::send_new][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			}

			if (_session->isCreated())
				// Send 0 Bytes, Session desconnected
				postIoOperationR(_session, lpBuffer, 0, STDA_OT_RECV_COMPLETED);
			
			if (error != WSAESHUTDOWN && error != WSAECONNRESET)
				throw exception("Erro no WSASend() -> threadpool::send_new()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 1, error));
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
					postIoOperationR(_session, lpBuffer, 0, STDA_OT_RECV_COMPLETED);
			}

			return;
		}
		
	} catch(exception& e) {

		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::SESSION, 6/*Error ao usar session*/)) {
			
			if (_session->devolve()) {

				if (_session->isCreated())
					// Send 0 Bytes, Session desconnected
					postIoOperationR(_session, lpBuffer, 0, STDA_OT_RECV_COMPLETED);

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

				_smp::message_pool::getInstance().push(new message("[threadpool::send_new][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			}

			if (_session->isCreated())
				// Send 0 Bytes, Session desconnected
				postIoOperationR(_session, lpBuffer, 0, STDA_OT_RECV_COMPLETED);
			
			if (errno != ECONNRESET && errno != EPIPE/*socket invalid*/)
				throw exception("Erro no WSASend() -> threadpool::send_new()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 1, errno));
			
		}else {

#ifdef _DEBUG
			// !@
			_smp::message_pool::getInstance().push(new message("[threadpool::send_new][WARNING] nao conseguiu enviar nesse momento. <---------------------->", CL_FILE_LOG_AND_CONSOLE));
#endif

			// Not have space in socket tcp stack, release send for send again to late
			try {
				_session->releaseSend();
			}catch (exception& e) {

				_smp::message_pool::getInstance().push(new message("[threadpool::send_new][Error] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
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
		
		// !@ send partial
		if ((uint32_t)sendlen != lpBuffer->getWSABufToSend()->len)
			_smp::message_pool::getInstance().push(new message("[threadpool::send_new][WARNING] Player[UID=" + std::to_string(_session->getUID()) 
					+ "] enviou dados partial[SENDLEN=" + std::to_string(sendlen) + "].", CL_FILE_LOG_AND_CONSOLE));

		// post to translate
		postIoOperationS(_session, lpBuffer, sendlen, STDA_OT_SEND_COMPLETED);
	}
#endif
};

void threadpool::recv_new(session *_session, Buffer *lpBuffer, DWORD operation) {
	DWORD dwFlag = 0;

	lpBuffer->setOperation(operation);

	if (!_session->isConnected())
		return;

	//_session->lock();

#if defined(_WIN32)
	if (SOCKET_ERROR == WSARecv(_session->m_sock, lpBuffer->getWSABufToRead(), 1, NULL, &dwFlag, (LPOVERLAPPED)lpBuffer, NULL)) {
		int error = WSAGetLastError();
		
		if (WSA_IO_PENDING != error) {
			/*_session->lock();

			//_session->m_sock = INVALID_SOCKET;
			_session->setState(false);

			_session->unlock();	// Unlock*/

			if (_session->isCreated())
				// Send 0 Bytes, Session desconnected
				postIoOperationR(_session, lpBuffer, 0, operation);

			if (error != WSAESHUTDOWN && error != WSAECONNRESET)
				throw exception("Erro no WSARecv() -> threadpool::recv_new()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 2, error));
		}
	}
#elif defined(__linux__)
	int recvlen = 0;
	if ((recvlen = ::recv(_session->m_sock.fd, lpBuffer->getWSABufToRead()->buf, lpBuffer->getWSABufToRead()->len, 0)) == SOCKET_ERROR) {
		
		if (errno != EWOULDBLOCK && errno != EAGAIN) {
			/*_session->lock();

			//_session->m_sock = INVALID_SOCKET;
			_session->setState(false);

			_session->unlock();	// Unlock*/

			if (_session->isCreated())
				// Send 0 Bytes, Session desconnected
				postIoOperationR(_session, lpBuffer, 0, /*operation*/STDA_OT_RECV_COMPLETED);

			if (errno != ECONNRESET)
				throw exception("Erro no WSARecv() -> threadpool::recv_new()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::THREADPOOL, 2, errno));
			
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
		postIoOperationR(_session, lpBuffer, recvlen, STDA_OT_RECV_COMPLETED);
	}
#endif

	//_session->unlock();
};

void threadpool::postIoOperation(session *_session, Buffer* lpBuffer, DWORD dwIOsize, DWORD operation) {
	myOver *tmp = new myOver;

	tmp->clear();
	tmp->tipo = operation;
	tmp->buffer = lpBuffer;
	
#if defined(_WIN32)
	UNREFERENCED_PARAMETER(dwIOsize);
	m_iocp_send.postStatus((ULONG_PTR)_session, 0, (LPOVERLAPPED)tmp);	// Esse tinha Index[session->m_key], era um array de 16 iocp
#elif defined(__linux__)
	m_iocp_send.push(new stThreadpoolMessage(_session, (Buffer*)tmp, dwIOsize));
#endif
};

void threadpool::postIoOperationL(session *_session, Buffer* lpBuffer, DWORD operation) {
	myOver *tmp = new myOver;

	tmp->clear();
	tmp->tipo = operation;
	tmp->buffer = lpBuffer;

#if defined(_WIN32)
	m_iocp_logical.postStatus((ULONG_PTR)_session, 0, (LPOVERLAPPED)tmp);	// Esse tinha Index[session->m_key], era um array de 16 iocp
#elif defined(__linux__)
	m_iocp_logical.push(new stThreadpoolMessage(_session, (Buffer*)tmp, 0));
#endif
};

void threadpool::postIoOperationS(session *_session, Buffer *lpBuffer, DWORD dwIOsize, DWORD operation) {
#if defined(_WIN32)
	memset(lpBuffer, 0, sizeof(OVERLAPPED));	// Zera

	lpBuffer->setOperation(operation);

	m_iocp_io_send[_session->m_key].postStatus((ULONG_PTR)_session, dwIOsize, (LPOVERLAPPED)lpBuffer);
#elif defined(__linux__)
	if (lpBuffer != nullptr)
		lpBuffer->setOperation(operation);

	if (_session != nullptr)
		m_iocp_io_send[_session->m_key].push(new stThreadpoolMessage(_session, lpBuffer, dwIOsize));
#endif
};

void threadpool::postIoOperationR(session * _session, Buffer *lpBuffer, DWORD dwIOsize, DWORD operation) {
#if defined(_WIN32)
	memset(lpBuffer, 0, sizeof(OVERLAPPED));	// Zera

	lpBuffer->setOperation(operation);

	m_iocp_io_recv[_session->m_key].postStatus((ULONG_PTR)_session, dwIOsize, (LPOVERLAPPED)lpBuffer);
#elif defined(__linux__)
	if (lpBuffer != nullptr)
		lpBuffer->setOperation(operation);

	if (_session != nullptr)
		m_iocp_io_recv[_session->m_key].push(new stThreadpoolMessage(_session, lpBuffer, dwIOsize));
#endif
};

void threadpool::postJob(job *_job) {
	m_job_pool.push(_job);
};

timer *threadpool::postTimer(DWORD _time, timer::timer_param *_arg, uint32_t _tipo) {
	return m_timer_mgr.createTimer(_time, _arg, _tipo);
};

timer *threadpool::postTimer(DWORD _time, timer::timer_param *_arg, std::vector< DWORD > _table_interval, uint32_t _tipo) {
	return m_timer_mgr.createTimer(_time, _arg, _table_interval, _tipo);
};

void threadpool::removeTimer(timer *_timer) {
	m_timer_mgr.deleteTimer(_timer);
};

job_pool& threadpool::getJobPool() {
	return m_job_pool;
};
