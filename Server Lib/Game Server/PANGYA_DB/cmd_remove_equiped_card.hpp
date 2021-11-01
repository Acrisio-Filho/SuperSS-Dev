// Arquivo cmd_remove_equiped_card.hpp
// Criado em 17/06/2018 as 21:15 por Acrisio
// Defini��o da classe CmdRemoveEquipedCard

#pragma once
#ifndef _STDA_CMD_REMOVE_EQUIPED_CARD_HPP
#define _STDA_CMD_REMOVE_EQUIPED_CARD_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdRemoveEquipedCard : public pangya_db {
		public:
			explicit CmdRemoveEquipedCard(bool _waiter = false);
			CmdRemoveEquipedCard(uint32_t _uid, CardEquipInfo& _cei, bool _waiter = false);
			virtual ~CmdRemoveEquipedCard();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			CardEquipInfo& getInfo();
			void setInfo(CardEquipInfo& _cei);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdRemoveEquipedCard"; };
			virtual std::wstring _wgetName() override { return L"CmdRemoveEquipedCard"; };

		private:
			uint32_t m_uid;
			CardEquipInfo m_cei;

			const char* m_szConsulta = "pangya.ProcRemoveEquipedCard";
	};
}

#endif // !_STDA_CMD_REMOVE_EQUIPED_CARD_HPP
