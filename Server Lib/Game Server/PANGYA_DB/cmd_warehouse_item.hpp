// Arquivo cmd_warehouse_item.hpp
// Criado em 18/03/2018 as 22:01 por Acrisio
// Defini��o da classe CmdWarehouseItem

#pragma once
#ifndef _STDA_CMD_WAREHOUSE_ITEM_HPP
#define _STDA_CMD_WAREHOUSE_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <map>

namespace stdA {
	class CmdWarehouseItem : public pangya_db {
		public:
			enum TYPE : unsigned char {
				ALL,
				ONE,
			};

		public:
			explicit CmdWarehouseItem(bool _waiter = false);
			CmdWarehouseItem(uint32_t _uid, TYPE _type, int32_t _item_id = -1, bool _waiter = false);
			virtual ~CmdWarehouseItem();

			std::multimap< int32_t/*ID*/, WarehouseItemEx >& getInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			TYPE getType();
			void setType(TYPE _type);

			int32_t getItemID();
			void setItemID(int32_t _item_id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdWarehouseItem"; };
			virtual std::wstring _wgetName() override { return L"CmdWarehouseItem"; };

		private:
			uint32_t m_uid;
			TYPE m_type;
			int32_t m_item_id;
			std::multimap< int32_t/*ID*/, WarehouseItemEx > v_wi;

			const char* m_szConsulta[2] = { "pangya.ProcGetWarehouseItem", "pangya.ProcGetWarehouseItem_One" };
	};
}

#endif // !_STDA_CMD_WAREHOUSE_ITEM_HPP
