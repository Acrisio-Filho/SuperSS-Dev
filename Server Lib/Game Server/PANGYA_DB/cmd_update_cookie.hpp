// Arquivo cmd_update_cookie.hpp
// Criado em 13/05/2018 as 11:14 por Acrisio
// Defini��o da classe CmdUpdateCookie

#pragma once
#ifndef _STDA_CMD_UPDATE_COOKIE_HPP
#define _STDA_CMD_UPDATE_COOKIE_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdUpdateCookie : public pangya_db {
		public:
			enum T_UPDATE_COOKIE : unsigned char {
				INCREASE,
				DECREASE,
			};

		public:
			explicit CmdUpdateCookie(bool _waiter = false);
			CmdUpdateCookie(uint32_t _uid, uint64_t _cookie, T_UPDATE_COOKIE _type_update, bool _waiter = false);
			virtual ~CmdUpdateCookie();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint64_t getCookie();
			void setCookie(uint64_t _cookie);

			T_UPDATE_COOKIE getTypeUpdate();
			void setTypeUpdate(T_UPDATE_COOKIE _type_update);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateCookie"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateCookie"; };

		private:
			uint32_t m_uid;
			uint64_t m_cookie;
			T_UPDATE_COOKIE m_type_update;

			const char* m_szConsulta[2] = { "UPDATE pangya.user_info SET cookie = cookie ", " WHERE UID = " };
	};
}

#endif // !_STDA_CMD_UPDATE_COOKIE_HPP
