// Arquivo cmd_verify_pass.hpp
// Criado em 17/03/2018 as 20:49 por Acrisio
// Definição da classe CmdVerifyPass

#pragma once
#ifndef _STDA_CMD_VERIFY_PASS_HPP
#define _STDA_CMD_VERIFY_PASS_HPP

#include "pangya_db.h"
#include <string>

namespace stdA {
    class CmdVerifyPass : public pangya_db {
        public:
            CmdVerifyPass(bool _waiter = false);
            CmdVerifyPass(uint32_t _uid, std::string _pass, bool _waiter = false);
            ~CmdVerifyPass();

            std::string& getPass();
            void setPass(std::string _pass);

            uint32_t getUID();
            void setUID(uint32_t _uid);

            bool getLastVerify();

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdVerifyPass"; };
			virtual std::wstring _wgetName() override { return L"CmdVerifyPass"; };

        private:
            std::string m_pass;
            uint32_t m_uid;

            bool m_lastVerify;

            const char* m_szConsulta = "pangya.ProcVerifyPass";
    };
}

#endif