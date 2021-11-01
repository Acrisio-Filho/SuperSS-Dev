// Arquivo cmd_add_warehouse_item.hpp
// Criado em 25/03/2018 as 19:26 por Acrisio
// Defini��o da classe CmdAddWarehouseItem

#pragma once
#ifndef _STDA_CMD_ADD_WAREHOUSE_ITEM_HPP
#define _STDA_CMD_ADD_WAREHOUSE_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/cmd_add_item_base.hpp"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdAddWarehouseItem : public CmdAddItemBase {
		public:
			explicit CmdAddWarehouseItem(bool _waiter = false);
			CmdAddWarehouseItem(uint32_t _uid, WarehouseItemEx& _wi, unsigned char _purchase, unsigned char _gift_flag, unsigned short _flag_iff, bool _waiter = false);
			virtual ~CmdAddWarehouseItem();

			WarehouseItemEx& getInfo();
			void setInfo(WarehouseItemEx& _wi);

			unsigned short getFlagIff();
			void setFlagIff(unsigned short _flag_iff);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAddWarehouseItem"; };
			virtual std::wstring _wgetName() override { return L"CmdAddWarehouseItem"; };

		private:
			unsigned short m_flag_iff;
			WarehouseItemEx m_wi;

			const char* m_szConsulta = "pangya.USP_ADD_ITEM";
	};
}

#endif // !_STDA_CMD_ADD_WAREHOUSE_ITEM_HPP
