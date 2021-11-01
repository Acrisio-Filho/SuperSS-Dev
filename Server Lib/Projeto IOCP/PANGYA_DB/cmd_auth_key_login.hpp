// Arquivo cmd_auth_key_login.hpp
// Criado em 18/03/2018 as 10:41 por Acrisio
// Definição da classe CmdAuthKeyLogin

#pragma once
#ifndef _STDA_CMD_AUTH_KEY_LOGIN_HPP
#define _STDA_CMD_AUTH_KEY_LOGIN_HPP

#include "pangya_db.h"
#include <string>

namespace stdA {
    class CmdAuthKeyLogin : public pangya_db {
        public:
            explicit CmdAuthKeyLogin(bool _waiter = false);
            CmdAuthKeyLogin(uint32_t _uid, bool _waiter = false);
            virtual ~CmdAuthKeyLogin();

            std::string& getAuthKey();
            void setAuthKey(std::string _auth_key);

            uint32_t getUID();
            void setUID(uint32_t _uid);

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAuthKeyLogin"; };
			virtual std::wstring _wgetName() override { return L"CmdAuthKeyLogin"; };

        private:
            uint32_t m_uid;
            std::string m_auth_key_login;

			const char* m_szConsulta = "pangya.ProcGeraAuthKeyLogin";
    };
}

#endif