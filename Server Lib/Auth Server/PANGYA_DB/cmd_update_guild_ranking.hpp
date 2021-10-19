// Arquivo cmd_update_guild_ranking.hpp
// Criado em 29/12/2019 as 15:43 por Acrisio
// Defini��o da classe CmdUpdateGuildRanking

#pragma once
#ifndef _STDA_CMD_UPDATE_GUILD_RANKING_HPP
#define _STDA_CMD_UPDATE_GUILD_RANKING_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdUpdateGuildRanking : public pangya_db {
		public:
			CmdUpdateGuildRanking(bool _waiter = false);
			virtual ~CmdUpdateGuildRanking();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdateGuildRanking"; };
			std::wstring _wgetName() override { return L"CmdUpdateGuildRanking"; };

		private:
			const char* m_szConsulta = "pangya.USP_UPDATE_GUILD_RANKING";
	};
}

#endif // !_STDA_CMD_UPDATE_GUILD_RANKING_HPP
