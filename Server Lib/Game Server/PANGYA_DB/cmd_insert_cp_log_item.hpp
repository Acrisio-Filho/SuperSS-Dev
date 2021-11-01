// Arquivo cmd_insert_cp_log_item.hpp
// Criado em 24/05/2019 as 04:22 por Acrisio
// Definição da classe CmdInsertCPLogItem

#pragma once
#ifndef _STDA_CMD_INSERT_CP_LOG_ITEM_HPP
#define _STDA_CMD_INSERT_CP_LOG_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
    class CmdInsertCPLogItem : public pangya_db {
        public:
            explicit CmdInsertCPLogItem(bool _waiter = false);
            CmdInsertCPLogItem(uint32_t _uid, int64_t _log_id, CPLog::stItem& _item, bool _waiter = false);
            virtual ~CmdInsertCPLogItem();

			uint32_t getUID();
			void setUID(uint32_t _uid);

            int64_t getLogId();
            void setLogId(int64_t _log_id);

            CPLog::stItem& getItem();
            void setItem(CPLog::stItem& _item);

            int64_t getItemId();

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

            virtual std::string _getName() override { return "CmdInsertCPLogItem"; };
            virtual std::wstring _wgetName() override { return L"CmdInsertCPLogItem"; };

        private:
            uint32_t m_uid;	// Para fins de log, ele não utiliza na proc do DB
            int64_t m_log_id;
            int64_t m_item_id;
            CPLog::stItem m_item;

            const char* m_szConsulta = "pangya.ProcInsertCPLogItem";
    };
}

#endif // !_STDA_CMD_INSERT_CP_LOG_ITEM_HPP