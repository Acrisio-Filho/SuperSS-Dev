// Arquivo cmd_find_warehouse_item.hpp
// Criado em 22/05/2018 as 22:31 por Acrisio
// Defini��o da class CmdFindWarehouseItem

#pragma once
#ifndef _STDA_CMD_FIND_WAREHOUSE_ITEM_HPP
#define _STDA_CMD_FIND_WAREHOUSE_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdFindWarehouseItem : public pangya_db {
		public:
			explicit CmdFindWarehouseItem(bool _waiter = false);
			CmdFindWarehouseItem(uint32_t _uid, uint32_t _typeid, bool _waiter = false);
			virtual ~CmdFindWarehouseItem();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getTypeid();
			void setTypeid(uint32_t _typeid);

			bool hasFound();

			WarehouseItemEx& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdFindWarehouseItem"; };
			virtual std::wstring _wgetName() override { return L"CmdFindWarehouseItem"; };

		private:
			uint32_t m_uid;
			uint32_t m_typeid;
			WarehouseItemEx m_wi;

			const char* m_szConsulta = "pangya.ProcFindWarehouseItem";
	};
}

#endif // !_STDA_CMD_FIND_WAREHOUSE_ITEM_HPP
