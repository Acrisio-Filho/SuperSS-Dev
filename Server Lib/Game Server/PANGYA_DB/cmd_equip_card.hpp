// Arquivo cmd_equip_card.hpp
// Criado em 17/06/2018 as 18:41 por Acrisio
// Defini��o da classe CmdEquipCard

#pragma once
#ifndef _STDA_CMD_EQUIP_CARD_HPP
#define _STDA_CMD_EQUIP_CARD_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdEquipCard : public pangya_db {
		public:
			explicit CmdEquipCard(bool _waiter = false);
			CmdEquipCard(uint32_t _uid, CardEquipInfoEx& _cei, uint32_t _tempo, bool _waiter = false);
			virtual ~CmdEquipCard();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getTempo();
			void setTempo(uint32_t _tempo);

			CardEquipInfoEx& getInfo();
			void setInfo(CardEquipInfoEx& _cei);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdEquipCard"; };
			virtual std::wstring _wgetName() override { return L"CmdEquipCard"; };

		private:
			uint32_t m_uid;
			uint32_t m_tempo;
			CardEquipInfoEx m_cei;

			const char* m_szConsulta = "pangya.ProcEquipCard";
	};
}

#endif // !_STDA_CMD_EQUIP_CARD_HPP
