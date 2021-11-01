// Arquivo cmd_update_mascot_time.cpp
// Criado em 29/05/2018 as 21:45 por Acrisio
// Implementa��o da classe CmdUpdateMascotTime

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_mascot_time.hpp"

using namespace stdA;

CmdUpdateMascotTime::CmdUpdateMascotTime(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(-1), m_time() {
}

CmdUpdateMascotTime::CmdUpdateMascotTime(uint32_t _uid, int32_t _id, std::string& _time, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id), m_time(_time) {
}

CmdUpdateMascotTime::~CmdUpdateMascotTime() {
}

void CmdUpdateMascotTime::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {
	
	// N�o usa por que � um UPDATE
}

response* CmdUpdateMascotTime::prepareConsulta(database& _db) {

	if (m_id <= 0)
		throw exception("[CmdUpdateMascotTime::prepareConsulta][Error] mascot id[value=" + std::to_string(m_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_time.empty())
		throw exception("[CmdUpdateMascotTime::prepareConsulta][Error] time is empty", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_id) + ", " + _db.makeText(m_time));

	checkResponse(r, "nao conseguiu atualizar o tempo do mascot[ID=" + std::to_string(m_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateMascotTime::getUID() {
	return m_uid;
}

void CmdUpdateMascotTime::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdUpdateMascotTime::getID() {
	return m_id;
}

void CmdUpdateMascotTime::setID(int32_t _id) {
	m_id = _id;
}

std::string& CmdUpdateMascotTime::getTime() {
	return m_time;
}

void CmdUpdateMascotTime::setTime(std::string& _time) {
	m_time = _time;
}
