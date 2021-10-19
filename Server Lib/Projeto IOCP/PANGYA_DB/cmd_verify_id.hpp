// Arquivo cmd_verify_id.hpp
// Criado em 17/03/2018 as 20:10 por Acrisio
// Definição da classe CmdVerifyID

#pragma once
#ifndef _STDA_CMD_VERIFY_ID_HPP
#define _STDA_CMD_VERIFY_ID_HPP

#include "pangya_db.h"
#include <string>

namespace stdA {
    class CmdVerifyID : public pangya_db {
        public:
            CmdVerifyID(bool _waiter = false);
            CmdVerifyID(std::string _id, bool _waiter = false);
            ~CmdVerifyID();

            std::string& getID();
            void setID(std::string _id);

            uint32_t getUID();

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdVerifyID"; };
			virtual std::wstring _wgetName() override { return L"CmdVerifyID"; };
        
        private:
            std::string m_id;
            uint32_t m_uid;

			const char* m_szConsulta = "pangya.ProcVerifyID";
    };
}

#endif