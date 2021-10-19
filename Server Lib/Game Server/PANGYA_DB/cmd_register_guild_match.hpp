// Arquivo cmd_register_guild_match.hpp
// Criado em 29/12/2019 as 12:04 por Acrisio
// Defini��o da classe CmdRegisterGuildMatch

#pragma once
#ifndef _STDA_CMD_REGISTER_GUILD_MATCH_HPP
#define _STDA_CMD_REGISTER_GUILD_MATCH_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/guild_type.hpp"

namespace stdA {
	class CmdRegisterGuildMatch : public pangya_db {
		public:
			explicit CmdRegisterGuildMatch(bool _waiter = false);
			CmdRegisterGuildMatch(GuildMatch& _match, bool _waiter = false);
			virtual ~CmdRegisterGuildMatch();

			GuildMatch& getInfo();
			void setInfo(GuildMatch& _match);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdRegisterGuildMatch"; };
			std::wstring _wgetName() override { return L"CmdRegisterGuildMatch"; };

		private:
			GuildMatch m_match;

			const char* m_szConsulta = "pangya.ProcRegisterGuildMatch";
	};
}

#endif // !_STDA_CMD_REGISTER_GUILD_MATCH_HPP