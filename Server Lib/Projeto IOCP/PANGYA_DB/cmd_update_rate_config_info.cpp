// Arquivo cmd_update_rate_config_info.cpp
// Criado em 09/05/2019 as 19:26 por Acrisio
// Implementação da classe CmdUpdateRateConfigInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_rate_config_info.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"

using namespace stdA;

CmdUpdateRateConfigInfo::CmdUpdateRateConfigInfo(bool _waiter) : pangya_db(_waiter), m_rci{}, m_server_uid(0u) {
}

CmdUpdateRateConfigInfo::CmdUpdateRateConfigInfo(uint32_t _server_uid, RateConfigInfo& _rci, bool _waiter) 
		: pangya_db(_waiter), m_rci(_rci), m_server_uid(_server_uid) {
}

CmdUpdateRateConfigInfo::~CmdUpdateRateConfigInfo() {
}

void CmdUpdateRateConfigInfo::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// Não usa por que é um UPDATE
	return;
}

response* CmdUpdateRateConfigInfo::prepareConsulta(database& _db) {

	if (m_server_uid == 0u)
		throw exception("[CmdUpdateRateConfigInfo][Error] server_uid[VALUE=" + std::to_string(m_server_uid) + "] is invalid.", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));
	
	auto r = procedure(_db, m_szConsulta, std::to_string(m_server_uid) + ", " + std::to_string(m_rci.grand_zodiac_event_time)
				+ ", " + std::to_string(m_rci.scratchy) + ", " + std::to_string(m_rci.papel_shop_rare_item)
				+ ", " + std::to_string(m_rci.papel_shop_cookie_item) + ", " + std::to_string(m_rci.treasure)
				+ ", " + std::to_string(m_rci.pang) + ", " + std::to_string(m_rci.exp) + ", " + std::to_string(m_rci.club_mastery)
				+ ", " + std::to_string(m_rci.chuva) + ", " + std::to_string(m_rci.memorial_shop) 
				+ ", " + std::to_string(m_rci.angel_event) + ", " + std::to_string(m_rci.grand_prix_event) 
				+ ", " + std::to_string(m_rci.golden_time_event) + ", " + std::to_string(m_rci.login_reward_event)
				+ ", " + std::to_string(m_rci.bot_gm_event) + ", " + std::to_string(m_rci.smart_calculator)
	);

	checkResponse(r, "nao conseguiu atualizar o Rate Config Info[SERVER_UID=" + std::to_string(m_server_uid) + ", " + m_rci.toString() + "]");

	return r;
}

RateConfigInfo& CmdUpdateRateConfigInfo::getInfo() {
	return m_rci;
}

void CmdUpdateRateConfigInfo::setInfo(RateConfigInfo& _rci) {
	m_rci = _rci;
}

uint32_t CmdUpdateRateConfigInfo::getServerUID() {
	return m_server_uid;
}

void CmdUpdateRateConfigInfo::setServerUID(uint32_t _server_uid) {
	m_server_uid = _server_uid;
}
