// Arquivo cmd_pay_caddie_holy_day.cpp
// Criado em 15/07/2018 as 18:32 por Acrisio
// Implementa��o da classe CmdPayCaddieHolyDay

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_pay_caddie_holy_day.hpp"

using namespace stdA;

CmdPayCaddieHolyDay::CmdPayCaddieHolyDay(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(-1), m_end_dt("") {
}

CmdPayCaddieHolyDay::CmdPayCaddieHolyDay(uint32_t _uid, int32_t _id, std::string& _end_dt, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id), m_end_dt(_end_dt) {
}

CmdPayCaddieHolyDay::~CmdPayCaddieHolyDay() {
}

void CmdPayCaddieHolyDay::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdPayCaddieHolyDay::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdPayCaddieHolyDay::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_id <= 0)
		throw exception("[CmdPayCaddieHolyDay::prepareConsulta][Error] m_id[value=" + std::to_string(m_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_end_dt.empty())
		throw exception("[CmdPayCaddieHolyDay::prepareConsulta][Error] m_end_dt_unix is invalid(empty)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_id) + ", " + _db.makeText(m_end_dt));

	checkResponse(r, "nao conseguiu atualizar a end date[exntend days of caddie][date=" + m_end_dt + "] do caddie[ID=" + std::to_string(m_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;

}

uint32_t CmdPayCaddieHolyDay::getUID() {
	return m_uid;
}

void CmdPayCaddieHolyDay::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdPayCaddieHolyDay::getId() {
	return m_id;
}

void CmdPayCaddieHolyDay::setId(int32_t _id) {
	m_id = _id;
}

std::string& CmdPayCaddieHolyDay::getEndDate() {
	return m_end_dt;
}

void CmdPayCaddieHolyDay::setEndDate(std::string& _end_dt) {
	m_end_dt = _end_dt;
}
