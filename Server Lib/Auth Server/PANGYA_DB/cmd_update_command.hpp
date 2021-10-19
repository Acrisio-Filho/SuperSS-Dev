// Arquivo cmd_update_command.hpp
// Criado em 03/12/2018 as 15:45 por Acrisio
// Defini��o da classe CmdUpdateCommand

#pragma once
#ifndef _STDA_CMD_UPDATE_COMMAND_HPP
#define _STDA_CMD_UPDATE_COMMAND_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_auth_st.h"

namespace stdA {
	class CmdUpdateCommand : public pangya_db {
		public:
			explicit CmdUpdateCommand(bool _waiter = false);
			CmdUpdateCommand(CommandInfo& _ci, bool _waiter = false);
			virtual ~CmdUpdateCommand();

			CommandInfo& getInfo();
			void setInfo(CommandInfo& _ci);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdateCommand"; };
			std::wstring _wgetName() override { return L"CmdUpdateCommand"; };

		private:
			CommandInfo m_ci;

			const char* m_szConsulta = "pangya.ProcUpdateCommand";
	};
}

#endif // !_STDA_CMD_UPDATE_COMMAND_HPP
