// Arquivo cmd_keys_of_login.hpp
// Criado em 18/03/2018 as 12:37 por Acrisio
// Definição da classe CmdKeysOfLogin

#pragma once
#ifndef _STDA_CMD_KEYS_OF_LOGIN_HPP
#define _STDA_CMD_KEYS_OF_LOGIN_HPP

#include "pangya_db.h"
#include "../TYPE/pangya_st.h"

namespace stdA {
    class CmdKeysOfLogin : public pangya_db {
        public:
            CmdKeysOfLogin();
            CmdKeysOfLogin(uint32_t _uid);
            ~CmdKeysOfLogin();

            KeysOfLogin& getKeys();
            void setKeys(KeysOfLogin& _keys_of_login);

            uint32_t getUID();
            void setUID(uint32_t _uid);

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta() override;

        private:
            uint32_t m_uid;
            KeysOfLogin m_keys_of_login;

            const char* m_szConsulta = "pangya.ProcGetAuthKeys";
    };
}

#endif