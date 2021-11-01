// Arquivo cmd_attendance_reward_info.cpp
// Criado em 24/03/2018 as 18:24 por Acrisio
// Implementa��o da classe CmdAttendanceRewardInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_attendance_reward_info.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdAttendanceRewardInfo::CmdAttendanceRewardInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_ari{0} {
}

CmdAttendanceRewardInfo::CmdAttendanceRewardInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_ari{0} {
}

CmdAttendanceRewardInfo::~CmdAttendanceRewardInfo() {
}

void CmdAttendanceRewardInfo::lineResult(result_set::ctx_res* _result, uint32_t _index_result) {

	checkColumnNumber(6, (uint32_t)_result->cols);

	m_ari.counter = IFNULL(atoi, _result->data[0]);
	m_ari.now._typeid = IFNULL(atoi, _result->data[1]);
	m_ari.now.qntd = IFNULL(atoi, _result->data[2]);
	m_ari.after._typeid = IFNULL(atoi, _result->data[3]);
	m_ari.after.qntd = IFNULL(atoi, _result->data[4]);
		
	if (_result->data[5] != nullptr)
		_translateDate(_result->data[5], &m_ari.last_login);
}

response* CmdAttendanceRewardInfo::prepareConsulta(database& _db) {

	m_ari.clear();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar attendance reward info do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdAttendanceRewardInfo::getUID() {
	return m_uid;
}

void CmdAttendanceRewardInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}

AttendanceRewardInfoEx& CmdAttendanceRewardInfo::getInfo() {
	return m_ari;
}