// Arquivo cmd_grand_prix_clear.hpp
// Criado em 14/06/2019 as 09:05 por Acrisio
// Defini��o da classe CmdGrandPrixClear

#pragma once
#ifndef _STDA_CMD_GRAND_PRIX_CLEAR_HPP
#define _STDA_CMD_GRAND_PRIX_CLEAR_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdGrandPrixClear : public pangya_db {
		public:
			explicit CmdGrandPrixClear(bool _waiter = false);
			CmdGrandPrixClear(uint32_t _uid, bool _waiter = false);
			virtual ~CmdGrandPrixClear();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			std::vector< GrandPrixClear >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdGrandPrixClear";  };
			std::wstring _wgetName() override { return L"CmdGrandPrixClear"; };

		private:
			uint32_t m_uid;
			std::vector< GrandPrixClear > m_gpc;

			const char* m_szConsulta = "pangya.ProcGetGrandPrixClear";
	};
}

#endif // !_STDA_CMD_GRAND_PRIX_CLEAR_HPP
