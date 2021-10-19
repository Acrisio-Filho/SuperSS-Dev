// Arquivo cmd_guild_update_activity_info.cpp
// Criado em 30/11/2019 as 16:25 por Acrisio
// Implementa��o da classe CmdGuildUpdateActivityInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_guild_update_activity_info.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdGuildUpdateActivityInfo::CmdGuildUpdateActivityInfo(bool _waiter) : pangya_db(_waiter), m_guild_uid(0u), m_member_uid(0u), m_info() {
}

CmdGuildUpdateActivityInfo::CmdGuildUpdateActivityInfo(uint32_t _guild_uid, uint32_t _member_uid, bool _waiter)
	: pangya_db(_waiter), m_guild_uid(_guild_uid), m_member_uid(_member_uid), m_info() {
}

CmdGuildUpdateActivityInfo::~CmdGuildUpdateActivityInfo() {

	// Clear, free memory
	if (!m_info.empty()) {
		m_info.clear();
		m_info.shrink_to_fit();
	}
}

void CmdGuildUpdateActivityInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(6, (uint32_t)_result->cols);

	GuildUpdateActivityInfo guai{ 0 };

	guai.index = (uint64_t)IFNULL(atoll, _result->data[0]);
	guai.club_uid = (uint32_t)IFNULL(atoi, _result->data[1]);
	guai.owner_uid = (uint32_t)IFNULL(atoi, _result->data[2]);
	guai.player_uid = (uint32_t)IFNULL(atoi, _result->data[3]);
	guai.type = GuildUpdateActivityInfo::TYPE_UPDATE(IFNULL(atoi, _result->data[4]));

	if (_result->data[5] != nullptr)
		guai.reg_date = StrToUnix(_result->data[5]);

	if (guai.club_uid != m_guild_uid)
		throw exception("[CmdGuildUpdateActivityInfo::lineResult][Error] guild_uid requisitado eh diferente do retornado pela consulta. QUERY_VALUES[GUILD_UID_REQ=" 
				+ std::to_string(m_guild_uid) + ", GUILD_UID_RET=" + std::to_string(guai.club_uid) + "].", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));

	if (guai.owner_uid != m_member_uid)
		throw exception("[CmdGuildUpdateActivityInfo::lineResult][Error] owner_uid requisitado eh diferente do retornado pela consulta. QUERY_VALUES[OWNER_UID_REQ="
				+ std::to_string(m_member_uid) + ", OWNER_UID_RET=" + std::to_string(guai.owner_uid) + "].", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));

	// Add para o vector
	m_info.push_back(guai);
}

response* CmdGuildUpdateActivityInfo::prepareConsulta(database& _db) {

	if (m_guild_uid == 0u)
		throw exception("[CmdGuildUpdateActivityInfo::prepareConsulta][Error] m_guild_uid is invalid(zero).", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_member_uid == 0u)
		throw exception("[CmdGuildUpdateActivityInfo::prepareConsulta][Error] m_member_uid is invalid(zero).",
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (!m_info.empty())
		m_info.clear();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_guild_uid) + ", " + std::to_string(m_member_uid));

	checkResponse(r, "nao conseguiu pegar a Update Activity do Member[UID=" + std::to_string(m_member_uid) 
			+ "] da Guild[UID=" + std::to_string(m_guild_uid) + "]");

	return r;
}

uint32_t CmdGuildUpdateActivityInfo::getGuildUID() {
	return m_guild_uid;
}

void CmdGuildUpdateActivityInfo::setGuildUID(uint32_t _uid) {
	m_guild_uid = _uid;
}

uint32_t CmdGuildUpdateActivityInfo::getMemberUID() {
	return m_member_uid;
}

void CmdGuildUpdateActivityInfo::setMemberUID(uint32_t _member_uid) {
	m_member_uid = _member_uid;
}

std::vector< GuildUpdateActivityInfo >& CmdGuildUpdateActivityInfo::getInfo() {
	return m_info;
}
