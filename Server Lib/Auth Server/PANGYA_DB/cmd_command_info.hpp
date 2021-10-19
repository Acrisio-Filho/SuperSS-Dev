// Arquivo cmd_command_info.hpp
// Criado em 02/12/2018 as 22:37 por Acrisio
// Defini��o da classe CmdCommandInfo

#pragma once
#ifndef _STDA_CMD_COMMAND_INFO_HPP
#define _STDA_CMD_COMMAND_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_auth_st.h"
#include <vector>

namespace stdA {
	class CmdCommandInfo : public pangya_db {
		public:
			explicit CmdCommandInfo(bool _waiter = false);
			virtual ~CmdCommandInfo();

			std::vector< CommandInfo >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdCommandInfo"; };
			std::wstring _wgetName() override { return L"CmdCommandInfo"; };

		private:
			std::vector< CommandInfo > v_ci;

			const char* m_szConsulta = "pangya.ProcGetCommands";
	};
}

#endif // !_STDA_CMD_COMMAND_INFO_HPP
