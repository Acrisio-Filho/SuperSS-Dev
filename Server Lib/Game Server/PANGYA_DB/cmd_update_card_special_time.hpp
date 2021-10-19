// Arquivo cmd_update_card_special_time.hpp
// Criado em 07/07/2018 as 22:30 por Acrisio
// Defini��o da classe CmdUpdateCardSpecialTime

#pragma once
#ifndef _STDA_CMD_UPDATE_CARD_SPECIAL_TIME_HPP
#define _STDA_CMD_UPDATE_CARD_SPECIAL_TIME_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateCardSpecialTime : public pangya_db {
		public:
			explicit CmdUpdateCardSpecialTime(bool _waiter = false);
			CmdUpdateCardSpecialTime(uint32_t _uid, CardEquipInfoEx& _cei, bool _waiter = false);
			virtual ~CmdUpdateCardSpecialTime();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			CardEquipInfoEx& getInfo();
			void setInfo(CardEquipInfoEx& _cei);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateCardSpecialTime"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateCardSpecialTime"; };

		private:
			uint32_t m_uid;
			CardEquipInfoEx m_cei;

			const char* m_szConsulta = "pangya.ProcUpdateCardSpecialTime";
	};
}

#endif // !_STDA_CMD_UPDATE_CARD_SPECIAL_TIME_HPP
