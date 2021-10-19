// Arquivo cmd_auth_key_game.hpp
// Criado em 18/03/2018 as 10:41 por Acrisio
// Definição da classe CmdAuthKeyGame

#pragma once
#ifndef _STDA_CMD_AUTH_KEY_GAME_HPP
#define _STDA_CMD_AUTH_KEY_GAME_HPP

#include "pangya_db.h"
#include <string>

namespace stdA {
    class CmdAuthKeyGame : public pangya_db {
        public:
            explicit CmdAuthKeyGame(bool _waiter = false);
            CmdAuthKeyGame(uint32_t _uid, uint32_t _server_uid, bool _waiter = false);
            virtual ~CmdAuthKeyGame();

            std::string& getAuthKey();
            void setAuthKey(std::string _auth_key);

            uint32_t getUID();
            void setUID(uint32_t _uid);

            uint32_t getServerUID();
            void setServerUID(uint32_t _server_uid);

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAuthKeyGame"; };
			virtual std::wstring _wgetName() override { return L"CmdAuthKeyGame"; };

        private:
            uint32_t m_uid;
            uint32_t m_server_uid;
            std::string m_auth_key_game;

            const char* m_szConsulta = "pangya.ProcGeraAuthKeyGame";
    };
}

#endif