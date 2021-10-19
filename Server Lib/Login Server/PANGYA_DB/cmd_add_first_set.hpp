// Arquivo cmd_add_first_set.hpp
// Criado em 01/04/2018 as 17:32 por Acrisio
// Defini��o da classe CmdAddFirstSet

#pragma once
#ifndef _STDA_CMD_ADD_FIRST_SET_HPP
#define _STDA_CMD_ADD_FIRST_SET_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdAddFirstSet : public pangya_db {
		public:
			CmdAddFirstSet(bool _waiter = false);
			CmdAddFirstSet(uint32_t _uid, bool _waiter = false);
			~CmdAddFirstSet();

			uint32_t getUID();
			void setUID(uint32_t _uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdAddFirstSet"; };
			virtual std::wstring _wgetName() override { return L"CmdAddFirstSet"; };

		private:
			uint32_t m_uid;

			const char* m_szConsulta = "pangya.ProcFirstSet";
	};
}

#endif // !_STDA_CMD_ADD_FIRST_SET_HPP
