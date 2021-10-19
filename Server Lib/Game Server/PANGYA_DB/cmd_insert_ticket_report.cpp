// Arquivo cmd_insert_ticket_report.cpp
// Criado em 22/09/2018 as 16:52 por Acrisio
// Implementa��o da classe CmdInsertTicketReport

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_insert_ticket_report.hpp"

using namespace stdA;

CmdInsertTicketReport::CmdInsertTicketReport(bool _waiter) : pangya_db(_waiter), m_id(-1), m_trofel(0u), m_type(0u) {
}

CmdInsertTicketReport::CmdInsertTicketReport(uint32_t _trofel, unsigned char _type, bool _waiter)
	: pangya_db(_waiter), m_id(-1), m_trofel(_trofel), m_type(_type) {
}

CmdInsertTicketReport::~CmdInsertTicketReport() {
}

void CmdInsertTicketReport::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_id = IFNULL(atoi, _result->data[0]);

	if (m_id == -1)
		throw exception("[CmdInsertTicketReport::lineResult][Error] nao conseguiu inserir um Ticket Report[TROFEL=" 
				+ std::to_string(m_trofel) + ", TYPE=" + std::to_string((unsigned short)m_type) + "] no banco de dados, ele retornou um id == -1", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
}

response* CmdInsertTicketReport::prepareConsulta(database& _db) {

	m_id = -1;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_trofel) + ", " + std::to_string((unsigned short)m_type));

	checkResponse(r, "nao conseguiu inserir um Ticket Report[TROFEL=" + std::to_string(m_trofel) + ", TYPE=" + std::to_string((unsigned short)m_type) + "] no banco de dados");

	return r;
}

uint32_t CmdInsertTicketReport::getTrofel() {
	return m_trofel;
}

void CmdInsertTicketReport::setTrofel(uint32_t _trofel) {
	m_trofel = _trofel;
}

unsigned char CmdInsertTicketReport::getType() {
	return m_type;
}

void CmdInsertTicketReport::setType(unsigned char _type) {
	m_type = _type;
}

int32_t CmdInsertTicketReport::getId() {
	return m_id;
}
