// Arquivo cmd_add_item_base.hpp
// Criado em 25/03/2018 as 18:40 por Acrisio
// Defini��o da classe CmdAddItemBase

#pragma once
#ifndef _STDA_CMD_ADD_ITEM_BASE_HPP
#define _STDA_CMD_ADD_ITEM_BASE_HPP

#include "../PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdAddItemBase : public pangya_db {
		public:
			explicit CmdAddItemBase(bool _waiter = false);
			CmdAddItemBase(uint32_t _uid, unsigned char _purchase, unsigned char _gift_flag, bool _waiter = false);
			virtual ~CmdAddItemBase();

			virtual uint32_t getUID();
			virtual void setUID(uint32_t _uid);

			virtual unsigned char getPurchase();
			virtual void setPurchase(unsigned char _purchase);

			virtual unsigned char getGiftFlag();
			virtual void setGiftFlag(unsigned char _gift_flag);

		protected:
			virtual void lineResult(result_set::ctx_res* _result, uint32_t _index_result) = 0;
			virtual response* prepareConsulta(database& _db) = 0;

		protected:
			uint32_t m_uid;
			unsigned char m_purchase;
			unsigned char m_gift_flag;
	};
}

#endif // !_STDA_CMD_ADD_ITEM_BASE_HPP
