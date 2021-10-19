// Arquivo cmd_chat_macro_user.hpp
// Criado em 18/03/2018 as 11:15 por Acrisio
// Definição da classe CmdChatMacroUser

#pragma once
#ifndef _STDA_CMD_CHAT_MACRO_USER_HPP
#define _STDA_CMD_CHAT_MACRO_USER_HPP

#include "pangya_db.h"
#include "../TYPE/pangya_st.h"

namespace stdA {
    class CmdChatMacroUser : public pangya_db {
        public:
			CmdChatMacroUser(bool _waiter = false);
			CmdChatMacroUser(uint32_t _uid, bool _waiter = false);
            ~CmdChatMacroUser();

            chat_macro_user& getMacroUser();
            void setMacroUser(chat_macro_user& _macro_user);

            uint32_t getUID();
            void setUID(uint32_t _uid);

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdChatMacroUser"; };
			virtual std::wstring _wgetName() override { return L"CmdChatMacroUser"; };

        private:
            uint32_t m_uid;
            chat_macro_user m_macro_user;

            const char* m_szConsulta = "pangya.ProcGetMacrosUser";
    };
}

#endif