// Arquivo client.h
// Criado em 19/12/2017 por Acrisio
// Definição da classe client

#pragma once
#ifndef _STDA_CLIENT_H
#define _STDA_CLIENT_H

#include "../THREAD POOL/threadpl_client.h"
#include "../SOCKET/session_manager.hpp"

namespace stdA {
    class client : public threadpl_client {
		public:
			enum CLIENT_STATE : unsigned char {
				UNINITIALIZED,
				INITIALIZED,
				GOOD,
				FAILURE,
			};

        public:
            client(session_manager& _session_manager);
            virtual ~client();
        
			static DWORD WINAPI CALLBACK _monitor(LPVOID lpParameter);

			static DWORD WINAPI CALLBACK _send_msg_lobby(LPVOID lpParameter);

			virtual void waitAllThreadFinish(DWORD dwMilleseconds) override;

			virtual void DisconnectSession(session *_session) = 0;

			virtual void start();

			virtual void checkClienteOnline() = 0;

		protected:
			virtual std::string getSessionID(session *_session) = 0;

		protected:
			virtual DWORD monitor() override;
			virtual DWORD send_msg_lobby() = 0;

			virtual void dispach_packet_same_thread(session& _session, packet *_packet) override;
			virtual void dispach_packet_sv_same_thread(session& _session, packet *_packet) override;

		protected:
			virtual void commandScan() = 0;

		protected:
			session_manager& m_session_manager;

			unsigned char m_state;

		protected:
			thread* m_thread_monitor;

		protected:
			int32_t volatile m_continue_monitor;
			int32_t volatile m_continue_send_msg;
            
    };
}

#endif