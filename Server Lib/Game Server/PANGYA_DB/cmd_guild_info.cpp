// Arquivo cmd_guild_info.cpp
// Criado em 24/03/2018 as 20:23 por Acrisio
// Implementa��o da classe CmdGuildInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_guild_info.hpp"

using namespace stdA;

CmdGuildInfo::CmdGuildInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_option(0), m_gi{0} {
}

CmdGuildInfo::CmdGuildInfo(uint32_t _uid, uint32_t _option, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_option(_option), m_gi{0} {
}

CmdGuildInfo::~CmdGuildInfo() {
}

void CmdGuildInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(13, (uint32_t)_result->cols);

	m_gi.uid = IFNULL(atoi, _result->data[0]);
	
	if (_result->data[1] != nullptr)
#if defined(_WIN32)
		memcpy_s(m_gi.name, sizeof(m_gi.name), _result->data[1], sizeof(m_gi.name));
#elif defined(__linux__)
		memcpy(m_gi.name, _result->data[1], sizeof(m_gi.name));
#endif

	if (_result->data[3] != nullptr)
#if defined(_WIN32)
		memcpy_s(m_gi.mark_emblem, sizeof(m_gi.mark_emblem), _result->data[3], sizeof(m_gi.mark_emblem));
#elif defined(__linux__)
		memcpy(m_gi.mark_emblem, _result->data[3], sizeof(m_gi.mark_emblem));
#endif
	
	m_gi.index_mark_emblem = (uint32_t)IFNULL(atoi, _result->data[4]); // Guild Mark index esse � usado no JP
	
	m_gi.point = IFNULL(atoi, _result->data[7]);
	m_gi.pang = IFNULL(atoi, _result->data[8]);
}

response* CmdGuildInfo::prepareConsulta(database& _db) {

	m_gi.clear();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_option));

	checkResponse(r, "nao conseguiu pegar o guild info do player: " + std::to_string(m_uid) + " na option: " + std::to_string(m_option));

	return r;
}

GuildInfoEx& CmdGuildInfo::getInfo() {
	return m_gi;
}

void CmdGuildInfo::setInfo(GuildInfoEx& _gi) {
	m_gi = _gi;
}

uint32_t CmdGuildInfo::getUID() {
	return m_uid;
}

void CmdGuildInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdGuildInfo::getOption() {
	return m_option;
}

void CmdGuildInfo::setOption(uint32_t _option) {
	m_option = _option;
}
