// Arquivo cmd_save_nick.hpp
// Criado em 01/04/2018 as 13:57 por Acrisio
// Defini��o da classe CmdSaveNick

#pragma once
#ifndef _STDA_CMD_SAVE_NICK_HPP
#define _STDA_CMD_SAVE_NICK_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdSaveNick : public pangya_db {
		public:
			CmdSaveNick(bool _waiter = false);
			CmdSaveNick(uint32_t _uid, std::string& _nick, bool _waiter = false);
			CmdSaveNick(uint32_t _uid, std::wstring& _nick, bool _waiter = false);
			~CmdSaveNick();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			std::string getNick();
			std::wstring& getWNick();
			void setNick(std::string& _nick);
			void setNick(std::wstring& _nick);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdSaveNick"; };
			virtual std::wstring _wgetName() override { return L"CmdSaveNick"; };

		private:
			uint32_t m_uid;
			std::wstring m_nick;

			const wchar_t* m_szConsulta = L"pangya.ProcSaveNickname";
	};
}

#endif // !_STDA_CMD_SAVE_NICK_HPP
