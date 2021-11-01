// Arquivo cmd_update_guild_member_points.cpp
// Criado em 29/12/2019 as 13:09 por Acrisio
// Implementa��o da classe CmdUpdateGuildMemberPoints

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_guild_member_points.hpp"

using namespace stdA;

CmdUpdateGuildMemberPoints::CmdUpdateGuildMemberPoints(bool _waiter) : pangya_db(_waiter), m_gmp{0u} {
}

CmdUpdateGuildMemberPoints::CmdUpdateGuildMemberPoints(GuildMemberPoints& _gmp, bool _waiter)
	: pangya_db(_waiter), m_gmp(_gmp) {
}

CmdUpdateGuildMemberPoints::~CmdUpdateGuildMemberPoints() {
}

void CmdUpdateGuildMemberPoints::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateGuildMemberPoints::prepareConsulta(database& _db) {
	
	if (m_gmp.guild_uid == 0u)
		throw exception("[CmdUpdateGuildMemberPoints::prepareConsulta][Error] m_gmp.guild_uid is invalid(zero). Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_gmp.member_uid == 0u)
		throw exception("[CmdUpdateGuildMemberPoints::prepareConsulta][Error] m_gmp.member_uid is invalid(zero). Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_gmp.guild_uid) + ", " + std::to_string(m_gmp.member_uid)
			+ ", " + std::to_string(m_gmp.point) + ", " + std::to_string(m_gmp.pang));

	checkResponse(r, "nao conseguiu atualizar o Guild[UID=" + std::to_string(m_gmp.guild_uid) + "] POINTS[POINT=" 
			+ std::to_string(m_gmp.point) + ", PANG=" + std::to_string(m_gmp.pang) + "] do player[UID=" + std::to_string(m_gmp.member_uid) + "]");

	return r;
}

GuildMemberPoints& CmdUpdateGuildMemberPoints::getInfo() {
	return m_gmp;
}

void CmdUpdateGuildMemberPoints::setInfo(GuildMemberPoints& _gmp) {
	m_gmp = _gmp;
}
