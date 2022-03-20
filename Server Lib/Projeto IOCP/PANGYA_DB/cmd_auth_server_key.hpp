// Arquivo cmd_auth_server_key.hpp
// Criado em 155/12/2018 as 15:18 por Acrisio
// Definição da classe CmdAuthServerKey

#pragma once
#ifndef _STDA_CMD_AUTH_SERVER_KEY_HPP
#define _STDA_CMD_AUTH_SERVER_KEY_HPP

#include "pangya_db.h"
#include "../TYPE/pangya_st.h"

namespace stdA {
    class CmdAuthServerKey : public pangya_db {
        public:
            explicit CmdAuthServerKey(bool _waiter = false);
            CmdAuthServerKey(uint32_t _server_uid, bool _waiter = false);
            virtual ~CmdAuthServerKey();

            uint32_t getServerUID();
            void setServerUID(uint32_t _server_uid);

            AuthServerKey& getInfo();

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

            std::string _getName() override { return "CmdAuthServerKey"; };
            std::wstring _wgetName() override { return L"CmdAuthSeverKey"; };

        private:
            uint32_t m_server_uid;
            AuthServerKey m_ask;

            const char* m_szConsulta = "SELECT server_uid, " DB_MAKE_ESCAPE_KEYWORD_A("key") ", VALID FROM pangya.pangya_auth_key WHERE server_uid = ";
    };
}

#endif // !_STDA_CMD_AUTH_SERVER_KEY_HPP