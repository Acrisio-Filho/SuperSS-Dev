// Arquivo cmd_guild_ranking_update_time.cpp
// Criado em 29/12/2019 as 16:03 por Acrisio
// Implementa��o da classe CmdGuildRankingUpdateTime

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_guild_ranking_update_time.hpp"

#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdGuildRankingUpdateTime::CmdGuildRankingUpdateTime(bool _waiter) : pangya_db(_waiter), m_si{0} {
}

CmdGuildRankingUpdateTime::~CmdGuildRankingUpdateTime() {
}

void CmdGuildRankingUpdateTime::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	if (_result->data[0] != nullptr)
		_translateDate(_result->data[0], &m_si);
}

response* CmdGuildRankingUpdateTime::prepareConsulta(database& _db) {
	
	auto r = procedure(_db, m_szConsulta, "");

	checkResponse(r, "Nao conseguiu pegar a date em que o Guild Ranking foi atualizado.");
	
	return r;
}

SYSTEMTIME& CmdGuildRankingUpdateTime::getTime() {
	return m_si;
}
