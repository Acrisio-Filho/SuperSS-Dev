// Aquivo cmd_rate_config_info.hpp
// Criado em 09/05/2019 as 18:39 por Acrisio
// Definição da classe CmdRateConfigInfo

#pragma once
#ifndef _STDA_CMD_RATE_CONFIG_INFO_HPP
#define _STDA_CMD_RATE_CONFIG_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../../Projeto IOCP/TYPE/pangya_st.h"

namespace stdA {
    class CmdRateConfigInfo : public pangya_db {
        public:
            CmdRateConfigInfo(bool _waiter = false);
			explicit CmdRateConfigInfo(uint32_t _server_uid, bool _waiter = false);
            virtual ~CmdRateConfigInfo();

			uint32_t getServerUID();
			void setServerUID(uint32_t _server_uid);

            RateConfigInfo& getInfo();

			bool isError();

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

            // get Class Name
            virtual std::string _getName() override { return "CmdRateConfigInfo"; };
            virtual std::wstring _wgetName() override { return L"CmdRateConfigInfo"; };

        private:
            RateConfigInfo m_rate_info;
			uint32_t m_server_uid;
			bool m_error;

            const char* m_szConsulta = "pangya.ProcGetRateConfigInfo";
    };
}

#endif // !_STDA_CMD_RATE_CONFIG_INFO_HPP