// Arquivo cmd_tuto_event_clear.cpp
// Criado em 28/06/2018 as 22:03 por Acrisio
// Implementa��o da classe CmdTutoEventClear

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_tuto_event_clear.hpp"

using namespace stdA;

CmdTutoEventClear::CmdTutoEventClear(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_type(T_ROOKIE) {
}

CmdTutoEventClear::CmdTutoEventClear(uint32_t _uid, TYPE _type, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_type(_type) {
}

CmdTutoEventClear::~CmdTutoEventClear() {
}

void CmdTutoEventClear::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UDPATE
	return;
}

response* CmdTutoEventClear::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdTutoEventClear::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::string(m_type == T_ROOKIE ? m_szConsulta[1] : (m_type == T_BEGINNER ? m_szConsulta[2] : m_szConsulta[3]/*Advancer*/)) + m_szConsulta[4] + std::to_string(m_uid));

	checkResponse(r, "nao conseguiu atualizar Tutorial Evento[Type=" + std::to_string(m_type) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdTutoEventClear::getUID() {
	return m_uid;
}

void CmdTutoEventClear::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CmdTutoEventClear::TYPE CmdTutoEventClear::getType() {
	return m_type;
}

void CmdTutoEventClear::setType(TYPE _type) {
	m_type = _type;
}
