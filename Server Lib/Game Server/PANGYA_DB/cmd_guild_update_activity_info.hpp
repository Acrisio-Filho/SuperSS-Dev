// Arquivo cmd_guild_update_activity_info.hpp
// Criado em 30/11/2019 as 16:12 por Acrisio
// Defini��o da classe CmdGuildUpdateActivityInfo

#pragma once
#ifndef _STDA_CMD_GUILD_UPDATE_ACTIVITY_INFO_HPP
#define _STDA_CMD_GUILD_UPDATE_ACTIVITY_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <vector>

namespace stdA {
	class CmdGuildUpdateActivityInfo : public pangya_db {
		public:
			explicit CmdGuildUpdateActivityInfo(bool _waiter = false);
			CmdGuildUpdateActivityInfo(uint32_t _guild_uid, uint32_t _memeber_uid, bool _waiter = false);
			virtual ~CmdGuildUpdateActivityInfo();

			uint32_t getGuildUID();
			void setGuildUID(uint32_t _uid);

			uint32_t getMemberUID();
			void setMemberUID(uint32_t _member_uid);

			std::vector< GuildUpdateActivityInfo >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdGuildUpdateActivityInfo"; };
			std::wstring _wgetName() override { return L"CmdGuildUpdateActivityInfo"; };

		private:
			uint32_t m_guild_uid;
			uint32_t m_member_uid;
			std::vector< GuildUpdateActivityInfo > m_info;

			const char* m_szConsulta = "pangya.ProcGetGuildUpdateActivity";
	};
}

#endif // !_STDA_CMD_GUILD_UPDATE_ACTIVITY_INFO_HPP
