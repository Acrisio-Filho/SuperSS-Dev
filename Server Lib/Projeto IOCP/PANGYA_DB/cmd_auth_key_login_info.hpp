// Arquivo cmd_auth_key_login_info.hpp
// Criado em 01/04/2018 as 19:49 por Acrisio
// Definição da classe CmdAuthKeyLoginInfo

#pragma once
#ifndef _STDA_CMD_KEY_LOGIN_INFO_HPP
#define _STDA_CMD_KEY_LOGIN_INFO_HPP

#include "pangya_db.h"
#include "../TYPE/pangya_st.h"

namespace stdA {
    class CmdAuthKeyLoginInfo : public pangya_db {
        public:
            explicit CmdAuthKeyLoginInfo(bool _waiter = false);
            CmdAuthKeyLoginInfo(uint32_t _uid, bool _waiter = false);
            virtual ~CmdAuthKeyLoginInfo();

            uint32_t getUID();
            void setUID(uint32_t _uid);

            AuthKeyLoginInfo& getInfo();
            void setInfo(AuthKeyLoginInfo& _akli);

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAuthKeyLoginInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdAuthKeyLoginInfo"; };

        private:
            uint32_t m_uid;
            AuthKeyLoginInfo m_akli;

            const char* m_szConsulta = "pangya.ProcGetAuthKeyLogin";
    };
}

#endif // !_STDA_CMD_KEY_LOGIN_INFO_HPP
