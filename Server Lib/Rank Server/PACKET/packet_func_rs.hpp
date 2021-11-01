// Arquivo packet_func_rs.hpp
// Criado em 15/06/2020 as 14:59 por Acrisio
// Defini��o da classe packet_func para o Rank Server

#pragma once
#ifndef _STDA_PACKET_FUNC_RS_HPP
#define _STDA_PACKET_FUNC_RS_HPP

#if defined(_WIN32)
#include <Windows.h>
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "../../Projeto IOCP/TYPE/stdAType.h"
#include "../../Projeto IOCP/PACKET/packet_func.h"
#include "../../Projeto IOCP/PACKET/packet.h"

#include "../TYPE/pangya_rank_st.hpp"

#include <string>
#include <vector>

#include "../SESSION/player.hpp"

#define MAKE_BEGIN_SERVER(_arg1) rank_server *rs = reinterpret_cast< rank_server* >((_arg1));

#define _MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) MAKE_BEGIN_SERVER(_arg1) _MAKE_BEGIN_PACKET(_arg2)

#define MAKE_BEGIN_PACKET_AUTH_SERVER(_arg1, _arg2) MAKE_BEGIN_SERVER(_arg1) _MAKE_BEGIN_PACKET_AUTH_SERVER(_arg2)

#ifdef _DEBUG
#define MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) _MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) _MSG_BEGIN_PACKET
#else
#define MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) _MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2)
#endif // _DEBUG

namespace stdA {
	class packet_func : public packet_func_base {
		public:
			// Cliente
			static int packet000(void* _arg1, void* _arg2); // Request Login
			static int packet001(void* _arg1, void* _arg2);	// Request Player[UID] info
			static int packet002(void* _arg1, void* _arg2);	// Request search player
			static int packet003(void* _arg1, void* _arg2);	// Unknown
			static int packet004(void* _arg1, void* _arg2); // Unknown
			static int packet005(void* _arg1, void* _arg2); // Unknown

			// Server
			static int packet_svFazNada(void* _arg1, void* _arg2);

			// Auth Server
			// Auth Server
			static int packet_as001(void* _arg1, void* _arg2);

			// Broadcast

			// Session
			static void session_send(packet& _p, session *_s, unsigned char _debug);
			static void session_send(std::vector< packet* > _v_p, session *_s, unsigned char _debug);
	};
}

#endif // !_STDA_PACKET_FUNC_RS_HPP
