// Arquivo packet_func_client.h
// Criado em 21/10/2017 por Acrisio
// Defini��o da classe packet_func para o cliente(BOT)

#pragma once
#ifndef _STDA_PACKET_FUNC_CLIENT_H
#define _STDA_PACKET_FUNC_CLIENT_H

#include "../../Projeto IOCP/TYPE/stdAType.h"
#include "../../Projeto IOCP/SOCKET/session.h"
#include "../../Projeto IOCP/UTIL/func_arr.h"
#include "../../Projeto IOCP/PACKET/packet.h"
#include "../SESSION/player.hpp"
#include <string>
#include <vector>
#include <map>

#include "../../Projeto IOCP/PACKET/packet_func.h"

#if defined SHOW_DEBUG_PACKET && defined _DEBUG
#undef SHOW_DEBUG_PACKET
#define SHOW_DEBUG_PACKET(_packet, _debug) if ((_debug)) _smp::message_pool::getInstance().push(new message(hex_util::BufferToHexString((_packet).getBuffer(), (_packet).getSize()), CL_ONLY_FILE_LOG));
#define SHOW_DEBUG_FINAL_PACKET(_mwsabuff, _debug) if ((_debug)) _smp::message_pool::getInstance().push(new message(hex_util::BufferToHexString((unsigned char*)(_mwsabuff).buf, (_mwsabuff).len), CL_ONLY_FILE_LOG));
#else
#define SHOW_DEBUG_PACKET
#define SHOW_DEBUG_FINAL_PACKET
#endif

#define _MAKE_BEGIN_CLIENT(_arg) multi_client *mc = reinterpret_cast< multi_client* >((_arg));

#define _MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2) _MAKE_BEGIN_CLIENT(_arg1) _MAKE_BEGIN_PACKET(_arg2)

#ifdef _DEBUG
#define MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2) _MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2) _MSG_BEGIN_PACKET
#else
#define MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2) _MAKE_BEGIN_PACKET_CLIENT(_arg1, _arg2)
#endif

namespace stdA {

	class packet_func : public packet_func_base {
		public:
			// Cliente
			static int packet000(void* _arg1, void* _arg2);
			static int packet001(void* _arg1, void* _arg2);
			static int packet002(void* _arg1, void* _arg2);
			static int packet003(void* _arg1, void* _arg2);
			static int packet006(void* _arg1, void* _arg2);
			static int packet009(void* _arg1, void* _arg2);
			static int packet010(void* _arg1, void* _arg2);
			static int packet03F(void* _arg1, void* _arg2);
			static int packet044(void* _arg1, void* _arg2);
			static int packet04D(void* _arg1, void* _arg2);
			static int packet04E(void* _arg1, void* _arg2);
			static int packet0F5(void* _arg1, void* _arg2);
			static int packet0D7(void* _arg1, void* _arg2);
			static int packet1A9(void* _arg1, void* _arg2);
			static int packet1AD(void* _arg1, void* _arg2);

			// Packet Server - Está invertido por que aqui é o cliente
			static int packet_svFazNada(void* _arg1, void* _arg2);
			static int packet_sv001(void* _arg1, void* _arg2);

			// Server
			static int pacote001(packet& p, player *_session, int option = 0);
			static int pacote002(packet& p, player *_session, int option = 0);
			static int pacote003(packet& p, player *_session, unsigned long guid);
			static int pacote003g(packet& p, player *_session, std::string _nickname, std::string _msg);
			static int pacote004(packet& p, player *_session, int option, unsigned char canal_id = ~0);
			static int pacote03D(packet& p, player *_session);
			static int pacote081(packet& p, player *_session);
			static int packet09E(packet& p, player *_session);
			static int pacote0A1(packet& p, player *_session, unsigned char opt);
			static int pacote0A2(packet& p, player *_session);
			static int pacote0C1(packet& p, player *_session, unsigned char opt);
			static int pacote0F4(packet& p, player *_session);
			static int pacote0FB(packet& p, player *_session);

			// Metêdos de auxálio de criação de pacotes
			static void session_send(packet& p, player *s, unsigned char _debug = 0);
			static void session_send(std::vector< packet* > v_p, player *s, unsigned char _debug = 0);

			// Funcionalidades do command
			static void abrirWeblink(player *_session, unsigned char who);
			static void entraCanal(player *_session, unsigned char canal_id);

			// Send Msg For Timer Job
			static int sendMessage(void *_arg1, void *_arg2);
			
			// Send TTL Time To Live (HeartBeat)
			static int HeartBeat(void *_arg1, void *_arg2);

			// Send mensage no chat
			static void sendMessage(player *_session, std::string _nickname, std::string _msg);
	};
}

#endif // !_STDA_PACKET_FUNC_CLIENT_H

