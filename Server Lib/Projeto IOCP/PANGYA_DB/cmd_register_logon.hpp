// Arquivo cmd_register_logon.hpp
// Criado em 01/04/2018 as 21:10 por Acrisio
// Definição da classe CmdRegisterLogon

#pragma once
#ifndef _STDA_CMD_REGISTER_LOGON_HPP
#define _STDA_CMD_REGISTER_LOGON_HPP

#include "pangya_db.h"

namespace stdA {
    class CmdRegisterLogon : public pangya_db {
        public:
            CmdRegisterLogon(bool _waiter = false);
            CmdRegisterLogon(uint32_t _uid, int _option, bool _waiter = false);
            ~CmdRegisterLogon();

            uint32_t getUID();
            void setUID(uint32_t _uid);

            int getOption();
            void setOption(int _option);

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdRegisterLogon"; };
			virtual std::wstring _wgetName() override { return L"CmdRegisterLogon"; };

        private:
            uint32_t m_uid;
            int m_option;

            const char* m_szConsulta = "pangya.ProcRegisterLogon";
    };
}

#endif // !_STDA_CMD_REGISTER_LOGON_HPP
