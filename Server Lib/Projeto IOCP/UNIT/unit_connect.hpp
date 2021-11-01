// Arquivo unit_connect.hpp
// Criado em 02/12/2018 as 14:28 por Acrisio
// Definição da classe unit_connect

#pragma once
#ifndef _STDA_UNIT_CONNECT_HPP
#define _STDA_UNIT_CONNECT_HPP

#if defined(_WIN32)
#include <Windows.h>
#include <WinSock2.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include <sys/socket.h>
#include <pthread.h>
#include "../UTIL/event.hpp"
#include "../TYPE/list_fifo.h"
#endif

#include "../TYPE/stdAType.h"
#include "../THREAD POOL/thread.h"

#if defined(_WIN32)
#include "../IOCP/iocp.h"
#elif defined(__linux__)
#include "../IOCP/epoll.hpp"
#include "../TYPE/threadpool_linux_type.h"
#endif

#include "../UTIL/buffer.h"
#include "../SOCKET/session.h"
#include "../TIMER/timer_manager.h"
#include "../THREAD POOL/threadpool_base.hpp"

#include "../UTIL/func_arr.h"

#include "../TYPE/pangya_st.h"

#include "../UTIL/reader_ini.hpp"

#ifndef _INI_PATH
	#if defined(_WIN32)
		#define _INI_PATH "\\server.ini"
	#elif defined(__linux__)
		#define _INI_PATH "/server.ini"
	#endif
#endif // _INI_PATH

namespace stdA {
	class UnitPlayer : public session {
		public:
			struct player_info {
				player_info(uint32_t _ul = 0u) {
					clear();
				};
				void clear() { memset(this, 0, sizeof(player_info)); };
				char nickname[22];
				char id[22];
				uint32_t uid;
				uint32_t tipo;
				unsigned char m_state;
			};

		public:
			UnitPlayer(threadpool_base& _threadpool, ServerInfoEx& _si);
			virtual ~UnitPlayer();

			virtual bool clear();

			virtual unsigned char getStateLogged() override;

			virtual uint32_t getUID() override;
			virtual uint32_t getCapability() override;
			virtual char* getNickname() override;
			virtual char* getID() override;

		public:
			ServerInfoEx& m_si;
			player_info m_pi;
	};

	struct ParamDispatchAS {
		UnitPlayer& _session;
		packet *_packet;
	};

    class unit_connect_base : public threadpool_base {
        public:
			enum STATE : unsigned char {
				UNINITIALIZED,
				GOOD,
				GOOD_WITH_WARNING,
				INITIALIZED,
				FAILURE,
			};

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
            };

			struct stUnitCtx {
				stUnitCtx(uint32_t _ul = 0u) {
					clear();
				};
				void clear() {
					
					if (!ip.empty()) {
						ip.clear();
						ip.shrink_to_fit();
					}

					port = 0u;

					state = false;
				};
				bool state;
				std::string ip;
				uint32_t port;
			};

        public:
            unit_connect_base(ServerInfoEx& _si);
            virtual ~unit_connect_base();

#if defined(_WIN32)
			static DWORD WINAPI CALLBACK _worker_io(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK _worker_logical(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK _worker_send(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK _monitor(LPVOID lpParameter);
#elif defined(__linux__)
			static void* _worker_io(LPVOID lpParameter);
			static void* _worker_logical(LPVOID lpParameter);
			static void* _worker_send(LPVOID lpParameter);
			static void* _monitor(LPVOID lpParameter);
#endif

			virtual void waitAllThreadFinish(DWORD dwMilleseconds);

			virtual void postIoOperation(session *_session, Buffer* lpBuffer, DWORD dwIOsize, DWORD operation) override;

			virtual void DisconnectSession(session *_session);

			virtual void start();

			// Está conectado com o Auth Server
			virtual bool isLive();

		protected:
#if defined(_WIN32)
			DWORD worker_io();
			DWORD worker_logical();
			DWORD worker_send();
			virtual DWORD monitor();
#elif defined(__linux__)
			void* worker_io();
			void* worker_logical();
			void* worker_send();
			virtual void* monitor();
#endif

			virtual void dispach_packet_same_thread(session& _session, packet *_packet);
			virtual void dispach_packet_sv_same_thread(session& _session, packet *_packet);

			void translate_operation(session *_session, DWORD dwIOsize, Buffer *lpBuffer, DWORD _operation);

			void send_new(session *_session, Buffer *lpBuffer, DWORD operation);
			void recv_new(session *_session, Buffer *lpBuffer, DWORD operation);
			virtual void translate_packet(session *_session, Buffer *lpBuffer, DWORD dwIOsize, DWORD operation);

			// Post on iocp Work Thread Pool Logical
			void postIoOperationL(session *_session, Buffer* lpBuffer, DWORD operation);

			virtual void onHeartBeat() = 0;

			virtual void onConnected() = 0;

			virtual void onDisconnect() = 0;

			virtual void config_init() = 0;

		protected:
			virtual void ConnectAndAssoc();

#if defined(_WIN32)
			HANDLE hEventTryConnect;
#elif defined(__linux__)
			Event *hEventTryConnect;
#endif

        protected:
            std::vector< thread* > m_threads;
			iocp m_iocp_io;
#if defined(_WIN32)
			iocp m_iocp_logical;
			iocp m_iocp_send;
#elif defined(__linux__)
			list_fifo_asyc< stThreadpoolMessage > m_iocp_logical;
			list_fifo_asyc< stThreadpoolMessage > m_iocp_send;
#endif

			STATE m_state;

		protected:
			class packet_func_as {
				public:
					static void session_send(packet& p, UnitPlayer *s, unsigned char _debug);
					static void session_send(std::vector< packet* > v_p, UnitPlayer *s, unsigned char _debug);
			};

			func_arr funcs;
			func_arr funcs_sv;

			UnitPlayer m_session;

		protected:
			thread *m_thread_monitor;

#if defined(_WIN32)
			HANDLE hEventContinueMonitor;
#elif defined(__linux__)
			Event *hEventContinueMonitor;
#endif

		protected:
			stUnitCtx m_unit_ctx;
			ReaderIni m_reader_ini;

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

#endif // !_STDA_UNIT_CONNECT_HPP