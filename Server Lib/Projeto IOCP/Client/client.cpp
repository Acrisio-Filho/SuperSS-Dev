// Arquivo client.cpp
// Criado em 19/12/2017 por Acrisio
// Implementação da classe client

// Tem que ter o pack aqui se não da erro na hora da allocação do HEAP
#if defined(_WIN32)
#pragma pack(1)
#endif

#include <WinSock2.h>
#include "client.h"
#include "../SOCKET/socket.h"
#include "../UTIL/exception.h"
#include "../UTIL/message_pool.h"
#include "../UTIL/hex_util.h"
#include "../TIMER/timer.h"

#include "../PACKET/packet_func.h"

#include <DbgHelp.h>

#define CHECK_SESSION_BEGIN(method) if (!_session.getState()) \
										throw exception("[client::" + std::string((method)) +"][Error] player nao esta connectado.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::CLIENT, 1, 0)); \

using namespace stdA;

client::client(session_manager& _session_manager) : m_session_manager(_session_manager), threadpl_client(16, 16), m_state(UNINITIALIZED) {

	size_t i = 0;

	try {
		InterlockedExchange(&m_continue_monitor, 1);
		InterlockedExchange(&m_continue_send_msg, 1);

		// Send Message to Lobby
		//m_threads.push_back(new thread(TT_SEND_MSG_TO_LOBBY, client::_send_msg_lobby, this));

		// Monitor Thread
		m_thread_monitor = new thread(TT_MONITOR, client::_monitor, (LPVOID)this, THREAD_PRIORITY_NORMAL);

		m_state = INITIALIZED;
	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[client::client][Error] client inicializado com error: " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		m_state = FAILURE;
	}
};

client::~client() {

	if (m_thread_monitor != nullptr)
		delete m_thread_monitor;

	m_state = UNINITIALIZED;
};

DWORD client::_monitor(LPVOID lpParameter) {
	BEGIN_THREAD_SETUP(client)

	pTP->monitor();

	END_THREAD_SETUP("_monitor()");
};

DWORD client::_send_msg_lobby(LPVOID lpParameter) {
	BEGIN_THREAD_SETUP(client)

	pTP->send_msg_lobby();

	END_THREAD_SETUP("_send_msg_lobby()");
};

DWORD client::monitor() {

	try {
		_smp::message_pool::getInstance().push(new message("monitor iniciado com sucesso!"));

		while (InterlockedCompareExchange(&m_continue_monitor, 1, 1)) {
			try {
				if (m_thread_console != nullptr && !m_thread_console->isLive())
					m_thread_console->init_thread();

				for (size_t i = 0; i < m_threads.size(); i++)
					if (m_threads[i] != nullptr && !m_threads[i]->isLive())
						m_threads[i]->init_thread();

				checkClienteOnline();

				Sleep(1000); // 1 second para próxima verificação
			}catch (exception& e) {
				if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) != STDA_ERROR_TYPE::THREAD)
					throw;
			}
		}
	}catch (exception& e) {
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}catch (...) {
		std::cout << "monitor() -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n";
	}

	_smp::message_pool::getInstance().push(new message("Saindo de monitor()..."));

	return 0u;
};

inline void client::dispach_packet_same_thread(session& _session, packet *_packet) {
	CHECK_SESSION_BEGIN("dispach_packet_same_thread");

	//ParamWorkerC pw = { *this, m_iocp_io, m_job_pool, m_session_pool, (cliente*)_packet->getSession()->m_client, _packet };
	func_arr::func_arr_ex *func = nullptr;

	try {
		func = packet_func_base::funcs.getPacketCall(_packet->getTipo());
	}catch (exception& e) {
		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::FUNC_ARR/*Packet_func Erro, Warning e etc*/) {
			_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			_smp::message_pool::getInstance().push(new message("size packet: " + std::to_string(_packet->getSize()) + "\n" + hex_util::BufferToHexString(_packet->getBuffer(), _packet->getSize()), CL_FILE_LOG_AND_CONSOLE));

			// Trata o erro aqui
			// e desloga a session que enviou pacote errado
			/// colocar de novo ------> m_pe->_session_pool.deleteSession(_packet->getSession());
			// se for muito grave relança ele para terminar a thread
		}else
			throw;
	}

	//pw.p = _packet;

	ParamDispatch _pd = { *(player*)&_session, _packet };

	if (func != nullptr && func->execCmd(&_pd) != 0)
		_smp::message_pool::getInstance().push(new message("Erro ao tratar o pacote. ID: " + std::to_string(_pd._packet->getTipo()) + "(0x" + hex_util::ltoaToHex(_pd._packet->getTipo()) + "). threadpool::dispach_packet_same_thread()", CL_FILE_LOG_AND_CONSOLE));
};

