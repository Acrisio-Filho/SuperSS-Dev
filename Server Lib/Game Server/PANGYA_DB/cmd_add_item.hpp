// Arquivo cmd_add_item.hpp
// Criado em 31/05/2018 as 10:17 por Acrisio
// Defini��o da classe CmdAddItem

#pragma once
#ifndef _STDA_CMD_ADD_ITEM_HPP
#define _STDA_CMD_ADD_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdAddItem : public pangya_db {
		public:
			explicit CmdAddItem(bool _waiter = false);
			CmdAddItem(uint32_t _uid, WarehouseItemEx& _wi, unsigned char _purchase, unsigned char _gift_flag, bool _waiter = false);
			virtual ~CmdAddItem();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			unsigned char getPurchase();
			void setPurchase(unsigned char _purchase);

			unsigned char getGiftFlag();
			void setGiftFlag(unsigned char _gift_flag);

			WarehouseItemEx& getInfo();
			void setInfo(WarehouseItemEx& _wi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAddItem"; };
			virtual std::wstring _wgetName() override { return L"CmdAddItem"; };

		private:
			uint32_t m_uid;
			unsigned char m_purchase;
			unsigned char m_gift_flag;
			WarehouseItemEx m_wi;

			const char* m_szConsulta = "pangya.ProcAddItem";
	};
}

#endif // !_STDA_CMD_ADD_ITEM_HPP
