// Arquivo cmd_server_list.hpp
// Criado em 17/03/2018 as 22:45 por Acrisio
// Definição da classe CmdServerList

#pragma once
#ifndef _STDA_CMD_SERVER_LIST_HPP
#define _STDA_CMD_SERVER_LIST_HPP

#include "pangya_db.h"
#include "../TYPE/pangya_st.h"
#include <vector>

namespace stdA {
    class CmdServerList : public pangya_db {
        public:
            enum TYPE_SERVER : unsigned char {
                GAME,
                MSN,
                LOGIN,
                RANK,
                AUTH,
            };

        public:
            explicit CmdServerList(bool _waiter = false);
            CmdServerList(TYPE_SERVER _type, bool _waiter = false);
            virtual ~CmdServerList();

            TYPE_SERVER getType();
            void setType(TYPE_SERVER _type);

            std::vector< ServerInfo >& getServerList();

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdServerList"; };
			virtual std::wstring _wgetName() override { return L"CmdServerList"; };

        private:
            TYPE_SERVER m_type;
            std::vector< ServerInfo > v_server_list;

            const char* m_szConsulta = "pangya.ProcGetServerList";
    };
}

#endif