// Arquivo cmd_update_grand_prix_clear.cpp
// Criado em 14/06/2019 as 10:41 por Acrisio
// Implementa��o da classe CmdUpdateGrandPrixClear

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_grand_prix_clear.hpp"

using namespace stdA;

CmdUpdateGrandPrixClear::CmdUpdateGrandPrixClear(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_gpc{0} {
}

CmdUpdateGrandPrixClear::CmdUpdateGrandPrixClear(uint32_t _uid, GrandPrixClear& _gpc, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_gpc(_gpc) {
}

CmdUpdateGrandPrixClear::~CmdUpdateGrandPrixClear() {
}

void CmdUpdateGrandPrixClear::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateGrandPrixClear::prepareConsulta(database& _db) {
	
	if (m_uid == 0u)
		throw exception("[CmdUpdateGrandPrixClear::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_gpc._typeid == 0u)
		throw exception("[CmdUpdateGrandPrixClear::prepareConsulta][Error] Grand Prix Clear is invalid typeid is zero", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = consulta(_db, m_szConsulta[0] + std::to_string(m_gpc.position) + m_szConsulta[1] + std::to_string(m_uid) + m_szConsulta[2] + std::to_string(m_gpc._typeid));

	checkResponse(r, "nao conseguiu atualizar o Grand Prix Clear[TYPEID=" + std::to_string(m_gpc._typeid) + ", POSITION=" 
			+ std::to_string(m_gpc.position) + "] do Player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateGrandPrixClear::getUID() {
	return m_uid;
}

void CmdUpdateGrandPrixClear::setUID(uint32_t _uid) {
	m_uid = _uid;
}

GrandPrixClear& CmdUpdateGrandPrixClear::getInfo() {
	return m_gpc;
}

void CmdUpdateGrandPrixClear::setInfo(GrandPrixClear& _gpc) {
	m_gpc = _gpc;
}
