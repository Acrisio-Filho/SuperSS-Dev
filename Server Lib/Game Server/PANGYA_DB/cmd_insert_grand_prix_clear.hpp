// Arquivo cmd_insert_grand_prix_clear.hpp
// Criado em 14/06/2019 as 10:09 por Acrisio
// Defini��o da classe CmdInsertGrandPrixClear

#pragma once
#ifndef _STDA_CMD_INSERT_GRAND_PRIX_CLEAR_HPP
#define _STDA_CMD_INSERT_GRAND_PRIX_CLEAR_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdInsertGrandPrixClear : public pangya_db {
		public:
			explicit CmdInsertGrandPrixClear(bool _waiter = false);
			CmdInsertGrandPrixClear(uint32_t _uid, GrandPrixClear& _gpc, bool _waiter = false);
			virtual ~CmdInsertGrandPrixClear();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			GrandPrixClear& getInfo();
			void setInfo(GrandPrixClear& _gpc);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdInsertGrandPrixClear"; };
			std::wstring _wgetName() override { return L"CmdInsertGrandPrixClear"; };

		private:
			uint32_t m_uid;
			GrandPrixClear m_gpc;

			const char* m_szConsulta[2] = { "INSERT INTO pangya.pangya_grandprix_clear(UID, TYPEID, FLAG) VALUES(", ")" };
	};
}

#endif // !_STDA_CMD_INSERT_GRAND_PRIX_CLEAR_HPP
