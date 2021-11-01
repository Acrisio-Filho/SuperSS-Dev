// Arquivo cmd_find_dolfini_locker_item.hpp
// Criado em 24/05/2019 as 00:09 por Acrisio
// Definição da classe CmdFindDolfiniLockerItem

#pragma once
#ifndef _STDA_CMD_FIND_DOLFINI_LOCKER_ITEM_HPP
#define _STDA_CMD_FIND_DOLFINI_LOCKER_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
    class CmdFindDolfiniLockerItem : public pangya_db {
        public:
            explicit CmdFindDolfiniLockerItem(bool _waiter = false);
            CmdFindDolfiniLockerItem(uint32_t _uid, uint32_t _typeid, bool _waiter = false);
            virtual ~CmdFindDolfiniLockerItem();

            uint32_t getUID();
            void setUID(uint32_t _uid);

            uint32_t getTypeid();
            void setTypeid(uint32_t _typeid);

            DolfiniLockerItem& getInfo();

            bool hasFound();

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

            // get Class Name
            virtual std::string _getName() override { return "CmdFindDolfiniLockerItem"; };
            virtual std::wstring _wgetName() override { return L"CmdFindDolfiniLockerItem"; };
        
        private:
            uint32_t m_uid;
            uint32_t m_typeid;
            DolfiniLockerItem m_dli;

            const char* m_szConsulta = "pangya.ProcFindDolfiniLockerItem";
    };
}

#endif // !_STDA_CMD_FIND_DOLFINI_LOCKER_ITEM_HPP