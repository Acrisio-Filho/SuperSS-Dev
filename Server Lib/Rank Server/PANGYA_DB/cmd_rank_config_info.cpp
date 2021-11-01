// Arquivo cmd_rank_config_info.cpp
// Criado em 18/06/2020 as 21:28 por Acrisio
// Implementa��o da classe CmdRankConfigInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_rank_config_info.hpp"

using namespace stdA;

CmdRankConfigInfo::CmdRankConfigInfo(bool _waiter) : pangya_db(_waiter), m_rft() {
}

CmdRankConfigInfo::~CmdRankConfigInfo() {
}

void CmdRankConfigInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(2, (uint32_t)_result->cols);

	m_rft.setIntervalRefresh((uint32_t)IFNULL(atoi, _result->data[0]));

	if (_result->data[1] != nullptr)
		m_rft.setLastRefreshDate(_result->data[1]);
}

response* CmdRankConfigInfo::prepareConsulta(database& _db) {
	
	m_rft.clear();

	auto r = consulta(_db, m_szConsulta);

	checkResponse(r, "Nao conseguiu pegar as configuracao do Rank.");

	return r;
}

rank_refresh_time& CmdRankConfigInfo::getInfo() {
	return m_rft;
}
