// Arquivo cmd_update_guild_member_points.hpp
// Criado em 29/12/2019 as 13:04 por Acrisio
// Defini��o da classe CmdUpdateGuildMemberPoints

#pragma once
#ifndef _STDA_CMD_UPDATE_GUILD_MEMBER_POINTS_HPP
#define _STDA_CMD_UPDATE_GUILD_MEMBER_POINTS_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/guild_type.hpp"

namespace stdA {
	class CmdUpdateGuildMemberPoints : public pangya_db {
		public:
			explicit CmdUpdateGuildMemberPoints(bool _waiter = false);
			CmdUpdateGuildMemberPoints(GuildMemberPoints& _gmp, bool _waiter = false);
			virtual ~CmdUpdateGuildMemberPoints();

			GuildMemberPoints& getInfo();
			void setInfo(GuildMemberPoints& _gmp);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdateGuildMemberPoints"; };
			std::wstring _wgetName() override { return L"CmdUpdateGuildMemberPoints"; };

		private:
			GuildMemberPoints m_gmp;

			const char* m_szConsulta = "pangya.ProcUpdateGuildMemberPoints";
	};
}

#endif // !_STDA_CMD_UPDATE_GUILD_MEMBER_POINTS_HPP
