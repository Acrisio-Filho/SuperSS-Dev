// Arquivo cmd_update_rank_registry.cpp
// Criado em 18/06/2020 as 23:00 por Acrisio
// Implementa��o da classe CmdUpdateRankRegistry

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_rank_registry.hpp"

using namespace stdA;

CmdUpdateRankRegistry::CmdUpdateRankRegistry(bool _waiter) : pangya_db(_waiter), m_ret_state(0u), m_date("") {
}

CmdUpdateRankRegistry::~CmdUpdateRankRegistry() {
}

void CmdUpdateRankRegistry::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	if (_result->cols == 1)
		m_ret_state = (uint32_t)IFNULL(atoi, _result->data[0]); // Error
	else if (_result->cols == 2) {

		m_ret_state = (uint32_t)IFNULL(atoi, _result->data[0]);
		
		if (_result->data[1] != nullptr)
			m_date = _result->data[1];

	}else
		checkColumnNumber(1, (uint32_t)_result->cols); // S� para enviar a exception, por que a consulta retornou n�mero de colunas inv�lidas
}

response* CmdUpdateRankRegistry::prepareConsulta(database& _db) {
	
	m_ret_state = 0u;
	m_date = "";

	auto r = procedure(_db, m_szConsulta, "");

	checkResponse(r, "Nao conseguiu atualizar os registro do Rank no banco de dados.");

	return r;
}

uint32_t& CmdUpdateRankRegistry::getRetState() {
	return m_ret_state;
}

std::string& CmdUpdateRankRegistry::getDate() {
	return m_date;
}
