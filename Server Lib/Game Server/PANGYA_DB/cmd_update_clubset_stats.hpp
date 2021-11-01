// Arquivo cmd_udpate_clubset_stats.hpp
// Criado em 16/06/2018 as 21:58 por Acrisio
// Defini��o da classe CmdUpdateClubSetStats

#pragma once
#ifndef _STDA_CMD_UPDATE_CLUBSET_STATS_HPP
#define _STDA_CMD_UPDATE_CLUBSET_STATS_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateClubSetStats : public pangya_db {
		public:
			explicit CmdUpdateClubSetStats(bool _waiter = false);
			CmdUpdateClubSetStats(uint32_t _uid, WarehouseItemEx& _wi, uint64_t _pang, bool _waiter = false);
			virtual ~CmdUpdateClubSetStats();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint64_t getPang();
			void setPang(uint64_t _pang);

			WarehouseItemEx& getInfo();
			void setInfo(WarehouseItemEx& _wi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateClubSetStats"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateClubSetStats"; };

		private:
			uint32_t m_uid;
			uint64_t m_pang;
			WarehouseItemEx m_wi;

			const char* m_szConsulta = "pangya.ProcUpdateClubSetStats";
	};
}

#endif // !_STDA_CMD_UPDATE_CLUBSET_STATS_HPP
