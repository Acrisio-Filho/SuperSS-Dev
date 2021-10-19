// Arquivo cmd_item_left_from_email.cpp
// Criado em 24/03/2018 as 17:29 por Acrisio
// Implementa��o da classe CmdItemLeftFromEmail

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_item_left_from_email.hpp"

using namespace stdA;

CmdItemLeftFromEmail::CmdItemLeftFromEmail(bool _waiter) : pangya_db(_waiter), m_email_id(0) {
}

CmdItemLeftFromEmail::CmdItemLeftFromEmail(int32_t _email_id, bool _waiter)
			: pangya_db(_waiter), m_email_id(_email_id) {
}

CmdItemLeftFromEmail::~CmdItemLeftFromEmail() {
}

void CmdItemLeftFromEmail::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {
	return;
}

response* CmdItemLeftFromEmail::prepareConsulta(database& _db) {

	auto r = _update(_db, m_szConsulta + std::to_string(m_email_id));

	checkResponse(r, "nao conseguiu deletar o(s) item(ns) do email[ID=" + std::to_string(m_email_id) + "]");

	return r;
}

uint32_t CmdItemLeftFromEmail::getEmailID() {
	return m_email_id;
}

void CmdItemLeftFromEmail::setEmailID(uint32_t _email_id) {
	m_email_id = _email_id;
}
