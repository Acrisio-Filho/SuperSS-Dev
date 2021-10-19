// Aruivo cmd_insert_block_mac.hpp
// Criado em 22/08/2019 as 11:45 por Acrisio
// Definição da classe CmdInsertBlockMac

#pragma once
#ifndef _STDA_CMD_INSERT_BLOCK_MAC_HPP
#define _STDA_CMD_INSERT_BLOCK_MAC_HPP

#include "pangya_db.h"
#include <string>

namespace stdA {
    class CmdInsertBlockMAC : public pangya_db {
        public:
            explicit CmdInsertBlockMAC(bool _waiter = false);
            CmdInsertBlockMAC(std::string& _mac_address, bool _waiter = false);
            virtual ~CmdInsertBlockMAC();

            std::string& getMACAddress();
            void setMACAddress(std::string& _mac_address);

        protected:
            void lineResult(result_set::ctx_res *_result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

            std::string _getName() override { return "CmdInsertBlockMAC"; };
            std::wstring _wgetName() override { return L"CmdInsertBlockMAC"; };

        private:
            std::string m_mac_address;

            const char* m_szConsulta = "pangya.ProcInsertBlockMAC";
    };
}

#endif // !STDA_CMD_INSERT_BLOCK_MAC_HPP