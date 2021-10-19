// Arquivo cmd_update_premium_ticket_time.cpp
// Criado em 07/12/2019 as 13:07 por Acrisio
// Implementa��o da classe CmdUpdatePremiumTicketTime

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_premium_ticket_time.hpp"

using namespace stdA;

CmdUpdatePremiumTicketTime::CmdUpdatePremiumTicketTime(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_wi{0} {
}

CmdUpdatePremiumTicketTime::CmdUpdatePremiumTicketTime(uint32_t _uid, WarehouseItemEx& _wi, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_wi(_wi) {
}

CmdUpdatePremiumTicketTime::~CmdUpdatePremiumTicketTime() {
}

void CmdUpdatePremiumTicketTime::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdatePremiumTicketTime::prepareConsulta(database& _db) {
	
	if (m_uid == 0u)
		throw exception("[CmdUpdatePremiumTicketTime::prepareConsulta][Error] m_uid is invalid(zero).", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_wi.id <= 0)
		throw exception("[CmdUpdatePremiumTicketTime::prepareConsulta][Error] m_wi.id is invalid[VALUE=" + std::to_string(m_wi.id) + "].", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_wi.id) 
			+ ", " + std::to_string(m_wi.STDA_C_ITEM_TIME) + ", " + std::to_string(m_wi.c[0]) 
			+ ", " + std::to_string(m_wi.c[1]) + ", " + std::to_string(m_wi.c[2]) 
			+ ", " + std::to_string(m_wi.c[3]) + ", " + std::to_string(m_wi.c[4]));

	checkResponse(r, "nao conseguiu atualizar Premium Ticket Time[ID=" + std::to_string(m_wi.id)
			+ ", TEMPO=" + std::to_string(m_wi.STDA_C_ITEM_TIME) + ", C0=" + std::to_string(m_wi.c[0])
			+ ", C1=" + std::to_string(m_wi.c[1]) + ", C2=" + std::to_string(m_wi.c[2])
			+ ", C3=" + std::to_string(m_wi.c[3]) + ", C4=" + std::to_string(m_wi.c[4]) 
			+ "] do Player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdatePremiumTicketTime::getUID() {
	return m_uid;
}

void CmdUpdatePremiumTicketTime::setUID(uint32_t _uid) {
	m_uid = _uid;
}

WarehouseItemEx& CmdUpdatePremiumTicketTime::getPremiumTicket() {
	return m_wi;
}

void CmdUpdatePremiumTicketTime::setPremiumTicket(WarehouseItemEx& _wi) {
	m_wi = _wi;
}
