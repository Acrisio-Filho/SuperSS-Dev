// Arquivo cmd_member_info.hpp
// Criado em 18/03/2018 as 13:11 por Acrisio
// Definição da classe CmdMemberInfo

#pragma once
#ifndef _STDA_CMD_MEMBER_INFO_HPP
#define _STDA_CMD_MEMBER_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
    class CmdMemberInfo : public pangya_db {
        public:
            explicit CmdMemberInfo(bool _waiter = false);
            CmdMemberInfo(uint32_t _uid, bool _waiter = false);
            virtual ~CmdMemberInfo();

            MemberInfoEx& getInfo();
            void setInfo(MemberInfoEx& _mi);

            uint32_t getUID();
            void setUID(uint32_t _uid);

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdMemberInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdMemberInfo"; };

        private:
            uint32_t m_uid;
            MemberInfoEx m_mi;

            const char* m_szConsulta = "pangya.ProcGetUserInfo";
    };
}

#endif