// Arquivo cmd_update_guild_update_activity.hpp
// Criado em 30/11/2019 as 17:27 por Acrisio
// Defini��o da classe CmdUpdateGuildUpdateActivity

#pragma once
#ifndef _STDA_CMD_UPDATE_GUILD_UPDATE_ACTIVITY_HPP
#define _STDA_CMD_UPDATE_GUILD_UPDATE_ACTIVITY_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdUpdateGuildUpdateActiviy : public pangya_db {
		public:
			explicit CmdUpdateGuildUpdateActiviy(bool _waiter = false);
			CmdUpdateGuildUpdateActiviy(uint64_t _index, bool _waiter = false);
			virtual ~CmdUpdateGuildUpdateActiviy();

			uint64_t getIndex();
			void setIndex(uint64_t _index);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdateGuildUpdateActivity"; };
			std::wstring _wgetName() override { return L"CmdUpdateGuildUpdateActivity"; };

		private:
			uint64_t m_index;

			const char* m_szConsulta = "UPDATE pangya.pangya_guild_update_activity SET STATE = 1 WHERE INDEX = ";
	};
}

#endif // !_STDA_CMD_UPDATE_GUILD_UPDATE_ACTIVITY_HPP
