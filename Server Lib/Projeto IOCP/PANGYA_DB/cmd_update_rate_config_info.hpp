// Aqui cmd_update_rate_config_info.hpp
// Criado em 09/05/2019 as 19:16 por Acrisio
// Definição da classe CmdUpdateRateConfigInfo

#pragma once
#ifndef _STDA_CMD_UPDATE_RATE_CONFIG_INFO_HPP
#define _STDA_CMD_UPDATE_RATE_CONFIG_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../../Projeto IOCP/TYPE/pangya_st.h"

namespace stdA {

    class CmdUpdateRateConfigInfo : public pangya_db {
        public:
            CmdUpdateRateConfigInfo(bool _waiter = false);
            explicit CmdUpdateRateConfigInfo(uint32_t _server_uid, RateConfigInfo& _rci, bool _waiter = false);
            virtual ~CmdUpdateRateConfigInfo();

			uint32_t getServerUID();
			void setServerUID(uint32_t _server_uid);

			RateConfigInfo& getInfo();
            void setInfo(RateConfigInfo& _rci);

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

            virtual std::string _getName() override { return "CmdUpdateRateConfigInfo"; };
            virtual std::wstring _wgetName() override { return L"CmdUpdateRateConfigInfo"; };

        private:
            RateConfigInfo m_rci;
			uint32_t m_server_uid;

            const char* m_szConsulta = "pangya.ProcUpdateRateConfigInfo";
    };
}

#endif // !_STDA_CMD_UPDATE_RATE_CONFIG_INFO_HPP