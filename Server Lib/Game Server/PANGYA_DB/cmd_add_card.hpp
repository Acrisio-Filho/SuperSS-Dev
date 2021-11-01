// Arquivo cmd_add_card.hpp
// Criado em 19/05/2018 as 15:23 por Acrisio
// Defini��o da classe CmdAddCard

#pragma once
#ifndef _STDA_CMD_ADD_CARD_HPP
#define _STDA_CMD_ADD_CARD_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdAddCard : public pangya_db {
		public:
			explicit CmdAddCard(bool _waiter = false);
			CmdAddCard(uint32_t _uid, CardInfo& _ci, unsigned char _purchase, unsigned char _gift_flag, bool _waiter = false);
			virtual ~CmdAddCard();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			CardInfo& getInfo();
			void setInfo(CardInfo& _ci);

			unsigned char getGiftFlag();
			void setGiftFlag(unsigned char _gift_flag);

			unsigned char getPurchase();
			void setPurchase(unsigned char _purchase);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAddCard"; };
			virtual std::wstring _wgetName() override { return L"CmdAddCard"; };

		private:
			uint32_t m_uid;
			unsigned char m_gift_flag;
			unsigned char m_purchase;
			CardInfo m_ci;

			const char* m_szConsulta = "pangya.ProcInsertCard";
	};
}

#endif // !_STDA_CMD_ADD_CARD_HPP
