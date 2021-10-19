// Arquivo cmd_papel_shop_config.cpp
// Criado em 09/07/2018 as 20:00 por Acrisio
// Implementa��o da classe CmdPapelShopConfig

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_papel_shop_config.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdPapelShopConfig::CmdPapelShopConfig(bool _waiter) : pangya_db(_waiter), m_ctx_ps{0} {
}

CmdPapelShopConfig::~CmdPapelShopConfig() {
}

void CmdPapelShopConfig::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(5, (uint32_t)_result->cols);

	m_ctx_ps.numero = IFNULL(atoi, _result->data[0]);
	m_ctx_ps.price_normal = IFNULL(atoll, _result->data[1]);
	m_ctx_ps.price_big = IFNULL(atoll, _result->data[2]);
	m_ctx_ps.limitted_per_day = (unsigned char)IFNULL(atoi, _result->data[3]);

	if (_result->data[4] != nullptr)
		_translateDate(_result->data[4], &m_ctx_ps.update_date);
}

response* CmdPapelShopConfig::prepareConsulta(database& _db) {

	auto r = consulta(_db, m_szConsulta);

	checkResponse(r, "nao conseguiu pegar o papel shop config.");

	return r;
}

ctx_papel_shop& CmdPapelShopConfig::getInfo() {
	return m_ctx_ps;
}
