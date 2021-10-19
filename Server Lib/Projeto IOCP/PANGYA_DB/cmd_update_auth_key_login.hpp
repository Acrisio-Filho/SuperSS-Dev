// Arquivo cmd_update_auth_key_login.hpp
// Criado em 07/04/2018 as 18:09 por Acrisio
// Definição da classe CmdUpdateAuthKeyLogin

#pragma once
#ifndef _STDA_CMD_UPDATE_AUTH_KEY_LOGIN_HPP
#define _STDA_CMD_UPDATE_AUTH_KEY_LOGIN_HPP

#include "pangya_db.h"

namespace stdA {
    class CmdUpdateAuthKeyLogin : public pangya_db {
        public:
            explicit CmdUpdateAuthKeyLogin(bool _waiter = false);
            CmdUpdateAuthKeyLogin(uint32_t _uid, unsigned char _valid, bool _waiter = false);
            virtual ~CmdUpdateAuthKeyLogin();

            uint32_t getUID();
            void setUID(uint32_t _uid);

            unsigned char getValid();
            void setValid(unsigned char _valid);

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateAuthKeyLogin"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateAuthKeyLogin"; };

        private:
            uint32_t m_uid;
            unsigned char m_valid;

            const char* m_szConsulta[2] = { "UPDATE pangya.authkey_login SET valid = ", " WHERE UID = " };
    };
}

#endif // !_STDA_CMD_UPDATE_AUTH_KEY_LOGIN_HPP
