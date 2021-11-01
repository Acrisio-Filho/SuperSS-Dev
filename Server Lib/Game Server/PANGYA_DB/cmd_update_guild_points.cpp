// Arquivo cmd_update_guild_points.cpp
// Criado em 29/12/2019 as 12:40 por Acrisio
// Implementa��o da classe CmdUpdateGuildPoints

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_guild_points.hpp"

using namespace stdA;

CmdUpdateGuildPoints::CmdUpdateGuildPoints(bool _waiter) : pangya_db(_waiter), m_gp{0u} {
}

CmdUpdateGuildPoints::CmdUpdateGuildPoints(GuildPoints& _gp, bool _waiter)
	: pangya_db(_waiter), m_gp(_gp) {
}

CmdUpdateGuildPoints::~CmdUpdateGuildPoints() {
}

void CmdUpdateGuildPoints::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateGuildPoints::prepareConsulta(database& _db) {
	
	if (m_gp.uid == 0u)
		throw exception("[CmdUpdateGuildPoints::prepareConsulta][Error] m_gp.uid is invalid(zero). Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_gp.uid) + ", " + std::to_string(m_gp.point) + ", " + std::to_string(m_gp.pang) + ", " + std::to_string((unsigned short)m_gp.win));

	checkResponse(r, "nao conseguiu atualizar os Pontos[POINT=" + std::to_string(m_gp.point) + ", PANG=" 
			+ std::to_string(m_gp.pang) + "] da Guild[UID=" + std::to_string(m_gp.uid) + ", WIN=" + std::to_string((unsigned short)m_gp.win) + "]");

	return r;
}

GuildPoints& CmdUpdateGuildPoints::getInfo() {
	return m_gp;
}

void CmdUpdateGuildPoints::setInfo(GuildPoints& _gp) {
	m_gp = _gp;
}
