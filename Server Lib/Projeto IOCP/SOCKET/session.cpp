// Arquivo session.cpp
// Criado em 04/06/2017 por Acrisio
// Implementação da classe session

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#endif

#include "session.h"
#include "../UTIL/exception.h"
#include "../THREAD POOL/threadpool_base.hpp"
#include "../UTIL/message_pool.h"
#include "../UTIL/hex_util.h"

#include "../TYPE/stda_error.h"

using namespace stdA;

session::session(threadpool_base& _threadpool) : m_threadpool(_threadpool), m_use_ctx() {
#if defined(_WIN32)
	m_sock = INVALID_SOCKET;
#elif defined(__linux__)
    m_sock.fd = INVALID_SOCKET;
	m_sock.connect_time.tv_sec = 0;
	m_sock.connect_time.tv_nsec = 0;
#endif

    m_key = 0;
	m_addr = { 0 };

	m_time_start = 0l;
	m_tick = 0l;
	m_tick_bot = 0l;

	m_ip_maked = false;

	m_oid = ~0;

	m_is_authorized = 0u;

	m_connection_timeout = false;

	m_request_recv = 0ll;

	m_check_packet.clear();

	memset(m_ip, 0, sizeof(m_ip));

	m_buff_r.buff.init(0);
	m_buff_s.buff.init(0);

	setPacketS(nullptr);
	setPacketR(nullptr);

	//m_threadpool = nullptr;

	// Virou uma classe o buff_ctx, agora chame essa função no construtor da classe
	/*m_buff_s.init();
	m_buff_r.init();*/

	m_state = false;
	m_connected = false;
	m_connected_to_send = false;

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_cs_lock_other);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs_lock_other);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
};

session::session(threadpool_base& _threadpool, SOCKET _sock, SOCKADDR_IN _addr, unsigned char _key) 
		: m_threadpool(_threadpool), m_sock(_sock), m_addr(_addr), m_key(_key), m_use_ctx() {

	m_ip_maked = false;
	
	m_oid = ~0;

	m_is_authorized = 0u;

	m_connection_timeout = false;

	m_request_recv = 0ll;

	m_check_packet.clear();

	memset(m_ip, 0, sizeof(m_ip));
	
	make_ip();

	m_buff_r.buff.init(0);
	m_buff_s.buff.init(0);

	setPacketS(nullptr);
	setPacketR(nullptr);

	//m_threadpool = nullptr;

	// Virou uma classe o buff_ctx, agora chame essa função no construtor da classe
	/*m_buff_s.init();
	m_buff_r.init();*/

#if defined(_WIN32)
	if (m_sock != INVALID_SOCKET)
#elif defined(__linux__)
	if (m_sock.fd != INVALID_SOCKET)
#endif
		m_state = true;

	m_connected = false;
	m_connected_to_send = false;

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_cs_lock_other);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs_lock_other);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
};

session::~session() {
	
	clear();

	// Virou uma classe o buff_ctx, agora chame essa função no destrutor da classe
	/*m_buff_s.destroy();
	m_buff_r.destroy();*/

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
	DeleteCriticalSection(&m_cs_lock_other);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
	pthread_mutex_destroy(&m_cs_lock_other);
#endif
};

bool session::clear() {

	if (!m_use_ctx.checkCanQuit()) {

		_smp::message_pool::getInstance().push(new message("[session::clear][WARNING] [Session OID=" + std::to_string(m_oid) 
				+ "] o buffer esta sendo usada, espera, o proximo thread que esta usando tem que limpar a session.", CL_ONLY_FILE_LOG));

// !@ Teste
#if defined(__linux__)
		// Libera o send buffer para deletar a session
		// !@ deu dead lock no login server isso aqui
		//releaseSend();
#endif
		
		return false;
	}

	m_state = false;
	m_connected = false;
	m_connected_to_send = false;

#if defined(_WIN32)
	if (m_sock != INVALID_SOCKET) {
		shutdown(m_sock, SD_BOTH);
		closesocket(m_sock);
	}

	m_sock = INVALID_SOCKET;
#elif defined(__linux__)
	if (m_sock.fd != INVALID_SOCKET) {
		shutdown(m_sock.fd, SD_BOTH);
		closesocket(m_sock.fd);
	}

	m_sock.fd = INVALID_SOCKET;
	m_sock.connect_time.tv_sec = 0;
	m_sock.connect_time.tv_nsec = 0;
#endif
	
	m_key = 0;
	m_addr = { 0 };
	
	m_time_start = 0l;
	m_tick = 0l;
	m_tick_bot = 0l;
	
	m_oid = ~0;

	m_is_authorized = 0u;

	m_connection_timeout = false;

	m_request_recv = 0ll;

	m_check_packet.clear();

	memset(m_ip, 0, sizeof(m_ip));
	m_ip_maked = false;
	
	//m_pi.clear();

	m_buff_s.clear();
	m_buff_r.clear();

	if (getPacketS() != nullptr)
		delete getPacketS();

	setPacketS(nullptr);

	if (getPacketR() != nullptr)
		delete getPacketR();

	setPacketR(nullptr);

	m_use_ctx.clear();

	return true;
};

