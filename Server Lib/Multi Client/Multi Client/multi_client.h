// Arquivo multi_client.h
// Criado em 19/12/2017 por Acrisio
// Definição da classe multi_client

#pragma once
#ifndef _STDA_MULTI_CLIENT
#define _STDA_MULTI_CLIENT

#include "../../Projeto IOCP/Client/client.h"
#include "../SESSION/cliente_manager.hpp"

namespace stdA {
	class multi_client : public client {
		public:
			multi_client(unsigned long num_connection);
			virtual ~multi_client();

			virtual void start() override;

			virtual void checkClienteOnline() override;

			virtual void ConnectAndAssoc(std::string _host, short _port, ClientInfo& _ci);

			virtual void DisconnectSession(session *_session) override;

		protected:
			virtual void accept_completed(SOCKET *_listener, DWORD dwIOsize, myOver *lpBuffer, DWORD _operation) override;

			virtual std::string getSessionID(session *_session) override;

		protected:
			virtual DWORD send_msg_lobby() override;

			virtual void commandScan() override;

			unsigned long m_num_connection;

		protected:
			ClienteManager m_cliente_manager;
	};

	class smc {
		public:
			static multi_client* mc;
	};
}

#endif // !_STDA_MULTI_CLIENT

