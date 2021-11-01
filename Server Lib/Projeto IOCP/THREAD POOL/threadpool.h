// Arquivo threadpool.h
// Criado em 21/05/2017 por Acrisio
// Definição da classe threadpool

#pragma once
#ifndef _STDA_THREADPOOL_H
#define _STDA_THREADPOOL_H

#if defined(_WIN32)
#include <Windows.h>
#include <WinSock2.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include <pthread.h>
#endif

#include "threadpool_base.hpp"
#include "../TYPE/stdAType.h"
#include "thread.h"
#include "job_pool.h"

#if defined(_WIN32)
#include "../IOCP/iocp.h"
#elif defined(__linux__)
#include "../IOCP/epoll.hpp"
#include "../TYPE/threadpool_linux_type.h"
#endif

#include "../UTIL/buffer.h"
#include "../SOCKET/session.h"
#include "../TIMER/timer_manager.h"

// LOOP TRANSLATE BUFFER TO PACKET

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

#define LOOP_TRANSLATE_BUFFER_TO_PACKET_SERVER(_ph, _sz_ph, STDA_OT) \
					BEGIN_LOOP_TRANSLATE_BUFFER_TO_PACKET((_ph), (_sz_ph), GET_FUNC_SEND, SET_FUNC_SEND); \
						\
					_packet->unMakeFull(_session->m_key); \
						\
					MED_LOOP_TRANSLATE_BUFFER_TO_PACKET((_ph), (_sz_ph), (STDA_OT), GET_FUNC_SEND, SET_FUNC_SEND); \
						\
					_packet->unMakeFull(_session->m_key); \
						\
					END_LOOP_TRANSLATE_BUFFER_TO_PACKET((_ph), (_sz_ph), (STDA_OT), GET_FUNC_SEND, SET_FUNC_SEND); \

#define LOOP_TRANSLATE_BUFFER_TO_PACKET_CLIENT(_phc, _sz_phc, STDA_OT) \
					BEGIN_LOOP_TRANSLATE_BUFFER_TO_PACKET((_phc), (_sz_phc), GET_FUNC_RECV, SET_FUNC_RECV); \
						\
					_packet->unMake(_session->m_key); \
						\
					MED_LOOP_TRANSLATE_BUFFER_TO_PACKET((_phc), (_sz_phc), (STDA_OT), GET_FUNC_RECV, SET_FUNC_RECV); \
						\
					_packet->unMake(_session->m_key); \
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

namespace stdA {
	/*enum TT {	// TYPE THREAD
		TT_WORKER_IO,
		TT_WORKER_IO_SEND,
		TT_WORKER_IO_RECV,
		TT_WORKER_LOGICAL,
		TT_WORKER_SEND,
		TT_CONSOLE,
		TT_ACCEPT,
		TT_ACCEPTEX,
		TT_ACCEPTEX_IO,
		TT_RECV,
		TT_SEND,
		TT_JOB,
		TT_DB_NORMAL,
		TT_MONITOR,
		TT_SEND_MSG_TO_LOBBY
	};

	enum {	// Operation Type
		STDA_OT_SEND_RAW_REQUEST,
		STDA_OT_SEND_RAW_COMPLETED,
		STDA_OT_RECV_REQUEST,
		STDA_OT_RECV_COMPLETED,
		STDA_OT_SEND_REQUEST,
		STDA_OT_SEND_COMPLETED,
		STDA_OT_DISPACH_PACKET_SERVER,
		STDA_OT_DISPACH_PACKET_CLIENT,
		STDA_OT_ACCEPT_COMPLETED,
	};*/

    class threadpool : threadpool_base {
		public:
			struct threadpool_and_index {
				threadpool* m_threadpool;
				DWORD m_index;
			};

