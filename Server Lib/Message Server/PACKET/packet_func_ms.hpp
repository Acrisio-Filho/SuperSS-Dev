// Arquivo packet_func_ms.hpp
// Criado em 29/07/2018 as 14:29 por Acrisio
// Defini��o da classe packet_func para o Message Server

#pragma once
#ifndef _STDA_PACKET_FUNC_MS_HPP
#define _STDA_PACKET_FUNC_MS_HPP

#if defined(_WIN32)
#include <Windows.h>
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "../../Projeto IOCP/TYPE/stdAType.h"
#include "../../Projeto IOCP/PACKET/packet_func.h"
#include "../../Projeto IOCP/PACKET/packet.h"

#include "../TYPE/pangya_message_st.hpp"

#include <string>
#include <vector>

#include "../SESSION/player.hpp"

#define MAKE_BEGIN_SERVER(_arg1) message_server *ms = reinterpret_cast< message_server* >((_arg1));

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
			static int packet012(void* _arg1, void* _arg2);	// Request Login
			static int packet013(void* _arg1, void* _arg2);	// N�o sei o que � ainda
			static int packet014(void* _arg1, void* _arg2);	// Request List de Amigos e Membros da Guild se tiver em uma guild
			static int packet016(void* _arg1, void* _arg2);	// Request Update Player Logout
			static int packet017(void* _arg1, void* _arg2);	// Request Verify Nickname
			static int packet018(void* _arg1, void* _arg2);	// Request Add Friend
			static int packet019(void* _arg1, void* _arg2);	// Request Confirm Friend
			static int packet01A(void* _arg1, void* _arg2);	// Request Block Friend
			static int packet01B(void* _arg1, void* _arg2);	// Request Unblock Friend
			static int packet01C(void* _arg1, void* _arg2);	// Request Delete Friend
			static int packet01D(void* _arg1, void* _arg2);	// Request Update State Player
			static int packet01E(void* _arg1, void* _arg2);	// Request Chat Friend
			static int packet01F(void* _arg1, void* _arg2);	// Request Assing Apelido
			static int packet023(void* _arg1, void* _arg2);	// Request Update Channel Player Info
			static int packet024(void* _arg1, void* _arg2);	// Request Invite player to room
			static int packet025(void* _arg1, void* _arg2);	// Request Chat Guild
			static int packet028(void* _arg1, void* _arg2);	// Request Invite player to Room Guild Battle
			static int packet029(void* _arg1, void* _arg2);	// N�o sei o que � ainda
			static int packet02A(void* _arg1, void* _arg2);	// Request send to all members of guild that player is accepted on guild
			static int packet02B(void* _arg1, void* _arg2);	// Request send to all members of guild that player is kicked from guild
			static int packet02C(void* _arg1, void* _arg2); // Request Change image guild (guild_mark)
			static int packet02D(void* _arg1, void* _arg2);	// Request Change name of guild

			// Server
			static int packet_svFazNada(void* _arg1, void* _arg2);

			// Auth Server
			static int packet_as001(void* _arg1, void* _arg2);
			static int packet_as002(void* _arg1, void* _arg2);
			static int packet_as003(void* _arg1, void* _arg2);

			// BroadCast Friend And Guild Member Online
			static void friend_broadcast(std::map< uint32_t, player* > _m_player, packet& _p, session *_s, unsigned char _debug);
			static void friend_broadcast(std::map< uint32_t, player* > _m_player, std::vector< packet* > _v_p, session *_s, unsigned char _debug);

			// Session
			static void session_send(packet& _p, session *_s, unsigned char _debug);
			static void session_send(std::vector< packet* > _v_p, session *_s, unsigned char _debug);
	};
}

#endif // !_STDA_PACKET_FUNC_MS_HPP
