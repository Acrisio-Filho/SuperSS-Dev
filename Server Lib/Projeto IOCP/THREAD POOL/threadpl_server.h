// Arquivo threadpl_server.h
// Criado em 03/12/2017 por Acrisio
// Definição da classe threadpl_server

#pragma once
#ifndef _STDA_THREADPL_SERVER_H
#define _STDA_THREADPL_SERVER_H

#include "threadpool.h"

#define STDA_THREADPL_SERVER_ERROR_TYPE	55

namespace stdA {

    class threadpl_server : public threadpool {
        public:
            threadpl_server(size_t _num_thread_workers_io, size_t _num_thread_workers_logical, uint32_t _job_thread_num);
            virtual ~threadpl_server();

            virtual void waitAllThreadFinish(DWORD dwMilleseconds) = 0;

			virtual bool DisconnectSession(session *_session) = 0;

        protected:
#if defined(_WIN32)
            virtual DWORD accept() = 0;
			virtual DWORD acceptEx() = 0;
			virtual DWORD monitor() = 0;
			virtual DWORD disconnect_session() = 0;
#elif defined(__linux__)
            virtual void* accept() = 0;
			virtual void* acceptEx() = 0;
			virtual void* monitor() = 0;
			virtual void* disconnect_session() = 0;
#endif

			virtual void dispach_packet_same_thread(session& _session, packet *_packet) = 0;
			virtual void dispach_packet_sv_same_thread(session& _session, packet *_packet) = 0;

            void translate_packet(session *_session, Buffer *lpBuffer, DWORD dwIOsize, DWORD operation) override;

			virtual void accept_completed(SOCKET *_listener, DWORD dwIOsize, myOver *lpBuffer, DWORD _operation) = 0;
    };
}

#endif