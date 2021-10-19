// Arquivo cmd_register_guild_match.cpp
// Criado em 29/12/2019 as 12:12 por Acrisio
// Implementa��o da classe CmdRegisterGuildMatch

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_register_guild_match.hpp"

using namespace stdA;

CmdRegisterGuildMatch::CmdRegisterGuildMatch(bool _waiter) : pangya_db(_waiter), m_match{0u} {
}

CmdRegisterGuildMatch::CmdRegisterGuildMatch(GuildMatch& _match, bool _waiter)
	: pangya_db(_waiter), m_match(_match) {
}

CmdRegisterGuildMatch::~CmdRegisterGuildMatch() {
}

void CmdRegisterGuildMatch::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um INSERT
	return;
}

response* CmdRegisterGuildMatch::prepareConsulta(database& _db) {
	
	if (m_match.uid[0] == 0u || m_match.uid[1] == 0u)
		throw exception("[CmdRegisterGuildMatch::prepareConsulta][Error] Guild[0][VALUE=" + std::to_string(m_match.uid[0]) 
				+ "] ou Guild[1][VALUE=" + std::to_string(m_match.uid[1]) + "] is invalid(zero). Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_match.uid[0]) + ", " + std::to_string(m_match.point[0]) + ", " + std::to_string(m_match.pang[0])
				+ ", " + std::to_string(m_match.uid[1]) + ", " + std::to_string(m_match.point[1]) + ", " + std::to_string(m_match.pang[1]));

	checkResponse(r, "nao conseguiu registrar Guild Match[GUILD_1_UID=" + std::to_string(m_match.uid[0]) + ", GUILD_1_POINT=" + std::to_string(m_match.point[0]) 
			+ ", GUILD_1_PANG=" + std::to_string(m_match.pang[0]) + ", GUILD_2_UID=" + std::to_string(m_match.uid[1]) + ", GUILD_2_POINT="
			+ std::to_string(m_match.point[1]) + ", GUILD_2_PANG=" + std::to_string(m_match.pang[1]) + "];");

	return r;
}

GuildMatch& CmdRegisterGuildMatch::getInfo() {
	return m_match;
}

void CmdRegisterGuildMatch::setInfo(GuildMatch& _match) {
	m_match = _match;
}
