// Arquivo cmd_set_notice_caddie_holy_day.cpp
// Criado em 15/07/2018 as 19:30 por Acrisio
// Implementa��o  da classe CmdSetNoticeCaddieHolyDay

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_set_notice_caddie_holy_day.hpp"

using namespace stdA;

CmdSetNoticeCaddieHolyDay::CmdSetNoticeCaddieHolyDay(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(-1), m_check(0u) {
}

CmdSetNoticeCaddieHolyDay::CmdSetNoticeCaddieHolyDay(uint32_t _uid, int32_t _id, unsigned short _check, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id), m_check(_check) {
}

CmdSetNoticeCaddieHolyDay::~CmdSetNoticeCaddieHolyDay() {
}

void CmdSetNoticeCaddieHolyDay::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdSetNoticeCaddieHolyDay::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdSetNoticeCaddieHolyDay::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_id <= 0)
		throw exception("[CmdSetNoticeCaddieHolyDay::prepareConsulta][Error] m_id[value=" + std::to_string(m_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_check) + m_szConsulta[1] + std::to_string(m_uid) + m_szConsulta[2] + std::to_string(m_id));

	checkResponse(r, "nao conseguiu atualizar o Aviso[check=" + (m_check ? std::string("ON") : std::string("OFF") ) + "] de ferias do Caddie[ID=" + std::to_string(m_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdSetNoticeCaddieHolyDay::getUID() {
	return m_uid;
}

void CmdSetNoticeCaddieHolyDay::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdSetNoticeCaddieHolyDay::getId() {
	return m_id;
}

void CmdSetNoticeCaddieHolyDay::setId(int32_t _id) {
	m_id = _id;
}

unsigned short CmdSetNoticeCaddieHolyDay::getCheck() {
	return m_check;
}

void CmdSetNoticeCaddieHolyDay::setCheck(unsigned short _check) {
	m_check = _check;
}
