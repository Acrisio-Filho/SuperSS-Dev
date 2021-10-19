// Arquivo cmd_personal_shop_log.hpp
// Criado em 10/06/2018 as 09:10 por Acrisio
// Defini��o da classe CmdPersonalShopLog

#pragma once
#ifndef _STDA_CMD_PERSONAL_SHOP_LOG_HPP
#define _STDA_CMD_PERSONAL_SHOP_LOG_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdPersonalShopLog : public pangya_db {
		public:
			explicit CmdPersonalShopLog(bool _waiter = false);
			CmdPersonalShopLog(uint32_t _sell_uid, uint32_t _buy_uid, PersonalShopItem& _psi, int32_t _item_id_buy, bool _waiter = false);
			virtual ~CmdPersonalShopLog();

			uint32_t getUIDSell();
			void setUIDSell(uint32_t _sell_uid);

			uint32_t getUIDBuy();
			void setUIDBuy(uint32_t _buy_uid);

			int32_t getItemIDBuy();
			void setItemIDBuy(int32_t _item_id_buy);

			PersonalShopItem& getItemSell();
			void setItemSell(PersonalShopItem& _psi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdPersonalShopLog"; };
			virtual std::wstring _wgetName() override { return L"CmdPersonalShopLog"; };

		private:
			uint32_t m_uid_sell;
			uint32_t m_uid_buy;
			int32_t m_item_id_buy;
			PersonalShopItem m_psi;

			const char* m_szConsulta = "pangya.ProcInsertPersonalShopLog";
	};
}

#endif // !_STDA_CMD_PERSONAL_SHOP_LOG_HPP
