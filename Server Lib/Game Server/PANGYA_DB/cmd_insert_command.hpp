// Arquivo cmd_insert_command.hpp
// Criado em 04/12/2018 as 03:51 por Acrisio
// Defini��o da classe CmdInsertCommand

#pragma once
#ifndef _STDA_CMD_INSERT_COMMAND_HPP
#define _STDA_CMD_INSERT_COMMAND_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdInsertCommand : public pangya_db {
		public:
			explicit CmdInsertCommand(bool _waiter = false);
			CmdInsertCommand(CommandInfo& _ci, bool _waiter = false);
			virtual ~CmdInsertCommand();

			CommandInfo& getInfo();
			void setInfo(CommandInfo& _ci);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdInsertCommand"; };
			std::wstring _wgetName() override { return L"CmdInsertCommand"; };

		private:
			CommandInfo m_ci;

			const char* m_szConsulta = "pangya.ProcInsertCommand";
	};
}

#endif // !_STDA_CMD_INSERT_COMMAND_HPP
