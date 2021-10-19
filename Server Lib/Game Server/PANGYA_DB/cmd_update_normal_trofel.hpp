// Arquivo cmd_update_normal_trofel.hpp
// Criado em 23/09/2018 as 12:21 por Acrisio
// Defini��o da classe CmdInsertNormalTrofel

#pragma once
#ifndef _STDA_CMD_UPDATE_NORMAL_TROFEL_HPP
#define _STDA_CMD_UPDATE_NORMAL_TROFEL_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateNormalTrofel : public pangya_db {
		public:
			explicit CmdUpdateNormalTrofel(bool _waiter = false);
			CmdUpdateNormalTrofel(uint32_t _uid, TrofelInfo& _ti, bool _waiter = false);
			virtual ~CmdUpdateNormalTrofel();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			TrofelInfo& getInfo();
			void setInfo(TrofelInfo& _ti);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdateNormalTrofel"; };
			std::wstring _wgetName() override { return L"CmdUpdateNormalTrofel"; };

		private:
			uint32_t m_uid;
			TrofelInfo m_ti;

			const char* m_szConsulta = "pangya.ProcUpdateTrofelNormal";
	};
}

#endif // !_STDA_CMD_UPDATE_NORMAL_TROFEL_HPP
