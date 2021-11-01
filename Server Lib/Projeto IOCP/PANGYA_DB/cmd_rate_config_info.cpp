// Aquivo cmd_rate_config_info.cpp
// Criado em 09/05/2019 as 19:00 por Acrisio
// Implementão da classe CmdRateConfigInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_rate_config_info.hpp"

using namespace stdA;

CmdRateConfigInfo::CmdRateConfigInfo(bool _waiter) : pangya_db(_waiter), m_rate_info{}, m_server_uid(0u), m_error(false) {
}

CmdRateConfigInfo::CmdRateConfigInfo(uint32_t _server_uid, bool _waiter) 
		: pangya_db(_waiter), m_server_uid(_server_uid), m_error(false) {
}

CmdRateConfigInfo::~CmdRateConfigInfo() {
}

void CmdRateConfigInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	// Verifica se recebeu o valores certo da consulta
	checkColumnNumber(16, (uint32_t)_result->cols);

	if (IFNULL(atoi, _result->data[0]) == -1)
		m_error = true;	// Error pode ser uma um server novo que tem que criar ou passou argumentos errados para a procedure
	else {

		m_rate_info.grand_zodiac_event_time = (unsigned short)IFNULL(atoi, _result->data[0]);
		m_rate_info.scratchy = (unsigned short)IFNULL(atoi, _result->data[1]);
		m_rate_info.papel_shop_rare_item = (unsigned short)IFNULL(atoi, _result->data[2]);
		m_rate_info.papel_shop_cookie_item = (unsigned short)IFNULL(atoi, _result->data[3]);
		m_rate_info.treasure = (unsigned short)IFNULL(atoi, _result->data[4]);
		m_rate_info.pang = (unsigned short)IFNULL(atoi, _result->data[5]);
		m_rate_info.exp = (unsigned short)IFNULL(atoi, _result->data[6]);
		m_rate_info.club_mastery = (unsigned short)IFNULL(atoi, _result->data[7]);
		m_rate_info.chuva = (unsigned short)IFNULL(atoi, _result->data[8]);
		m_rate_info.memorial_shop = (unsigned short)IFNULL(atoi, _result->data[9]);
		m_rate_info.angel_event = (unsigned short)IFNULL(atoi, _result->data[10]);
		m_rate_info.grand_prix_event = (unsigned short)IFNULL(atoi, _result->data[11]);
		m_rate_info.golden_time_event = (unsigned short)IFNULL(atoi, _result->data[12]);
		m_rate_info.login_reward_event = (unsigned short)IFNULL(atoi, _result->data[13]);
		m_rate_info.bot_gm_event = (unsigned short)IFNULL(atoi, _result->data[14]);
		m_rate_info.smart_calculator = (unsigned short)IFNULL(atoi, _result->data[15]);
	}
}

response* CmdRateConfigInfo::prepareConsulta(database& _db) {
	
	m_rate_info.clear();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_server_uid));

	checkResponse(r, "nao conseguiu pegar o Rate Config Info do Server[UID=" + std::to_string(m_server_uid) + "].");

	return r;
}

uint32_t CmdRateConfigInfo::getServerUID() {
	return m_server_uid;
}

void CmdRateConfigInfo::setServerUID(uint32_t _server_uid) {
	m_server_uid = _server_uid;
}

RateConfigInfo& CmdRateConfigInfo::getInfo() {
	return m_rate_info;
}

bool CmdRateConfigInfo::isError() {
	return m_error;
}
