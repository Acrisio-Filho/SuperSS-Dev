// Arquivo cmd_update_auth_server_key.hpp
// Criado em 15/12/2018 as 18:46 por Acrisio
// Definição da classe CmdUpdateAuthServerKey

#pragma once
#ifndef _STDA_CMD_UPDATE_AUTH_SERVER_KEY_HPP
#define _STDA_CMD_UPDATE_AUTH_SERVER_KEY_HPP

#include "pangya_db.h"
#include "../TYPE/pangya_st.h"

namespace stdA {
    class CmdUpdateAuthServerKey : public pangya_db {
        public:
            explicit CmdUpdateAuthServerKey(bool _waiter = false);
            CmdUpdateAuthServerKey(AuthServerKey& _ask, bool _waiter = false);
            virtual ~CmdUpdateAuthServerKey();

            AuthServerKey& getInfo();
            void setInto(AuthServerKey& _ask);

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

            std::string _getName() override { return "CmdUpdateAuthServerKey"; };
            std::wstring _wgetName() override { return L"CmdUpdateAuthServerKey"; };

        private:
            AuthServerKey m_ask;

            const char* m_szConsulta = "pangya.ProcUpdateAuthServerKey";
    };
}

#endif // !_STDA_CMD_UPDATE_AUTH_SERVER_KEY_HPP