// Arquivo threadpool_base.hpp
// Criado em 02/12/2018 as 15:47 por Acrisio
// Definição da classe base threadpool_base

#pragma once
#ifndef _STDA_THREADPOOL_BASE_HPP
#define _STDA_THREADPOOL_BASE_HPP

#include "../SOCKET/session.h"

#if defined(_WIN32)
#include "../TYPE/set_se_exception_with_mini_dump.h"
#elif defined(__linux__)
	#include <signal.h>
#endif

#if defined(_WIN32)
#define BEGIN_THREAD_SETUP(_type_class) DWORD result = 0; \
						   STDA_SET_SE_EXCEPTION \
						   try { \
								_type_class *pTP = reinterpret_cast<_type_class*>(lpParameter); \
								if (pTP) { 
#elif defined(__linux__)
#define BEGIN_THREAD_SETUP(_type_class) void* result = (void*)0; \
						   try { \
								auto sig_ign_st = (struct sigaction){SIG_IGN}; \
						   		sigaction(SIGPIPE, &sig_ign_st, nullptr); \
								_type_class *pTP = reinterpret_cast<_type_class*>(lpParameter); \
								if (pTP) { 
#endif

#define END_THREAD_SETUP(name_thread)	  } \
							}catch (exception& e) { \
								_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE)); \
							}catch (std::exception& e) { \
								_smp::message_pool::getInstance().push(new message(e.what(), CL_FILE_LOG_AND_CONSOLE)); \
							}catch (...) { \
								_smp::message_pool::getInstance().push(new message(std::string((name_thread)) + " -> Exception (...) c++ nao tratada ou uma excessao de C(nullptr e etc)\n", CL_FILE_LOG_AND_CONSOLE)); \
							} \
							_smp::message_pool::getInstance().push(new message("Saindo do trabalho->" + std::string((name_thread)))); \
						return result; \

namespace stdA {

    enum TT {	// TYPE THREAD
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
		TT_SEND_MSG_TO_LOBBY,
		TT_SCAN_CMD,
		TT_DISCONNECT_SESSION,
		TT_REGISTER_SERVER,
		TT_COIN_CUBE_LOCATION_TRANSLATE,
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
	};

    class threadpool_base {
        public:
           virtual void postIoOperation(session *_session, Buffer* lpBuffer, DWORD dwIOsize, DWORD operation) = 0;
    };
}

#endif // !_STDA_THREADPOOL_BASE_HPP