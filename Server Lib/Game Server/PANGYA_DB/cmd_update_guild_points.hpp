// Arquivo cmd_update_guild_points.hpp
// Criado em 29/12/2019 as 12:24 por Acrisio
// Defini��o da classe CmdUpdateGuildPoints

#pragma once
#ifndef _STDA_CMD_UPDATE_GUILD_POINTS_HPP
#define _STDA_CMD_UPDATE_GUILD_POINTS_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/guild_type.hpp"

namespace stdA {
	class CmdUpdateGuildPoints : public pangya_db {
		public:
			explicit CmdUpdateGuildPoints(bool _waiter = false);
			CmdUpdateGuildPoints(GuildPoints& _gp, bool _waiter = false);
			virtual ~CmdUpdateGuildPoints();

			GuildPoints& getInfo();
			void setInfo(GuildPoints& _gp);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdateGuildPoints"; };
			std::wstring _wgetName() override { return L"CmdUpdateGuildPoints"; };

		private:
			GuildPoints m_gp;

			const char* m_szConsulta = "pangya.ProcUpdateGuildPoints";
	};
}

#endif // !_STDA_CMD_UPDATE_GUILD_POINTS_HPP