        public:
			threadpool(size_t _num_thread_workers_io, size_t _num_thread_workers_logical, uint32_t _job_thread_num = 4);
            virtual ~threadpool();

#if defined(_WIN32)
			static DWORD WINAPI CALLBACK _worker_io(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK _worker_io_accept(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK _worker_io_send(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK _worker_io_recv(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK _worker_logical(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK _worker_send(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK _console(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK __job(LPVOID lpParameter);
#elif defined(__linux__)
			static void* _worker_io(LPVOID lpParameter);
			static void* _worker_io_accept(LPVOID lpParameter);
			static void* _worker_io_send(LPVOID lpParameter);
			static void* _worker_io_recv(LPVOID lpParameter);
			static void* _worker_logical(LPVOID lpParameter);
			static void* _worker_send(LPVOID lpParameter);
			static void* _console(LPVOID lpParameter);
			static void* __job(LPVOID lpParameter);
#endif

			virtual void waitAllThreadFinish(DWORD dwMilleseconds) = 0;

			virtual void postIoOperation(session *_session, Buffer* lpBuffer, DWORD dwIOsize, DWORD operation) override;

			void postJob(job *_job);

			virtual timer *postTimer(DWORD _time, timer::timer_param *_arg, uint32_t _tipo = timer::NORMAL);
			virtual timer *postTimer(DWORD _time, timer::timer_param *_arg, std::vector< DWORD > _table_interval, uint32_t _tipo = timer::PERIODIC);
			virtual void removeTimer(timer *_timer);

			job_pool& getJobPool();

		protected:
#if defined(_WIN32)
			DWORD worker_io(DWORD _index);
			DWORD worker_io_accept();
			DWORD worker_io_send(DWORD _index);
			DWORD worker_io_recv(DWORD _index);
			DWORD worker_logical(DWORD _index);
			DWORD worker_send(DWORD _index);
			DWORD console();
			DWORD _job();
#elif defined(__linux__)
			void* worker_io(DWORD _index);
			void* worker_io_accept();
			void* worker_io_send(DWORD _index);
			void* worker_io_recv(DWORD _index);
			void* worker_logical(DWORD _index);
			void* worker_send(DWORD _index);
			void* console();
			void* _job();
#endif

#if defined(_WIN32)
			virtual DWORD monitor() = 0;
#elif defined(__linux__)
			virtual void* monitor() = 0;
#endif

			virtual void dispach_packet_same_thread(session& _session, packet *_packet) = 0;
			virtual void dispach_packet_sv_same_thread(session& _session, packet *_packet) = 0;

			void translate_operation(session *_session, DWORD dwIOsize, Buffer *lpBuffer, DWORD _operation);

			virtual void accept_completed(SOCKET *_listener, DWORD dwIOsize, myOver *lpBuffer, DWORD _operation) = 0;

			void send_new(session *_session, Buffer *lpBuffer, DWORD operation);
			void recv_new(session *_session, Buffer *lpBuffer, DWORD operation);
			virtual void translate_packet(session *_session, Buffer *lpBuffer, DWORD dwIOsize, DWORD operation) = 0;

			// Post on iocp Work Thread Pool Logical
			void postIoOperationL(session *_session, Buffer* lpBuffer, DWORD operation);

			// Post on iocp Work Thread Pool Send
			void postIoOperationS(session *_session, Buffer* lpBuffer, DWORD dwIOsize, DWORD operation);

			// Post on iocp Work Thread Pool Recv
			void postIoOperationR(session *_session, Buffer* lpBuffer, DWORD dwIOsize, DWORD operation);

        protected:
            std::vector< thread* > m_threads;
			thread* m_thread_console;
			job_pool m_job_pool;
			timer_manager m_timer_mgr;
			iocp m_iocp_io_accept;
			iocp m_iocp_io[16];

#if defined(_WIN32)
			iocp m_iocp_io_send[16];
			iocp m_iocp_io_recv[16];
			iocp m_iocp_logical;
			iocp m_iocp_send;
#elif defined(__linux__)
			list_fifo_asyc< stThreadpoolMessage > m_iocp_io_send[16];
			list_fifo_asyc< stThreadpoolMessage > m_iocp_io_recv[16];
			list_fifo_asyc< stThreadpoolMessage > m_iocp_logical;
			list_fifo_asyc< stThreadpoolMessage > m_iocp_send;
#endif

		private:
			uint64_t m_transfer_bytes_io;
			clock_t m_1sec;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
    };
}

#endif