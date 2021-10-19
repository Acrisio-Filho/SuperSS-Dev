// Arquivo cmd_auth_key_game_info.hpp
// Criado em 01/04/2018 as 20:09 por Acrisio
// Definição da classe CmdAuthKeyGameInfo

#pragma once
#ifndef _STDA_CMD_AUTH_KEY_GAME_INFO_HPP
#define _STDA_CMD_AUTH_KEY_GAME_INFO_HPP

#include "pangya_db.h"
#include "../TYPE/pangya_st.h"

namespace stdA {
    class CmdAuthKeyGameInfo : public pangya_db {
        public:
            explicit CmdAuthKeyGameInfo(bool _waiter = false);
            CmdAuthKeyGameInfo(uint32_t _uid, uint32_t _server_uid, bool _waiter = false);
            virtual ~CmdAuthKeyGameInfo();

            uint32_t getUID();
            void setUID(uint32_t _uid);

            uint32_t getServerUID();
            void setServerUID(uint32_t _server_uid);

            AuthKeyGameInfo& getInfo();
            void setInfo(AuthKeyGameInfo& _akgi);

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAuthKeyGameInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdAuthKeyGameInfo"; };

        private:
            uint32_t m_uid;
            uint32_t m_server_uid;
            AuthKeyGameInfo m_akgi;

            const char* m_szConsulta = "pangya.ProcGetAuthKeyGame";
    };
}

#endif // !_STDA_CMD_AUTH_KEY_GAME_INFO_HPP
