// Arquivo cmd_update_counter_item.hpp
// Criado em 15/04/2018 as 19:56 por Acrisio
// Defini��o da classe CmdUpdateCounterItem

#pragma once
#ifndef _STDA_CMD_UPDATE_COUNTER_ITEM_HPP
#define _STDA_CMD_UPDATE_COUNTER_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateCounterItem : public pangya_db {
		public:
			explicit CmdUpdateCounterItem(bool _waiter = false);
			CmdUpdateCounterItem(uint32_t _uid, bool _waiter = false);
			CmdUpdateCounterItem(uint32_t _uid, CounterItemInfo& _cii, bool _waiter = false);
			virtual ~CmdUpdateCounterItem();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			CounterItemInfo& getInfo();
			void setInfo(CounterItemInfo& _cii);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateCounterItem"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateCounterItem"; };

		private:
			uint32_t m_uid;
			CounterItemInfo m_cii;

			const char* m_szConsulta[3] = { "UPDATE pangya.pangya_counter_item SET count_num_item = ", " WHERE UID = ", " AND count_id = " };
	};
}

#endif // !_STDA_CMD_UPDATE_COUNTER_ITEM_HPP
