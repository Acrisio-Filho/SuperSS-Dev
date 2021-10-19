// Arquivo cmd_papel_shop_config.hpp
// Criado em 09/07/2018 as 19:50 por Acrisio
// Defini��o da classe CmdPapelShopConfig

#pragma once
#ifndef _STDA_CMD_PAPEL_SHOP_CONFIG_HPP
#define _STDA_CMD_PAPEL_SHOP_CONFIG_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/papel_shop_type.hpp"

namespace stdA {
	class CmdPapelShopConfig : public pangya_db {
		public:
			explicit CmdPapelShopConfig(bool _waiter = false);
			virtual ~CmdPapelShopConfig();

			ctx_papel_shop& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdPapelShopConfig"; };
			virtual std::wstring _wgetName() override { return L"CmdPapelShopConfig"; };

		private:
			ctx_papel_shop m_ctx_ps;

			const char* m_szConsulta = "SELECT Numero, Price_Normal, Price_Big, Limitted_YN, Update_Date FROM pangya.pangya_papel_shop_config";
	};
}

#endif // !_STDA_CMD_PAPEL_SHOP_CONFIG_HPP
