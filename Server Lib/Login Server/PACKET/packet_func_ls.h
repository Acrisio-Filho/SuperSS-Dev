// Arquivo packet_func_ls.h
// Criado em 04/06/2017 por Acrisio
// Definição da classe packet_func

#pragma once
#ifndef STDA_PACKET_FUNC_LS_H
#define STDA_PACKET_FUNC_LS_H

//#define SHOW_DEBUG_PACKET

#include "../../Projeto IOCP/PACKET/packet_func.h"
#include "../../Projeto IOCP/TYPE/stdAType.h"
#include <string>
#include <vector>

#include "../TYPE/pangya_login_st.h"
#include "../SESSION/player.hpp"

#define _MAKE_BEGIN_LS(_arg) login_server *ls = reinterpret_cast< login_server* >((_arg));

#define _MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) _MAKE_BEGIN_LS(_arg1) _MAKE_BEGIN_PACKET(_arg2)

#define MAKE_BEGIN_PACKET_AUTH_SERVER(_arg1, _arg2) _MAKE_BEGIN_LS(_arg1) _MAKE_BEGIN_PACKET_AUTH_SERVER(_arg2)

#ifdef _DEBUG
#define MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) _MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) _MSG_BEGIN_PACKET
#else
#define MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) _MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2)
#endif

namespace stdA {

    class packet_func : public packet_func_base {
        public:
			// Cliente
            static int packet001(void* _arg1, void* _arg2);
			static int packet003(void* _arg1, void* _arg2);
			static int packet004(void* _arg1, void* _arg2);
			static int packet006(void* _arg1, void* _arg2);
			static int packet007(void* _arg1, void* _arg2);
			static int packet008(void* _arg1, void* _arg2);
			static int packet00B(void* _arg1, void* _arg2);

			static int packet_sv003(void* _arg1, void* _arg2);
			static int packet_sv006(void* _arg1, void* _arg2);
			static int packet_svFazNada(void* _arg1, void* _arg2);
			static int packet_svDisconectPlayerBroadcast(void* _arg1, void* _arg2);

			// Auth Server
			static int packet_as001(void* _arg1, void* _arg2);

			// Server
			static int pacote001(packet& p, player *_session, int option = 0);
			static int pacote002(packet& p, player *_session, std::vector< ServerInfo >& v_element);
			static int pacote003(packet& p, player *_session, std::string& AuthKeyLogin, int option = 0);
			static int pacote006(packet& p, player *_session, chat_macro_user& _mu);
			static int pacote009(packet& p, player *_session, std::vector< ServerInfo >& v_element);
			static int pacote00E(packet& p, player *_session, std::string nick, int option = 0, uint32_t error = 0);
			static int pacote00E(packet& p, player *_session, std::wstring nick, int option = 0, uint32_t error = 0);
			static int pacote00F(packet& p, player *_session, int option = 0);	// Abre o tutorial que quer dizer que é nova conta
			static int pacote010(packet& p, player *_session, std::string& AuthKey);
			static int pacote011(packet& p, player *_session, int option = 0);

			static void session_send(packet& p, player *s, unsigned char _debug = 0);
			static void session_send(std::vector< packet* > v_p, player *s, unsigned char _debug = 0);

		public:
			// success login
			static void succes_login(void* _arg, player *_session, int option = 0);
    };
}

#endif