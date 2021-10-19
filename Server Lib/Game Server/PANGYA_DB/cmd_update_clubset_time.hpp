// Arquivo cmd_update_clubset_time.hpp
// Criado em 01/11/2020 as 02:56 por Acrisio
// Defini��o da classe CmdUpdateClubSetTime

#pragma once
#ifndef _STDA_CMD_UPDATE_CLUBSET_TIME_HPP
#define _STDA_CMD_UPDATE_CLUBSET_TIME_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {

	class CmdUpdateClubSetTime : public pangya_db {

		public:
			CmdUpdateClubSetTime(uint32_t _uid, WarehouseItemEx& _wi, bool _waiter = false);
			CmdUpdateClubSetTime(bool _waiter = false);
			virtual ~CmdUpdateClubSetTime();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			WarehouseItemEx& getClubSet();
			void setClubSet(WarehouseItemEx& _wi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdUpdateClubSetTime"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateClubSetTime"; };

		private:
			uint32_t m_uid;
			WarehouseItemEx m_wi;

			const char* m_szConsulta = "pangya.ProcUpdateClubSetTime";
	};
}

#endif // !_STDA_CMD_UPDATE_CLUBSET_TIME_HPP
