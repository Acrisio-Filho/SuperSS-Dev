// Arquivo cmd_update_papel_shop_config.cpp
// Criado em 08/12/2018 as 15:16 por Acrisio
// Implementa��o da classe CmdUpdatePapelShopConfig

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_papel_shop_config.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdUpdatePapelShopConfig::CmdUpdatePapelShopConfig(bool _waiter) : pangya_db(_waiter), m_ps{ 0 }, m_updated(false) {
}

CmdUpdatePapelShopConfig::CmdUpdatePapelShopConfig(ctx_papel_shop& _ps, bool _waiter) 
	: pangya_db(_waiter), m_ps(_ps), m_updated(false) {
}

CmdUpdatePapelShopConfig::~CmdUpdatePapelShopConfig() {
}

void CmdUpdatePapelShopConfig::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(6, (uint32_t)_result->cols);

	// Update ON DB
	m_updated = IFNULL(atoi, _result->data[0]) == 1 ? true : false;

	if (!m_updated) { // Não atualizou, pega os valores atualizados do banco de dados
		
		m_ps.numero = IFNULL(atoi, _result->data[1]);
		m_ps.price_normal = IFNULL(atoll, _result->data[2]);
		m_ps.price_big = IFNULL(atoll, _result->data[3]);
		m_ps.limitted_per_day = (unsigned char)IFNULL(atoi, _result->data[4]);

		if (_result->data[5] != nullptr)
			_translateDate(_result->data[5], &m_ps.update_date);
	}
	
	return;
}

response* CmdUpdatePapelShopConfig::prepareConsulta(database& _db) {

	m_updated = false;

	std::string upt_dt = "null";

	if (!isEmpty(m_ps.update_date))
		upt_dt = _db.makeText(_formatDate(m_ps.update_date));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_ps.numero) + ", " + std::to_string(m_ps.price_normal)
			+ ", " + std::to_string(m_ps.price_big) + ", " + std::to_string((unsigned short)m_ps.limitted_per_day)
			+ ", " + upt_dt
	);

	checkResponse(r, "nao conseguiu atualizar o Papel Shop Config[" + m_ps.toString() + "]");

	return r;
}

ctx_papel_shop& CmdUpdatePapelShopConfig::getInfo() {
	return m_ps;
}

void CmdUpdatePapelShopConfig::setInfo(ctx_papel_shop& _ps) {
	m_ps = _ps;
}

bool CmdUpdatePapelShopConfig::isUpdated() {
	return m_updated;
}
