// Arquivo packet_func_as.h
// Criado em 02/12/2018 as 13:29 por Acrisio
// Definição da classe packet_func

#pragma once
#ifndef _STDA_PACKET_FUNC_AS_H
#define _STDA_PACKET_FUNC_AS_H

#include "../../Projeto IOCP/PACKET/packet_func.h"
#include "../../Projeto IOCP/TYPE/stdAType.h"
#include <string>
#include <vector>

#include "../TYPE/pangya_auth_st.h"
#include "../SESSION/player.hpp"

#define _MAKE_BEGIN_LS(_arg) auth_server *as = reinterpret_cast< auth_server* >((_arg));

#define _MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) _MAKE_BEGIN_LS(_arg1) _MAKE_BEGIN_PACKET(_arg2)

#ifdef _DEBUG
#define MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) _MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) _MSG_BEGIN_PACKET
#else
#define MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) _MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2)
#endif

namespace stdA {

	class packet_func : public packet_func_base {
	public:
		// Cliente
		static int packet001(void* _arg1, void* _arg2);	// request::Autenticação
		static int packet002(void* _arg1, void* _arg2); // request::Disconnect player
		static int packet003(void* _arg1, void* _arg2); // request::Confirm Disconnect player
		static int packet004(void* _arg1, void* _arg2);	// request::Info Player
		static int packet005(void* _arg1, void* _arg2);	// request::Confirm Send Info Player
		static int packet006(void* _arg1, void* _arg2);	// request::Send command to other Server
		static int packet007(void* _arg1, void* _arg2);	// request::Send Reply to other Server

		static int packet_svFazNada(void* _arg1, void* _arg2);

		// Server

		// Method Helper
		static void session_send(packet& p, player *s, unsigned char _debug = 0);
		static void session_send(std::vector< packet* > v_p, player *s, unsigned char _debug = 0);
		static void vector_send(packet& p, std::vector< player* > _v_s, unsigned char _debug = 0);
		static void vector_send(std::vector< packet* > _v_p, std::vector< player* > _v_s, unsigned char _debug = 0);
	};
}

#endif // !_STDA_PACKET_FUNC_AS_H
