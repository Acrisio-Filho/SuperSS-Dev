// Arquivo cmd_update_tutorial.cpp
// Criado em 28/06/2018 as 22:34 por Acrisio
// Implementa��o da classe CmdUpdateTutorial


#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_tutorial.hpp"

using namespace stdA;

CmdUpdateTutorial::CmdUpdateTutorial(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_ti{0} {
}

CmdUpdateTutorial::CmdUpdateTutorial(uint32_t _uid, TutorialInfo& _ti, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_ti(_ti) {
}

CmdUpdateTutorial::~CmdUpdateTutorial() {
}

void CmdUpdateTutorial::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateTutorial::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdUpdateTutorial::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_ti.rookie) + m_szConsulta[1] + std::to_string(m_ti.beginner) 
						+ m_szConsulta[2] + std::to_string(m_ti.advancer) + m_szConsulta[3] + std::to_string(m_uid));

	checkResponse(r, "nao conseguiu Atualizar o Tutorial[Rookie=" + std::to_string(m_ti.rookie) + ", Beginner=" 
			+ std::to_string(m_ti.beginner) + ", Advancer=" + std::to_string(m_ti.advancer) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateTutorial::getUID() {
	return m_uid;
}

void CmdUpdateTutorial::setUID(uint32_t _uid) {
	m_uid = _uid;
}

TutorialInfo& CmdUpdateTutorial::getInfo() {
	return m_ti;
}

void CmdUpdateTutorial::setInfo(TutorialInfo& _ti) {
	m_ti = _ti;
}
