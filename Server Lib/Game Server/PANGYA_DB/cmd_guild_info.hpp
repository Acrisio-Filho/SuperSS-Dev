// Arquivo cmd_guild_info.hpp
// Criado em 24/03/2018 as 20:18 por Acrisio
// Defini��o da classe CmdGuildInfo

#pragma once
#ifndef _STDA_CMD_GUILD_INFO_HPP
#define _STDA_CMD_GUILD_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdGuildInfo : public pangya_db {
		public:
			explicit CmdGuildInfo(bool _waiter = false);
			CmdGuildInfo(uint32_t _uid, uint32_t _option, bool _waiter = false);
			virtual ~CmdGuildInfo();

			GuildInfoEx& getInfo();
			void setInfo(GuildInfoEx& _gi);

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getOption();
			void setOption(uint32_t _option);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdGuildInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdGuildInfo"; };

		private:
			uint32_t m_uid;
			uint32_t m_option;
			GuildInfoEx m_gi;

			const char* m_szConsulta = "pangya.ProcGetGuildInfo";
	};
}

#endif // !_STDA_CMD_GUILD_INFO_HPP
