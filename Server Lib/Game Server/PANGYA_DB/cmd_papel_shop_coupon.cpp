// Arquivo cmd_papel_shop_coupon.cpp
// Criado em 09/07/2018 as 20:16 por Acrisio
// Implementa��o da classe CmdPapelShopCoupon

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_papel_shop_coupon.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

using namespace stdA;

CmdPapelShopCoupon::CmdPapelShopCoupon(bool _waiter) : pangya_db(_waiter), m_ctx_psc() {
}

CmdPapelShopCoupon::~CmdPapelShopCoupon() {
}

void CmdPapelShopCoupon::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(2, (uint32_t)_result->cols);

	ctx_papel_shop_coupon ctx_psc{ 0 };

	ctx_psc._typeid = IFNULL(atoi, _result->data[0]);
	ctx_psc.active = (unsigned char)IFNULL(atoi, _result->data[1]);

	auto it = m_ctx_psc.find(ctx_psc._typeid);

	if (it == m_ctx_psc.end())	// N�o tem add um novo coupon
		m_ctx_psc[ctx_psc._typeid] = ctx_psc;
	else	// J� tem um coupon no map, est� duplicado no banco de dados
		_smp::message_pool::getInstance().push(new message("[CmdPapelShopCoupon::lineResult][WARNING] tem Papel Shop Coupon[TYPEID=" + std::to_string(ctx_psc._typeid) + "] duplicado no banco de dados.", CL_FILE_LOG_AND_CONSOLE));
}

response* CmdPapelShopCoupon::prepareConsulta(database& _db) {

	auto r = consulta(_db, m_szConsulta);

	checkResponse(r, "nao conseguiu pegar os papel shop coupon(s).");

	return r;
}

std::map< uint32_t, ctx_papel_shop_coupon >& CmdPapelShopCoupon::getInfo() {
	return m_ctx_psc;
}
