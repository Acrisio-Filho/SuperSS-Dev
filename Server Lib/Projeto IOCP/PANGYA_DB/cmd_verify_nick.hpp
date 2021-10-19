// Arquivo cmd_verify_nick.hpp
// Criado em 01/04/2018 as 12:50 por Acrisio
// Defini��o da classe CmdVerifyNick

#pragma once
#ifndef _STDA_CMD_VERIFY_NICK_HPP
#define _STDA_CMD_VERIFY_NICK_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdVerifNick : public pangya_db {
		public:
			explicit CmdVerifNick(bool _waiter = false);
			explicit CmdVerifNick(std::string& _nick, bool _waiter = false);
			explicit CmdVerifNick(std::wstring& _nick, bool _waiter = false);
			virtual ~CmdVerifNick();

			std::string getNick();
			std::wstring& getWNick();
			void setNick(std::string& _nick);
			void setNick(std::wstring& _nick);

			uint32_t getUID();

			bool getLastCheck();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdVerifNick"; };
			virtual std::wstring _wgetName() override { return L"CmdVerifNick"; };

		private:
			std::wstring m_nick;
			uint32_t m_uid;
			bool m_check;

			const wchar_t* m_szConsulta = L"pangya.ProcVerifyNickname";
	};
}

#endif // !_STDA_CMD_VERIFY_NICK_HPP
