// Arquivo cmd_papel_shop_coupon.hpp
// Criado em 09/07/2018 as 20:11 por Acrisio
// Defini��o da classe CmdPapelShopCoupon

#pragma once
#ifndef _STDA_CMD_PAPEL_SHOP_COUPON_HPP
#define _STDA_CMD_PAPEL_SHOP_COUPON_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/papel_shop_type.hpp"
#include <map>

namespace stdA {
	class CmdPapelShopCoupon : public pangya_db {
		public:
			explicit CmdPapelShopCoupon(bool _waiter = false);
			virtual ~CmdPapelShopCoupon();

			std::map< uint32_t, ctx_papel_shop_coupon >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdPapelShopCoupon"; };
			virtual std::wstring _wgetName() override { return L"CmdPapelShopCoupon"; };

		private:
			std::map< uint32_t, ctx_papel_shop_coupon > m_ctx_psc;

			const char* m_szConsulta = "SELECT typeid, active FROM pangya.pangya_papel_shop_coupon";
	};
}

#endif // !_STDA_CMD_PAPEL_SHOP_COUPON_HPP
