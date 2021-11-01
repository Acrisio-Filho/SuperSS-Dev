// Arquivo cmd_insert_cp_log.hpp
// Criado em 24/05/2019 as 04:12 por Acrisio
// Definição da classe CmdInsertCPLog

#pragma once
#ifndef _STDA_CMD_INSERT_CP_LOG_HPP
#define _STDA_CMD_INSERT_CP_LOG_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
    class CmdInsertCPLog : public pangya_db {
        public:
            explicit CmdInsertCPLog(bool _waiter = false);
            CmdInsertCPLog(uint32_t _uid, CPLog& _cp_log, bool _waiter = false);
            virtual ~CmdInsertCPLog();

            uint32_t getUID();
            void setUID(uint32_t _uid);

            CPLog& getLog();
            void setLog(CPLog& _cp_log);

            int64_t getId();

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

            virtual std::string _getName() override { return "CmdInsertCPLog"; };
            virtual std::wstring _wgetName() override { return L"CmdInsertCPLog"; };

        private:
            uint32_t m_uid;
            int64_t m_id;
            CPLog m_cp_log;

            const char* m_szConsulta = "pangya.ProcInsertCPLog";
    };
}

#endif // !_STDA_CMD_INSERT_CP_LOG_HPP