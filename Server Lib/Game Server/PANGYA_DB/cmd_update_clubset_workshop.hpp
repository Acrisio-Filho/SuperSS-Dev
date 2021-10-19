// Arquivo cmd_update_clubset_workshop.hpp
// Criado em 23/06/2018 as 18:41 por Acrisio
// Defini��o da classe CmdUpdateClubSetWorkshop

#pragma once
#ifndef _STDA_CMD_UPDATE_CLUBSET_WORKSHOP_HPP
#define _STDA_CMD_UPDATE_CLUBSET_WORKSHOP_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateClubSetWorkshop : public pangya_db {
		public:
			enum FLAG : unsigned {
				F_TRANSFER_MASTERY_PTS,
				F_R_RECOVERY_PTS,
				F_UP_LEVEL,
				F_UP_LEVEL_CANCEL,
				F_UP_RANK,
				F_RESET,
			};

		public:
			explicit CmdUpdateClubSetWorkshop(bool _waiter = false);
			CmdUpdateClubSetWorkshop(uint32_t _uid, WarehouseItemEx& _wi, FLAG _flag, bool _waiter = false);
			virtual ~CmdUpdateClubSetWorkshop();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			FLAG getFlag();
			void setFlag(FLAG _flag);

			WarehouseItemEx& getInfo();
			void setInfo(WarehouseItemEx& _wi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateClubSetWorkshop"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateClubSetWorkshop"; };

		private:
			uint32_t m_uid;
			FLAG m_flag;
			WarehouseItemEx m_wi;

			const char* m_szConsulta = "pangya.ProcUpdateClubSetWorkshop";
	};
}

#endif // !_STDA_CMD_UPDATE_CLUBSET_WORKSHOP_HPP
