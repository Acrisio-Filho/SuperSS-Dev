// Arquivo cmd_cookie.hpp
// Criado em 19/03/2018 as 21:30 por Acrisio
// Definição da classe CmdCookie

#pragma once
#ifndef _STDA_CMD_COOKIE_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
    class CmdCookie : public pangya_db {
        public:
            explicit CmdCookie(bool _waiter = false);
            CmdCookie(uint32_t _uid, bool _waiter = false);
            virtual ~CmdCookie();

            uint64_t getCookie();
            void setCookie(uint64_t _cookie);

            uint32_t getUID();
            void setUID(uint32_t _uid);

        protected:
            void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
            response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdCookie"; };
			virtual std::wstring _wgetName() override { return L"CmdCookie"; };

        private:
            uint32_t m_uid;
            uint64_t m_cookie;

            const char* m_szConsulta = "SELECT uid, cookie FROM pangya.user_info WHERE uid = ";
    };
}

#endif