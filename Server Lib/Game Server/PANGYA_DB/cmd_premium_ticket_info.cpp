// Arquivo cmd_premium_ticket_info.cpp
// Criado em 24/03/2018 as 19:24 por Acrisio
// Implementa��o da classe CmdPremiumTicketInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_premium_ticket_info.hpp"

using namespace stdA;

CmdPremiumTicketInfo::CmdPremiumTicketInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_pt{0} {
}

CmdPremiumTicketInfo::CmdPremiumTicketInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_pt{0} {
}

CmdPremiumTicketInfo::~CmdPremiumTicketInfo() {
}

void CmdPremiumTicketInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(4, (uint32_t)_result->cols);

	m_pt.id = IFNULL(atoi, _result->data[0]);
	m_pt._typeid = IFNULL(atoi, _result->data[1]);
	m_pt.unix_end_date = IFNULL(atoi, _result->data[2]);
	m_pt.unix_sec_date = IFNULL(atoi, _result->data[3]);
}

response* CmdPremiumTicketInfo::prepareConsulta(database& _db) {

	m_pt.clear();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar premium ticket info do player: " + std::to_string(m_uid));

	return r;
}

PremiumTicket& CmdPremiumTicketInfo::getInfo() {
	return m_pt;
}

void CmdPremiumTicketInfo::setInfo(PremiumTicket& _pt) {
	m_pt = _pt;
}

uint32_t CmdPremiumTicketInfo::getUID() {
	return m_uid;
}

void CmdPremiumTicketInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}
