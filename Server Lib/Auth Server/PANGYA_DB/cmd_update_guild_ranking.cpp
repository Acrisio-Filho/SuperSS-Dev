// Arquivo cmd_update_guild_ranking.cpp
// Criado em 29/12/2019 as 15:47 por Acrisio
// Implementa��o da classe CmdUpdateGuildRanking

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_guild_ranking.hpp"

using namespace stdA;

CmdUpdateGuildRanking::CmdUpdateGuildRanking(bool _waiter) : pangya_db(_waiter) {
}

CmdUpdateGuildRanking::~CmdUpdateGuildRanking() {
}

void CmdUpdateGuildRanking::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateGuildRanking::prepareConsulta(database& _db) {
	
	auto r = procedure(_db, m_szConsulta, "");

	checkResponse(r, "Nao conseguiu atualizar Guild Ranking.");

	return r;
}
