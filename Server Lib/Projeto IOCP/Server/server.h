// server.h
// Criado em 17/12/2017 por Acrisio
// Dfinição da classe server

#pragma once
#ifndef _STDA_SERVER_H
#define _STDA_SERVER_H

#include <string>
#include <vector>
#include "../THREAD POOL/threadpl_server.h"
//#include "../DATABASE/pangya_db.h"
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

#include "../UNIT/unit_auth_server_connect.hpp"
#include "../UNIT/unit_auth_interface.hpp"

namespace stdA {
    class server : public threadpl_server, public IUnitAuthServer {
		public:
			enum STATE : unsigned char {
				UNINITIALIZED,
				GOOD,
				GOOD_WITH_WARNING,
				INITIALIZED,
				FAILURE,
			};

        public:
            server(session_manager& _session_manager, uint32_t _accept_thread_num, uint32_t _db_instance_num, uint32_t _job_thread_num);
            virtual ~server();

#if defined(_WIN32)
			static DWORD WINAPI CALLBACK _accept(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK _acceptEx(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK _monitor(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK _registerServer(LPVOID lpParameter);
			static DWORD WINAPI CALLBACK _disconnect_session(LPVOID lpParameter);
#elif defined(__linux__)
			static void* _accept(LPVOID lpParameter);
			static void* _acceptEx(LPVOID lpParameter);
			static void* _monitor(LPVOID lpParameter);
			static void* _registerServer(LPVOID lpParameter);
			static void* _disconnect_session(LPVOID lpParameter);
#endif

			virtual void waitAllThreadFinish(DWORD dwMilleseconds) override;

			virtual void start();

			virtual uint32_t getUID();

			virtual session* findSession(uint32_t _uid, bool _oid = false);

			virtual bool DisconnectSession(session *_session) override;

			// Time To Live load from ini file config
			virtual uint32_t getBotTTL();

        protected:
#if defined(_WIN32)
			virtual DWORD monitor() override;
			virtual DWORD accept() override;
			virtual DWORD acceptEx() override;
			virtual DWORD disconnect_session() override;
			virtual DWORD registerServer();
#elif defined(__linux__)
			virtual void* monitor() override;
			virtual void* accept() override;
			virtual void* acceptEx() override;
			virtual void* disconnect_session() override;
			virtual void* registerServer();
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

			virtual void init_option_accepted_socket(SOCKET _accepted);

			// Shutdown With Time
			virtual void shutdown_time(int32_t _time_sec) = 0;

			virtual void cmdUpdateListBlock_IP_MAC();

			virtual bool haveBanList(std::string _ip_address, std::string _mac_address, bool _check_mac = true);
			virtual session* HasLoggedWithOuterSocket(session& _session);

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

			static int end_time_shutdown(void* _arg1, void* _arg2);
        
        protected:
            ServerInfoEx m_si;
			std::vector< ServerInfo > m_server_list;

			session_manager& m_session_manager;

			ReaderIni m_reader_ini;

			STATE m_state;

			ctx_db m_ctx_db;

			uint32_t m_Bot_TTL;	// Anti Bot time to live

			std::vector< IPBan > v_ip_ban_list;
			std::vector< std::string > v_mac_ban_list;

		// Auth Server Membros protegidos
		protected:
			unit_auth_server_connect *m_unit_connect;		// Ponteiro Connecta com o Auth Server

			void destroy_unit();

		// Auth Server Comandos
		public:
			virtual void authCmdShutdown(int32_t _time_sec) = 0;
			virtual void authCmdBroadcastNotice(std::string _notice) = 0;
			virtual void authCmdBroadcastTicker(std::string _nickname, std::string _msg) = 0;
			virtual void authCmdBroadcastCubeWinRare(std::string _msg, uint32_t _option) = 0;
			virtual void authCmdDisconnectPlayer(uint32_t _req_server_uid, uint32_t _player_uid, unsigned char _force) = 0;
			virtual void authCmdConfirmDisconnectPlayer(uint32_t _player_uid) = 0;
			virtual void authCmdNewMailArrivedMailBox(uint32_t _player_uid, uint32_t _mail_id) = 0;
			virtual void authCmdNewRate(uint32_t _tipo, uint32_t _qntd) = 0;
			virtual void authCmdReloadGlobalSystem(uint32_t _tipo) = 0;
			virtual void authCmdInfoPlayerOnline(uint32_t _req_server_uid, uint32_t _player_uid) override;
			virtual void authCmdConfirmSendInfoPlayerOnline(uint32_t _req_server_uid, AuthServerPlayerInfo _aspi) = 0;

			// requests Comandos e respostas dinâmicas
			virtual void authCmdSendCommandToOtherServer(packet& _packet) override;
			virtual void authCmdSendReplyToOtherServer(packet& _packet) override;

			// Server envia comandos e resposta para outros server com o Auth Server
			virtual void sendCommandToOtherServerWithAuthServer(packet& _packet, uint32_t _send_server_uid_or_type) override;
			virtual void sendReplyToOtherServerWithAuthServer(packet& _packet, uint32_t _send_server_uid_or_type) override;

		// Smart Calcualtor And Discord Chat History, Quem usa é só o Game Server(Smart Calculator e Chat History) e Message Server(Chat History)
		public:
			// get and set - Flag chat discord
			bool getChatDiscord();
			void setChatDiscord(bool _chat_discord);

			virtual void sendSmartCalculatorReplyToPlayer(const uint32_t _uid, std::string _from, std::string _msg);
			virtual void sendNoticeGMFromDiscordCmd(std::string& _notice);

			virtual void sendMessageToDiscordChatHistory(std::string _nickname, std::string _msg);

		protected:
			bool m_chat_discord;	// Flag que habilita ou desabilita o chat ser enviado para o discord pelo Smart Calculator pelo Bot do Discord

		protected:
			thread *m_thread_monitor;

#if defined(_WIN32)
			LONG volatile m_continue_monitor;
			LONG volatile m_continue_register_server;
			LONG volatile m_atomic_disconnect_session;
#elif defined(__linux__)
			int32_t volatile m_continue_monitor;
			int32_t volatile m_continue_register_server;
			int32_t volatile m_atomic_disconnect_session;
#endif

		protected:
			virtual void commandScan();

			virtual void config_init();

		protected:
#if defined(_WIN32)
			LONG volatile m_continue_accept;
#elif defined(__linux__)
			int32_t volatile m_continue_accept;
#endif

			void setAcceptConnection();

		private:
#if defined(_WIN32)
			HANDLE EventShutdown;	// Accept Shutdown
			HANDLE EventMoreAccept;
#elif defined(__linux__)
			Event *EventShutdown;	// Accept Shutdown
			Event *EventMoreAccept;
#endif

		// Shutdown Membros
		protected:
#if defined(_WIN32)
			HANDLE EventShutdownServer;
			HANDLE EventAcceptConnection;
#elif defined(__linux__)
			Event *EventShutdownServer;
			Event *EventAcceptConnection;
#endif

			void closeHandles();

		public:
			void shutdown();

			// Shutdown timer
			timer *m_shutdown;
	
		protected:	// ponteiro para o socket que foi criado no accept
			socket* m_accept_sock;		// socket que controla os checks das conexão se pode ou não logar

		private:
			volatile uint32_t m_acceptsPendents;

#if defined(_WIN32)
		private:	// Valores para inicializar sockaddr do AcceptEx
			GUID m_GuidAcceptEx;
			GUID m_GuidGetAcceptExSockaddrs;
			LPFN_ACCEPTEX m_lpfnAcceptEx;
			LPFN_GETACCEPTEXSOCKADDRS m_lpfnGetAcceptExSockaddrs;

			void init_LpFnAccetEx(SOCKET _listener);
			void init_LpFnGetAcceptExSockaddrs(SOCKET _listener);
#endif
    };

	// Server Static
	class ssv {
		public:
			static server* sv;
	};
}

#endif