inline void client::dispach_packet_sv_same_thread(session& _session, packet *_packet) {
	CHECK_SESSION_BEGIN("dispach_packet_sv_same_thread");
	
	//ParamWorkerC pw = { *this, m_iocp_io, m_job_pool, m_session_pool, (cliente*)_packet->getSession()->m_client, _packet };
	func_arr::func_arr_ex *func = nullptr;

	try {
		func = packet_func_base::funcs_sv.getPacketCall(_packet->getTipo());
	}catch (exception& e) {
		if (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::FUNC_ARR/*Packet_func Erro, Warning e etc*/) {
			_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
			//_smp::message_pool.push(new message("size packet: " + std::to_string(_pd._packet->getSize()) + "\n" + hex_util::BufferToHexString(_pd._packet->getBuffer(), _pd._packet->getSize()), CL_FILE_LOG_AND_CONSOLE));

			// Trata o erro aqui
			// e desloga a session que enviou pacote errado
			/// colocar de novo ------> m_pe->_session_pool.deleteSession(_packet->getSession());
			// se for muito grave relança ele para terminar a thread
		}else
			throw;
	}

	//pw.p = _packet;

	ParamDispatch _pd = { *(player*)&_session, _packet };

	if (func != nullptr && func->execCmd(&_pd) != 0)
		_smp::message_pool::getInstance().push(new message("Erro ao tratar o pacote. ID: " + std::to_string(_pd._packet->getTipo()) + "(0x" + hex_util::ltoaToHex(_pd._packet->getTipo()) + "). threadpool::dispach_packet_sv_same_thread()", CL_FILE_LOG_AND_CONSOLE));
};

void client::waitAllThreadFinish(DWORD dwMilleseconds) {
	// Monitor Thread
	InterlockedDecrement(&m_continue_monitor);
	InterlockedDecrement(&m_continue_send_msg);

	if (m_thread_monitor != nullptr)
		m_thread_monitor->waitThreadFinish(dwMilleseconds);

	for (size_t i = 0; i < m_threads.size(); i++) {

		switch (m_threads[i]->getTipo()) {
		case TT_WORKER_IO:
			for (auto io = 0u; io < (sizeof(m_iocp_io) / sizeof(iocp)); ++io)
				m_iocp_io[io].postStatus((ULONG_PTR)nullptr, 0, nullptr);
			break;
		case TT_WORKER_IO_SEND:
			for (auto io = 0u; io < (sizeof(m_iocp_io_send) / sizeof(iocp)); ++io)
				m_iocp_io_send[io].postStatus((ULONG_PTR)nullptr, 0, nullptr);
			break;
		case TT_WORKER_IO_RECV:
			for (auto io = 0u; io < (sizeof(m_iocp_io_recv) / sizeof(iocp)); ++io)
				m_iocp_io_recv[io].postStatus((ULONG_PTR)nullptr, 0, nullptr);
			break;
		case TT_WORKER_LOGICAL:
			//for (auto io = 0u; io < (sizeof(m_iocp_logical) / sizeof(iocp)); ++io)
				m_iocp_logical.postStatus((ULONG_PTR)nullptr, 0, nullptr);			// Esse tinha Index[io], era um array de 16 iocp
			break;
		case TT_WORKER_SEND:
			//for (auto io = 0u; io < (sizeof(m_iocp_send) / sizeof(iocp)); ++io)
				m_iocp_send.postStatus((ULONG_PTR)nullptr, 0, nullptr);			// Esse tinha Index[io], era um array de 16 iocp
			break;
		case TT_JOB:
			m_job_pool.push(nullptr);			// Sai do job
			break;
		case TT_CONSOLE:
			_smp::message_pool::getInstance().push(nullptr);	// Sai do console
			break;
		case TT_MONITOR:
			InterlockedDecrement(&m_continue_monitor);
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

void client::start() {

	if (m_state != FAILURE) {
		try {
			commandScan();

			_smp::message_pool::getInstance().push(new message("Saindo..."));

			waitAllThreadFinish(INFINITE);
		}catch (exception& e) {
			_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		}catch (std::exception& e) {
			_smp::message_pool::getInstance().push(new message(e.what(), CL_FILE_LOG_AND_CONSOLE));
		}catch (...) {
			_smp::message_pool::getInstance().push(new message("Error desconhecido. client::start()", CL_FILE_LOG_AND_CONSOLE));
		}
	}else {

		_smp::message_pool::getInstance().push(new message("Cliente inicializado com falha.", CL_FILE_LOG_AND_CONSOLE));

		waitAllThreadFinish(INFINITE);
	}
};
