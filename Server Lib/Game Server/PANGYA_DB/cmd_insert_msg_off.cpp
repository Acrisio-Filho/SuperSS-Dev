// Arquivo cmd_insert_msg_off.cpp
// Criado em 11/08/2018 as 13:41 por Acrisio
// Implementa��o da classe CmdInsertMsgOff

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_insert_msg_off.hpp"

using namespace stdA;

CmdInsertMsgOff::CmdInsertMsgOff(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_to_uid(0u), m_msg("") {
}

CmdInsertMsgOff::CmdInsertMsgOff(uint32_t _uid, uint32_t _to_uid, std::string& _msg, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_to_uid(_to_uid), m_msg(_msg) {
}

CmdInsertMsgOff::~CmdInsertMsgOff() {
}

void CmdInsertMsgOff::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um INSERT
	return;
}

response* CmdInsertMsgOff::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdInsertMsgOff::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_to_uid == 0)
		throw exception("[CmdInsertMsgOff::prepareConsulta][Error] m_to_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_msg.empty())
		throw exception("[CmdInsertMsgOff::prepareConsulta][Error] m_msg is empty", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_msg.size() > 256)
		throw exception("[CmdInsertMsgOff::prepareConsulta][Error] m_msg size is great of limit supported", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_to_uid) + ", " + _db.makeText(m_msg));

	checkResponse(r, "nao conseguiu inserir Message Off[" + m_msg + "] do player[UID=" + std::to_string(m_uid) + "] para o player[UID=" + std::to_string(m_to_uid) + "]");

	return r;
}

uint32_t CmdInsertMsgOff::getUID() {
	return m_uid;
}

void CmdInsertMsgOff::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdInsertMsgOff::getToUID() {
	return m_to_uid;
}

void CmdInsertMsgOff::setToUID(uint32_t _to_uid) {
	m_to_uid = _to_uid;
}

std::string& CmdInsertMsgOff::getMessage() {
	return m_msg;
}

void CmdInsertMsgOff::setMessage(std::string& _msg) {
	m_msg = _msg;
}
