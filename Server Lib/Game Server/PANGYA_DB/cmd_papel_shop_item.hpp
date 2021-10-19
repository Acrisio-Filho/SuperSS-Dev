// Arquivo cmd_papel_shop_item.hpp
// Criado em 09/07/2018 as 20:27 por Acrisio
// Defini��o da classe CmdPapelShopItem

#pragma once
#ifndef _STDA_CMD_PAPEL_SHOP_ITEM_HPP
#define _STDA_CMD_PAPEL_SHOP_ITEM_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/papel_shop_type.hpp"
#include <vector>

namespace stdA {
	class CmdPapelShopItem : public pangya_db {
		public:
			explicit CmdPapelShopItem(bool _waiter = false);
			virtual ~CmdPapelShopItem();

			std::vector< ctx_papel_shop_item >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdPapelShopItem"; };
			virtual std::wstring _wgetName() override { return L"CmdPapelShopItem"; };

		private:
			std::vector< ctx_papel_shop_item > m_ctx_psi;

			const char* m_szConsulta = "SELECT typeid, probabilidade, numero, tipo, active FROM pangya.pangya_papel_shop_item";
	};
}

#endif // !_STDA_CMD_PAPEL_SHOP_ITEM_HPP
