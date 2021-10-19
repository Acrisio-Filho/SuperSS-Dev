// Arquivo cmd_list_mac_ban.hpp
// Criado em 22/08/2019 as 10:13 por Acrisio
// Definição da classe CmdListMacBan

#pragma once
#ifndef _STDA_CMD_LIST_MAC_BAN_HPP
#define _STDA_CMD_LIST_MAC_BAN_HPP

#include "pangya_db.h"

#include <vector>
#include <string>

namespace stdA {
    class CmdListMacBan : public pangya_db {
        public:
            CmdListMacBan(bool _waiter = false);
            virtual ~CmdListMacBan();

            std::vector< std::string >& getList();

        protected:
            void lineResult(result_set::ctx_res *_result, uint32_t _index_result) override;
            response *prepareConsulta(database& _db) override;

            std::string _getName() override { return "CmdListMacBan"; };
            std::wstring _wgetName() override { return L"CmdListMacBan"; };

        private:
            std::vector< std::string > v_list_mac_ban;

            const char *m_szConsulta = "SELECT mac FROM pangya.pangya_mac_table";
    };
}

#endif // !_STDA_CMD_LIST_MAC_BAN_HPP