// Arquivo unit.hpp
// Criado em 02/12/2018 as 12:28 por Acrisio
// Definição da classe unit

#pragma once
#ifndef _STDA_UNIT_HPP
#define _STDA_UNIT_HPP

#include <string>
#include <vector>
#include "../THREAD POOL/threadpl_unit.hpp"
#include "../PACKET/packet_func.h"
#include "../TYPE/stdAType.h"
#include "../SOCKET/session_manager.hpp"

#include "../PANGYA_DB/cmd_server_list.hpp"

#if defined(_WIN32)
#include <MSWSock.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include "../UTIL/event.hpp"
#endif

#include "../SOCKET/socket.h"

#include "../UTIL/reader_ini.hpp"

#ifndef _INI_PATH
	#if defined(_WIN32)
		#define _INI_PATH "\\server.ini"
	#elif defined(__linux__)
		#define _INI_PATH "/server.ini"
	#endif
#endif // _INI_PATH

#include "../DATABASE/normal_manager_db.hpp"

namespace stdA {
    class unit : public threadpl_unit {
        public:
			enum STATE : unsigned char {
				UNINITIALIZED,
				GOOD,
				GOOD_WITH_WARNING,
				FAILURE,
			};

        public:
            unit(session_manager& _session_manager, uint32_t _db_instance_num, uint32_t _job_thread_num);
            virtual ~unit();

#if defined(_WIN32)
			static DWORD WINAPI CALLBACK _accept(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK _monitor(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK _disconnect_session(LPVOID lpParameter);
#elif defined(__linux__)
			static void* _accept(LPVOID lpParameter);
			static void* _monitor(LPVOID lpParameter);
			static void* _disconnect_session(LPVOID lpParameter);
#endif

			virtual void waitAllThreadFinish(DWORD dwMilleseconds) override;

			virtual void start();

			virtual uint32_t getUID();

			virtual bool DisconnectSession(session *_session) override;

        protected:
#if defined(_WIN32)
			virtual DWORD monitor() override;
			virtual DWORD accept() override;
			virtual DWORD disconnect_session() override;
#elif defined(__linux__)
			virtual void* monitor() override;
			virtual void* accept() override;
			virtual void* disconnect_session() override;
#endif

			virtual void dispach_packet_same_thread(session& _session, packet *_packet) override;
			virtual void dispach_packet_sv_same_thread(session& _session, packet *_packet) override;

			virtual void accept_completed(SOCKET *_listener, DWORD dwIOsize, myOver *lpBuffer, DWORD _operation) override;

			virtual void onAcceptCompleted(session *_session) = 0;
			virtual void onDisconnected(session *_session) = 0;

			virtual void onHeartBeat() = 0;

			virtual void onStart() = 0;

			virtual bool checkCommand(std::stringstream& _command) = 0;

			virtual bool checkPacket(session& _session, packet *_packet) = 0;

			virtual void cmdUpdateServerList();
			virtual void updateServerList(std::vector< ServerInfo >& _v_si);

			virtual void shutdown_time(int32_t _time_sec) = 0;

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

			static int end_time_shutdown(void* _arg1, void* _arg2);

		protected:
			timer *m_shutdown;
        
        protected:
            ServerInfoEx m_si;
			std::vector< ServerInfo > m_server_list;

			session_manager& m_session_manager;

			ReaderIni m_reader_ini;

			STATE m_state;

			ctx_db m_ctx_db;

		protected:
#if defined(_WIN32)
			HANDLE EventShutdownServer;
			HANDLE EventAcceptConnection;
#elif defined(__linux__)
			Event *EventShutdownServer;
			Event *EventAcceptConnection;
#endif

			void shutdown();

			void setAcceptConnection();

		protected:
			thread *m_thread_monitor;

#if defined(_WIN32)
			LONG volatile m_continue_monitor;
			LONG volatile m_atomic_disconnect_session;
#elif defined(__linux__)
			int32_t volatile m_continue_monitor;
			int32_t volatile m_atomic_disconnect_session;
#endif

		protected:
			virtual void commandScan();

			virtual void config_init();

		protected:
#if defined(_WIN32)
			LONG volatile m_continue_accpet;
#elif defined(__linux__)
			int32_t volatile m_continue_accpet;
#endif
			
			// ponteiro para o socket que foi criado no accept
			socket* m_accept_sock;		// socket que controla os checks das conexão se pode ou não logar
    };
}

#endif // !_STDA_UNIT_HPP