const char* session::getIP() {

	if (!m_ip_maked || (m_addr.sin_addr.s_addr != 0 && strcmp(m_ip, "0.0.0.0") == 0))
		make_ip();

	return m_ip;
};

void session::make_ip() {

	if (!m_ip_maked || (m_addr.sin_addr.s_addr != 0 && strcmp(m_ip, "0.0.0.0") == 0)) {

		if (inet_ntop(AF_INET, &m_addr.sin_addr.s_addr, m_ip, sizeof(m_ip)) == nullptr)
#if defined(_WIN32)
			throw exception("Erro ao converter SOCKADDR_IN para string doted mode(IP). session::make_ip()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION, 1, 0));
#elif defined(__linux__)
			throw exception("Erro ao converter SOCKADDR_IN para string doted mode(IP). session::make_ip()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION, 1, errno));
#endif

		m_ip_maked = true;
	}
};

bool session::isConnectedToSend() {
	
	bool ret = false;

	try {

		lock();

		// getConnectTime pode lançar exception
		ret = m_connected_to_send && (getConnectTime() >= 0);

		unlock();
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[session::isConnectedToSend][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		unlock();
	}

	return ret;
};

void session::lock() {
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif
};

void session::unlock() {
#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
};

void session::lockSync() {
#if defined(_WIN32)
	EnterCriticalSection(&m_cs_lock_other);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs_lock_other);
#endif
};

void session::unlockSync() {
#if defined(_WIN32)
	LeaveCriticalSection(&m_cs_lock_other);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs_lock_other);
#endif
};

void session::requestSendBuffer(void* _buff, size_t _size, bool _raw) {

	if (_buff == nullptr)
		throw exception("Error _buff is nullptr. session::requestSendBuffer()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION, 3, 0));

	if (_size <= 0)
		throw exception("Error _size is less or equal the zero. session::requestSendBuffer()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION, 4, 0));
	
	m_buff_s.lock();

	if (isConnectedToSend() && m_buff_s.isWritable() && isConnectedToSend()) 
	{
			
		m_buff_s.setWrite();

		size_t sz_write = 0u;
		
		do {
			
			if (isConnectedToSend() && m_buff_s.readyToWrite() && isConnectedToSend()) {

				if (_raw && m_buff_s.buff.getUsed() > 0 
					&& m_buff_s.buff.getOperation() != STDA_OT_SEND_RAW_REQUEST && m_buff_s.buff.getOperation() != STDA_OT_SEND_RAW_COMPLETED) {

					m_buff_s.setPartialSend();

					m_threadpool.postIoOperation(this, &m_buff_s.buff, 0, STDA_OT_SEND_REQUEST);

				}else {

					sz_write += m_buff_s.buff.write((void*)((unsigned char*)_buff + sz_write), _size - sz_write);

					// Buffer já chegou ao seu limite, libera o parcial send, para o buffer ser esvaziado(Enviado)
					if (sz_write < _size)
						m_buff_s.setPartialSend();

					m_threadpool.postIoOperation(this, &m_buff_s.buff, 0, (_raw ? STDA_OT_SEND_RAW_REQUEST : STDA_OT_SEND_REQUEST));
				}

			}else { // Não conseguiu entrar para escrever ou a session não está mais conectada, libera o state write
				
				m_buff_s.releaseWrite();

				m_buff_s.unlock();

				return;
			}
		
		} while (sz_write < _size);

		// Libera o State Write
		m_buff_s.releaseWrite();	
	}

	m_buff_s.unlock();
};

void session::requestRecvBuffer() {
#if defined(_WIN32)
	m_threadpool.postIoOperation(this, &m_buff_r.buff, 0, STDA_OT_RECV_REQUEST);
#elif defined(__linux__)
	setRecv();
#endif
};

void session::setRecv() {

	m_buff_r.lock();

	if (m_request_recv <= 0ll) {

		m_request_recv = 1ll;

		m_threadpool.postIoOperation(this, &m_buff_r.buff, 0, STDA_OT_RECV_REQUEST);
		
	}else
		m_request_recv++;

	m_buff_r.unlock();
};

void session::releaseRecv() {

	m_buff_r.lock();

#if defined(__linux__)
	if (--m_request_recv > 0ll)
#endif
		m_threadpool.postIoOperation(this, &m_buff_r.buff, 0, STDA_OT_RECV_REQUEST);

	m_buff_r.unlock();
};

void session::setSend() {

	m_buff_s.lock();

	// Verifica antes e depois se a session ainda está conectada
	if (isConnectedToSend() && m_buff_s.isSendable() && isConnectedToSend()) {

		if (m_buff_s.buff.getUsed() <= 0) {
			
			m_buff_s.unlock();

			throw exception("[session::setSend][Error] nao tem nada no buffer para ser enviado[SIZE_BUFF=" + std::to_string(m_buff_s.buff.getUsed()) + "].", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION, 5, 0));
		}

		if (isConnectedToSend() && m_buff_s.readyToSend() && isConnectedToSend()) {
			
			m_buff_s.setSend();

			m_buff_s.increseRequestSendCount();
		
		}else {

			m_buff_s.releaseSendAndPartialSend();

			m_buff_s.unlock();
			
			// Lança o erro para ser exibido e não chamar o WSASend para não da error, pois a session já não é mais valida para enviar dados
			throw exception("[session::setSend][Error] nao conseguiu set o Send por que a session nao esta mais conectada.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION, 2, 0));
		}
	
	}else {

		m_buff_s.unlock();

		// Lança o erro para ser exibido e não chamar o WSASend para não da error, pois a session já não é mais valida para enviar dados
		throw exception("[session::setSend][Error] nao conseguiu set o Send por que a session nao esta mais conectada.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION, 2, 0));
	}

	m_buff_s.unlock();
};

void session::setSendPartial() {

	m_buff_s.lock();

	// Verifica antes e depois se a session ainda está conectada
	if (m_buff_s.isSetedToSend() && m_buff_s.buff.getUsed() > 0) {
		
		m_buff_s.setPartialSend();
	
		// Post new request to send buff, to send rest of buff
		m_threadpool.postIoOperation(this, &m_buff_s.buff, 0, m_buff_s.buff.getOperation());
	}

	m_buff_s.unlock();
};

void session::releaseSend() {

	m_buff_s.lock();

	if (m_buff_s.decreaseRequestSendCount() >= 0 || m_buff_s.isSetedToSendOrPartialSend())
		m_buff_s.releaseSendAndPartialSend();
	else
		_smp::message_pool::getInstance().push(new message("[session::releaseSend][WARNING] todos os request send ja foram liberados.", CL_FILE_LOG_AND_CONSOLE));

	m_buff_s.unlock();
};

bool session::isConnected() {
	bool ret = false;

	try {

		lock();

		// getConnectTime pode lançar exception
		ret = m_connected && (getConnectTime() >= 0);

		unlock();
	
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[session::isConnected][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		unlock();
	}

	return ret;
};

bool session::isCreated() {
	bool ret = false;

	lock();

	ret = m_state;

	unlock();

	return ret;
};

int session::getConnectTime() {

#if defined(_WIN32)
	if (m_sock != INVALID_SOCKET && getState()) {
		int seconds;
		int size_seconds = sizeof(seconds);

		if (0 == ::getsockopt(m_sock, SOL_SOCKET, SO_CONNECT_TIME, (char*)&seconds, &size_seconds))
			return seconds;
		else
			throw exception("[session::getConnectTime] erro ao pegar optsock SO_CONNECT_TIME.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION, 50, WSAGetLastError()));
	}
#elif defined(__linux__)
	if (m_sock.fd != INVALID_SOCKET && getState()) {

		if (m_sock.connect_time.tv_nsec == 0 && m_sock.connect_time.tv_sec == 0)
			return -1;

		int rt_flg = fcntl(m_sock.fd, F_GETFL);

		if (rt_flg == -1)
			return (errno == EAGAIN | errno == EBADF | errno == EACCES) ? -1 : -2; // -1 socket is not connected, -2 Erro ao pegar status flag do socket

		int error = 0;
		socklen_t len = sizeof (error);
		int retval = getsockopt (m_sock.fd, SOL_SOCKET, SO_ERROR, &error, &len);

		if (retval != 0) {
			/* there was a problem getting the error code */
			return -1;
		}

		if (error != 0) {
			/* socket has a non zero error status */
			return -1;
		}

		timespec ts;

		clock_gettime(SESSION_CONNECT_TIME_CLOCK, &ts);

		return (int)(((uint64_t)(ts.tv_sec * 1000000000 + ts.tv_nsec) - (uint64_t)(m_sock.connect_time.tv_sec * 1000000000 + m_sock.connect_time.tv_nsec)) / 1000000000);
	}
#endif
		
	return -1;
};

/* void session::setThreadpool(threadpool* _threadpool) {
	m_threadpool = _threadpool;
}; */

packet* session::getPacketS() {
#if defined(_WIN32)
	return (packet*)InterlockedExchangePointer((PVOID*)&m_packet_s, m_packet_s);
#elif defined(__linux__)
	return (packet*)__atomic_exchange_n((void**)&m_packet_s, m_packet_s, __ATOMIC_RELAXED);
#endif
};

void session::setPacketS(packet *_packet) {
#if defined(_WIN32)
	InterlockedExchangePointer((PVOID*)&m_packet_s, _packet);
#elif defined(__linux__)
	__atomic_exchange_n((void**)&m_packet_s, _packet, __ATOMIC_RELAXED);
#endif
};

packet* session::getPacketR() {
#if defined(_WIN32)
	return (packet*)InterlockedExchangePointer((PVOID*)&m_packet_r, m_packet_r);
#elif defined(__linux__)
	return (packet*)__atomic_exchange_n((void**)&m_packet_r, m_packet_r, __ATOMIC_RELAXED);
#endif
};

void session::setPacketR(packet *_packet) {
#if defined(_WIN32)
	InterlockedExchangePointer((PVOID*)&m_packet_r, _packet);
#elif defined(__linux__)
	__atomic_exchange_n((void**)&m_packet_r, _packet, __ATOMIC_RELAXED);
#endif
};

int32_t session::usa() {

	if (!isConnected())
		throw exception("[session::usa][error] nao pode usa porque o session nao esta mais conectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION, 6, 0));

	return m_use_ctx.usa();
};

bool session::devolve() {
	return m_use_ctx.devolve();
};

bool session::isQuit() {
	return m_use_ctx.isQuit();
};

bool session::getState() {
	return m_state;
};

void session::setState(bool _state) {
	m_state = _state;
};

void session::setConnected(bool _connected) {
	
	m_connected = _connected;
	
	// Espelho de connected exceto quando o setConnectedToSend é chamando que vão ter outros valores,
	// esse aqui é para quando o socket WSASend retorna WSAECONNREST, que o getTimeConnect não vai detectar no mesmo estante que o socket foi resetado,
	// essa flag é para bloquea os requestSend no socket, para não gerar deadlock no buffer_send do socket
	setConnectedToSend(_connected);
};

void session::setConnectedToSend(bool _connected_to_send) {
	m_connected_to_send = _connected_to_send;
};

// Implementação da class interna session::buff_ctx
session::buff_ctx::buff_ctx() {

	init();
}

session::buff_ctx::~buff_ctx() {

	destroy();
}

void session::buff_ctx::init() {

	state_send = 0u;
	state_write = 0u;
	state_wr_send = 0u;

	request_send_count = 0ll;

#if defined(_WIN32)
	InitializeCriticalSection(&cs);

	InitializeConditionVariable(&cv_send);
	InitializeConditionVariable(&cv_write);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;

	pthread_cond_init(&cv_send, nullptr);
	pthread_cond_init(&cv_write, nullptr);
#endif
}

void session::buff_ctx::destroy() {

	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&cs);
	pthread_cond_destroy(&cv_send);
	pthread_cond_destroy(&cv_write);
#endif
}

void session::buff_ctx::clear() {

	lock();

	buff.reset();

	state_send = 0u;
	state_write = 0u;
	state_wr_send = 0u;

	request_send_count = 0ll;

#if defined(_WIN32)
	WakeAllConditionVariable(&cv_write);
	WakeAllConditionVariable(&cv_send);
#elif defined(__linux__)
	pthread_cond_broadcast(&cv_write);
	pthread_cond_broadcast(&cv_send);
#endif

	unlock();
}

void session::buff_ctx::lock() {
#if defined(_WIN32)
	EnterCriticalSection(&cs);
#elif defined(__linux__)
	pthread_mutex_lock(&cs);
#endif
}

void session::buff_ctx::unlock() {
#if defined(_WIN32)
	LeaveCriticalSection(&cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&cs);
#endif
}

bool session::buff_ctx::isWritable() {
	
	while (state_write) {

#if defined(_WIN32)
		if (SleepConditionVariableCS(&cv_write, &cs, INFINITE) == 0) {

			DWORD error = GetLastError();

			if (error == ERROR_TIMEOUT) {

				_smp::message_pool::getInstance().push(new message("[session::buff_ctx::isWritable][Error] Time out no SleepConditionVariableCS, \
								mas o tempo de timeout era INFINITE, error desconhecido.", CL_FILE_LOG_AND_CONSOLE));

				// Não está preparado para escrever
				return false;

			}else {	// outro tipo de erro

				_smp::message_pool::getInstance().push(new message("[session::buff_ctx::isWritable][Error] problema para pegar o sinal do SleepConditionVariableCS. Error code: "
						+ std::to_string(error), CL_FILE_LOG_AND_CONSOLE));

				// Não está preparado para escrever
				return false;
			}
		}
#elif defined(__linux__)
		int32_t error = 0;

		if ((error = pthread_cond_wait(&cv_write, &cs)) != 0) {

			_smp::message_pool::getInstance().push(new message("[session::buff_ctx::isWritable][Error] problema para pegar o sinal do pthread_cond_wait. Error code: "
						+ std::to_string(error), CL_FILE_LOG_AND_CONSOLE));

			// Não está preparado para escrever
			return false;
		}
#endif
	}

	return true;
}

bool session::buff_ctx::readyToWrite() {
			
	while (state_send || state_wr_send) {

#if defined(_WIN32)
		if (SleepConditionVariableCS(&cv_send, &cs, INFINITE) == 0) {

			DWORD error = GetLastError();

			if (error == ERROR_TIMEOUT) {

				_smp::message_pool::getInstance().push(new message("[session::buff_ctx::readyToWrite][Error] Time out no SleepConditionVariableCS, \
								mas o tempo de timeout era INFINITE, error desconhecido.", CL_FILE_LOG_AND_CONSOLE));

				// Não está preparado para escrever
				return false;

			}else {	// outro tipo de erro

				_smp::message_pool::getInstance().push(new message("[session::buff_ctx::readyToWrite][Error] problema para pegar o sinal do SleepConditionVariableCS. Error code: "
						+ std::to_string(error), CL_FILE_LOG_AND_CONSOLE));

				// Não está preparado para escrever
				return false;
			}
		}
#elif defined(__linux__)

		int32_t error = 0;
		
		if ((error = pthread_cond_wait(&cv_send, &cs)) != 0) {

			_smp::message_pool::getInstance().push(new message("[session::buff_ctx::readyToWrite][Error] problema para pegar o sinal do pthread_cond_wait. Error code: "
						+ std::to_string(error), CL_FILE_LOG_AND_CONSOLE));

			// Não está preparado para escrever
			return false;	
		}
#endif
	}

	return true;
}

bool session::buff_ctx::isSendable() {
	
	while (state_send) {

#if defined(_WIN32)
		if (SleepConditionVariableCS(&cv_send, &cs, INFINITE) == 0) {

			DWORD error = GetLastError();

			if (error == ERROR_TIMEOUT) {

				_smp::message_pool::getInstance().push(new message("[session::buff_ctx::isSendable][Error] Time out no SleepConditionVariableCS, \
								mas o tempo de timeout era INFINITE, error desconhecido.", CL_FILE_LOG_AND_CONSOLE));

				// Não está preparado para escrever
				return false;

			}else {	// outro tipo de erro

				_smp::message_pool::getInstance().push(new message("[session::buff_ctx::isSendable][Error] problema para pegar o sinal do SleepConditionVariableCS. Error code: "
						+ std::to_string(error), CL_FILE_LOG_AND_CONSOLE));

				// Não está preparado para escrever
				return false;
			}
		}
#elif defined(__linux__)

		int32_t error = 0;

		if ((error = pthread_cond_wait(&cv_send, &cs)) != 0) {

			_smp::message_pool::getInstance().push(new message("[session::buff_ctx::isSendable][Error] problema para pegar o sinal do pthread_cond_wait. Error code: "
						+ std::to_string(error), CL_FILE_LOG_AND_CONSOLE));

			// Não está preparado para escrever
			return false;
		}
#endif
	}

	return true;
}

bool session::buff_ctx::readyToSend() {
	
	while (!state_wr_send && state_write) {

#if defined(_WIN32)
		if (SleepConditionVariableCS(&cv_write, &cs, INFINITE) == 0) {

			DWORD error = GetLastError();

			if (error == ERROR_TIMEOUT) {

				_smp::message_pool::getInstance().push(new message("[session::buff_ctx::readyToSend][Error] Time out no SleepConditionVariableCS, \
								mas o tempo de timeout era INFINITE, error desconhecido.", CL_FILE_LOG_AND_CONSOLE));

				// Não está preparado para escrever
				return false;

			}else {	// outro tipo de erro

				_smp::message_pool::getInstance().push(new message("[session::buff_ctx::readyToSend][Error] problema para pegar o sinal do SleepConditionVariableCS. Error code: "
						+ std::to_string(error), CL_FILE_LOG_AND_CONSOLE));

				// Não está preparado para escrever
				return false;
			}
		}
#elif defined(__linux__)

		int32_t error = 0;

		if ((error = pthread_cond_wait(&cv_write, &cs)) != 0) {

			_smp::message_pool::getInstance().push(new message("[session::buff_ctx::readyToSend][Error] problema para pegar o sinal do pthread_cond_wait. Error code: "
						+ std::to_string(error), CL_FILE_LOG_AND_CONSOLE));

			// Não está preparado para escrever
			return false;
		}
#endif
	}

	return true;
}

bool session::buff_ctx::isSetedToSend() {
	return state_send;
}

bool session::buff_ctx::isSetedToWrite() {
	return state_write;
}

bool session::buff_ctx::isSetedToPartial() {
	return state_wr_send;
}

bool session::buff_ctx::isSetedToSendOrPartialSend() {
	return state_send || state_wr_send;
}

void session::buff_ctx::setWrite() {
	state_write = 1u;
}

void session::buff_ctx::setSend() {
	state_send = 1u;
}

void session::buff_ctx::setPartialSend() {
	state_wr_send = 1u;
}

void session::buff_ctx::releaseWrite() {

	state_write = 0u;

#if defined(_WIN32)
	WakeConditionVariable(&cv_write);
#elif defined(__linux__)
	pthread_cond_signal(&cv_write);
#endif
}

void session::buff_ctx::releaseSend() {

	state_send = 0u;

#if defined(_WIN32)
	WakeAllConditionVariable(&cv_send);
#elif defined(__linux__)
	pthread_cond_broadcast(&cv_send);
#endif
}

void session::buff_ctx::releasePartial() {

	state_wr_send = 0u;

#if defined(_WIN32)
	WakeAllConditionVariable(&cv_send);
#elif defined(__linux__)
	pthread_cond_broadcast(&cv_send);
#endif
}

void session::buff_ctx::releaseSendAndPartialSend() {
	
	if (state_wr_send && buff.getUsed() <= 0)
		state_wr_send = 0u;

	state_send = 0u;

#if defined(_WIN32)
	WakeAllConditionVariable(&cv_send);
#elif defined(__linux__)
	pthread_cond_broadcast(&cv_send);
#endif
}

int64_t session::buff_ctx::increseRequestSendCount() {
	return ++request_send_count;
}

int64_t session::buff_ctx::decreaseRequestSendCount() {
	return --request_send_count;
}


