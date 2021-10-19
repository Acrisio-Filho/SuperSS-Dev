// Arquivo socket.hpp
// Criado em 14/11/2020 as 08:22 por Acrisio
// Defini��o da classe Socket

#ifndef _STDA_SOCKET_HPP
#define _STDA_SOCKET_HPP

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"

#define CLOCK_SOCKET_TO_CHECK CLOCK_MONOTONIC_RAW
#endif

#include <string>

namespace stdA {

	class Socket {
		public:
			Socket();
			virtual ~Socket();

			SOCKET detatch();

			void attach(SOCKET socket);

			void close();

			void abortiveClose();

			void shutdown(int como);

			void listen(int max_listen_same_time);

			void bind(const SOCKADDR_IN &address);

			void connect(std::string host, size_t port);

			void connect(const SOCKADDR_IN* _s_addr);

			bool isConnected();

			int read(unsigned char* _buff, size_t _len);

		protected:
			int getConnectTime();

		public:
			SOCKET m_socket;

		protected:
			SOCKADDR_IN m_sockaddr;
	};
}

#endif // !_STDA_SOCKET_HPP
