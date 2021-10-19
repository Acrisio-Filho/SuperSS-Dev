// Arquivo cmd_update_attendance_reward.cpp
// Criado em 14/11/2018 as 22:51 Acrisio
// Implementa��o da classe CmdUpdateAttendanceReward

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_attendance_reward.hpp"

using namespace stdA;

CmdUpdateAttendanceReward::CmdUpdateAttendanceReward(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_ari{0} {
}

CmdUpdateAttendanceReward::CmdUpdateAttendanceReward(uint32_t _uid, AttendanceRewardInfoEx& _ari, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_ari(_ari) {
}

CmdUpdateAttendanceReward::~CmdUpdateAttendanceReward() {
}

void CmdUpdateAttendanceReward::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateAttendanceReward::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdUpdateAttendanceReward::prepareConsulta][Error] m_uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	std::string last_login = "null";

	if (m_ari.last_login.wYear != 0 && m_ari.last_login.wMonth != 0 && m_ari.last_login.wDay != 0)
		last_login = _db.makeText(_formatDate(m_ari.last_login));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ari.counter) + ", " + std::to_string(m_ari.now._typeid)
			+ ", " + std::to_string(m_ari.now.qntd) + ", " + std::to_string(m_ari.after._typeid) + ", " + std::to_string(m_ari.after.qntd)
			+ ", " + last_login
	);

	checkResponse(r, "nao conseguiu Atualizar o Attendance Reward[COUNTER=" + std::to_string(m_ari.counter) + ", NOW_TYPEID=" + std::to_string(m_ari.now._typeid) + ", NOW_QNTD=" 
			+ std::to_string(m_ari.now.qntd) + ", AFTER_TYPEID=" + std::to_string(m_ari.after._typeid) + ", AFTER_QNTD=" + std::to_string(m_ari.after.qntd) + ", LAST_LOGIN=" 
			+ last_login + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateAttendanceReward::getUID() {
	return m_uid;
}

void CmdUpdateAttendanceReward::setUID(uint32_t _uid) {
	m_uid = _uid;
}

AttendanceRewardInfoEx& CmdUpdateAttendanceReward::getInfo() {
	return m_ari;
}

void CmdUpdateAttendanceReward::setInfo(AttendanceRewardInfoEx& _ari) {
	m_ari = _ari;
}
