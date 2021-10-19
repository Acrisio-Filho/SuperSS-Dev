// Arquivo pangya_login_db.h
// Criado em 23/07/2017 por Acrisio
// Definição da classe pangya_db para o Login Server

#pragma once
#ifndef _STDA_PANGYA_LOGIN_DB_H
#define _STDA_PANGYA_LOGIN_DB_H

#include "../../Projeto IOCP/TYPE/stdAType.h"
#include "../TYPE/player_info.hpp"
#include "../../Projeto IOCP/TYPE/list_fifo.h"
#include <string>

#include "../../Projeto IOCP/DATABASE/pangya_db.h"

namespace stdA {
    class pangya_db : public pangya_base_db {
        public:
            pangya_db();
            ~pangya_db();

			static PlayerInfo getPlayerInfo(uint32_t uid);
			static uint32_t VerifyID(std::string id);
			static bool VerifyPass(uint32_t uid, std::string pass);
			static std::string getAuthKey(uint32_t uid);
			static std::string getAuthKeyLogin(uint32_t uid);
			//std::vector< ServerInfo > getServerList();
			static std::vector< ServerInfo > getMsn();
			static macro_user getMacroUser(uint32_t uid);
	};
}

#endif