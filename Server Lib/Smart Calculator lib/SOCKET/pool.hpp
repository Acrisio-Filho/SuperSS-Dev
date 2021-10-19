// Arquivo pool.hpp
// Criado em 14/11/2020 as 09:59 por Acrisio
// Defini��o da classe Pool

#pragma once
#ifndef _STDA_POOL_HPP
#define _STDA_POOL_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "socket.hpp"

#if defined(_WIN32)
#include "../../Projeto IOCP/SOCKET/wsa.h"
#endif

#include "../../Projeto IOCP/THREAD POOL/thread.h"
#include "../../Projeto IOCP/TYPE/singleton.h"

#include "../UTIL/logger.hpp"
#include "../TYPE/common.hpp"

#include <vector>

namespace stdA {

	class Pool 
#if defined(_WIN32)
		: public wsa 
#endif
	{
		public:
			Pool();
			virtual ~Pool();

			int initialize(FNPROCSCLOG _scLog, FNPROCRESPONSECALLBACK _responseCallBack, const uint32_t _server_id);
			
			void sendPacket(Packet< ePACKET >& _pckt);

			// Send Server command
			void sendServerCmd(std::string _cmd);

		protected:
#if defined(_WIN32)
			static DWORD WINAPI _translate(LPVOID lpParameter);
#elif defined(__linux__)
			static void* WINAPI _translate(LPVOID lpParameter);
#endif
			
		protected:
#if defined(_WIN32)
			DWORD translate();
#elif defined(__linux__)
			void* translate();
#endif

			int connect();

		public:
			void close();

		protected:
			thread* m_thread;
			Socket m_socket;

			// Id do server que carregou a DLL
			uint32_t m_server_id;

			std::vector< unsigned char > m_sBuff;

			bool m_state;
	};

	typedef Singleton< Pool > sPool;
}

#endif // !_STDA_POOL_HPP
