// Arquivo cmd_delete_counter_item.hpp
// Criado em 14/04/2018 as 14:14 por Acrisio
// Defini��o da classe CmdDeleteCounterItem

#pragma once
#ifndef _STDA_CMD_DELETE_COUNTER_ITEM_HPP
#define _STDA_CMD_DELETE_COUNTER_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

#include <vector>
#include <map>

namespace stdA {
	class CmdDeleteCounterItem : public pangya_db {
		public:
			explicit CmdDeleteCounterItem(bool _waiter = false);
			CmdDeleteCounterItem(uint32_t _uid, int32_t _id, bool _waiter = false);
			CmdDeleteCounterItem(uint32_t _uid, std::vector< int32_t >& _v_id, bool _waiter = false);
			CmdDeleteCounterItem(uint32_t _uid, std::map< int32_t, CounterItemInfo >& _mp_id, bool _waiter = false);
			virtual ~CmdDeleteCounterItem();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getId();
			std::vector< int32_t >& getIds();
			void setId(int32_t _id);
			void setId(std::vector< int32_t >& _v_id);
			void setId(std::map< int32_t, CounterItemInfo >& _mp_id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdDeleteCounterItem"; };
			virtual std::wstring _wgetName() override { return L"CmdDeleteCounterItem"; };

		private:
			uint32_t m_uid;
			std::vector< int32_t > v_id;

			const char* m_szConsulta[3] = { "DELETE FROM pangya.pangya_counter_item WHERE UID = ", " AND Count_ID IN(", ")" };
	};
}

#endif // !_STDA_CMD_DELETE_COUNTER_ITEM_HPP
