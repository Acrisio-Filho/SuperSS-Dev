// Arquivo cmd_udpdate_papel_shop_config.hpp
// Criado em 08/12/2018 as 15:09 por Acrisio
// Defini��o da classe CmdUpdatePapelShopConfig

#pragma once
#ifndef _STDA_CMD_UPDATE_PAPEL_SHOP_CONFIG_HPP
#define _STDA_CMD_UPDATE_PAPEL_SHOP_CONFIG_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/papel_shop_type.hpp"

namespace stdA {
	class CmdUpdatePapelShopConfig : public pangya_db {
		public:
			explicit CmdUpdatePapelShopConfig(bool _waiter = false);
			CmdUpdatePapelShopConfig(ctx_papel_shop& _ps, bool _waiter = false);
			virtual ~CmdUpdatePapelShopConfig();

			ctx_papel_shop& getInfo();
			void setInfo(ctx_papel_shop& _ps);

			bool isUpdated();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdatePapelShopConfig"; };
			std::wstring _wgetName() override { return L"CmdUpdatePapelShopConfig"; };

		private:
			ctx_papel_shop m_ps;
			bool m_updated;

			const char* m_szConsulta = "pangya.ProcUpdatePapelShopConfig";
	};
}

#endif // !_STDA_CMD_UPDATE_PAPEL_SHOP_CONFIG_HPP
