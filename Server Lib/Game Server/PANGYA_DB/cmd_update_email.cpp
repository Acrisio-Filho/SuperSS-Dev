// Arquivo cmd_update_email.cpp
// Criado em 14/01/2021 as 09:05 por Acrisio
// Implemeneta��o da classe CmdUpdateEmail

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_email.hpp"

using namespace stdA;

CmdUpdateEmail::CmdUpdateEmail(uint32_t _uid, EmailInfoEx& _ei, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_ei(_ei) {
}

CmdUpdateEmail::CmdUpdateEmail(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_ei{ 0u } {
}

CmdUpdateEmail::~CmdUpdateEmail() {
}

void CmdUpdateEmail::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateEmail::prepareConsulta(database& _db) {
	
	if (m_uid == 0u)
		throw exception("[CmdUpdateEmail::prepareConsulta][Error] m_uid is invalid(0)", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_ei.id <= 0)
		throw exception("[CmdUpdateEmail::prepareConsulta][Error] Email[ID=" + std::to_string(m_ei.id) 
				+ "] is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = consulta(_db, 
			  m_szConsulta[0] + std::to_string((unsigned short)m_ei.lida_yn)
			+ m_szConsulta[1] + std::to_string(m_ei.visit_count) 
			+ m_szConsulta[2] + std::to_string(m_uid) 
			+ m_szConsulta[3] + std::to_string(m_ei.id));

	checkResponse(r, "nao conseguiu atualizar o Email[ID=" + std::to_string(m_ei.id) + ", LIDA_YN=" 
			+ std::to_string((unsigned short)m_ei.lida_yn) + ", VISIT_COUNT=" + std::to_string(m_ei.visit_count) + "] do Player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateEmail::getUID() {
	return m_uid;
}

void CmdUpdateEmail::setUID(uint32_t _uid) {
	m_uid = _uid;
}

EmailInfoEx& CmdUpdateEmail::getEmail() {
	return m_ei;
}

void CmdUpdateEmail::setEmail(EmailInfoEx& _ei) {
	m_ei = _ei;
}
