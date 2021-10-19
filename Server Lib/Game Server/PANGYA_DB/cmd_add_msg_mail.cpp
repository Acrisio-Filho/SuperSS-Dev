// Arquivo cmd_add_msg_mail.cpp
// Criado em 26/05/2018 as 15:12 por Acrisio
// Implement���o da classe CmdAddMsgMail

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_msg_mail.hpp"

using namespace stdA;

CmdAddMsgMail::CmdAddMsgMail(bool _waiter) : pangya_db(_waiter), m_uid_from(0u), m_uid_to(0u), m_msg(), m_mail_id(-1) {
}

CmdAddMsgMail::CmdAddMsgMail(uint32_t _uid_from, uint32_t _uid_to, std::string& _msg, bool _waiter)
	: pangya_db(_waiter), m_uid_from(_uid_from), m_uid_to(_uid_to), m_msg(_msg), m_mail_id(-1) {
}

CmdAddMsgMail::~CmdAddMsgMail() {
}

void CmdAddMsgMail::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_mail_id = IFNULL(atoi, _result->data[0]);
}

response* CmdAddMsgMail::prepareConsulta(database& _db) {

	if (m_uid_to == 0 || m_msg.empty())
		throw exception("[CmdAddMsgMail::prepareConsulta][Error] uid[value=" + std::to_string(m_uid_to) + "] to send is invalid or msg is emtpy", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_mail_id = -1;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid_from) + ", " + std::to_string(m_uid_to) + ", " + _db.makeText(m_msg));

	checkResponse(r, "player[UID=" + std::to_string(m_uid_from) + "] nao conseguiu adicionar msg[value=" + m_msg + "] no mail do player[UID=" + std::to_string(m_uid_to) + "]");

	return r;
}

uint32_t CmdAddMsgMail::getUIDFrom() {
	return m_uid_from;
}

void CmdAddMsgMail::setUIDFrom(uint32_t _uid_from) {
	m_uid_from = _uid_from;
}

uint32_t CmdAddMsgMail::getUIDTo() {
	return m_uid_to;
}

void CmdAddMsgMail::setUIDTo(uint32_t _uid_to) {
	m_uid_to = _uid_to;
}

std::string& CmdAddMsgMail::getMsg() {
	return m_msg;
}

void CmdAddMsgMail::setMsg(std::string& _msg) {
	m_msg = _msg;
}

int32_t CmdAddMsgMail::getMailID() {
	return m_mail_id;
}
