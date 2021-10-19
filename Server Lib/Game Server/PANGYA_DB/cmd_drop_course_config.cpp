// Arquivo cmd_drop_course_config.cpp
// Criado em 07/09/2018 as 15:14 por Acrisio
// Implementa��o da classe CmdDropCourseConfig

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "cmd_drop_course_config.hpp"

using namespace stdA;

CmdDropCourseConfig::CmdDropCourseConfig(bool _waiter) : pangya_db(_waiter), m_config{0} {
}

CmdDropCourseConfig::~CmdDropCourseConfig() {
}

void CmdDropCourseConfig::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(3, (uint32_t)_result->cols);

	m_config.rate_mana_artefact = (uint32_t)IFNULL(atoi, _result->data[0]);
	m_config.rate_grand_prix_ticket = (uint32_t)IFNULL(atoi, _result->data[1]);
	m_config.rate_SSC_ticket = (uint32_t)IFNULL(atoi, _result->data[2]);
}

response* CmdDropCourseConfig::prepareConsulta(database& _db) {

	m_config.clear();

	auto r = procedure(_db, m_szConsulta, "");

	checkResponse(r, "nao consiguiu pegar o Drop Course Config");

	return r;
}

DropSystem::stConfig& CmdDropCourseConfig::getConfig() {
	return m_config;
}
