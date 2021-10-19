// Arquivo cmd_new_auth_server_key.hpp
// Criado em 15/12/2018 as 15:36 por Acrisio
// Definição da classe CmdNewAuthServerKey

#pragma once
#ifndef _STDA_CMD_NEW_AUTH_SERVER_KEY_HPP
#define _STDA_CMD_NEW_AUTH_SERVER_KEY_HPP

#include "pangya_db.h"
#include <string>

namespace stdA {
    class CmdNewAuthServerKey : public pangya_db {
        public:
            explicit CmdNewAuthServerKey(bool _waiter = false);
            CmdNewAuthServerKey(uint32_t _server_uid, bool _waiter = false);
            virtual ~CmdNewAuthServerKey();

            uint32_t getServerUID();
            void setServerUID(uint32_t _server_uid);

            std::string& getInfo();

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

            std::string _getName() override { return "CmdNewAuthServerKey"; };
            std::wstring _wgetName() override { return L"CmdNewAuthServerKey"; };

        private:
            uint32_t m_server_uid;
            std::string m_key;

            const char* m_szConsulta = "pangya.ProcGetNewAuthServerKey";
    };
}

#endif // !_STDA_CMD_NEW_AUTH_SERVER_KEY_HPP