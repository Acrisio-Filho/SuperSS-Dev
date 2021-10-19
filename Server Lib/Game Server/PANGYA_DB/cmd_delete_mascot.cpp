// Arquivo cmd_delete_mascot.cpp
// Criado em 16/06/2018 as 13:44 por Acrisio
// Implementa��o da classe CmdDeleteMascot

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_delete_mascot.hpp"

using namespace stdA;

CmdDeleteMascot::CmdDeleteMascot(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(0l) {
}

CmdDeleteMascot::CmdDeleteMascot(uint32_t _uid, int32_t _id, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id) {
}

CmdDeleteMascot::~CmdDeleteMascot() {
}

void CmdDeleteMascot::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdDeleteMascot::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdDeleteMascot::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_id <= 0)
		throw exception("[CmdDeleteMascot::prepareConsulta][Error] m_id[value=" + std::to_string(m_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulat[0] + std::to_string(m_uid) + m_szConsulat[1] + std::to_string(m_id));

	checkResponse(r, "nao conseguiu deletar Mascot[ID=" + std::to_string(m_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdDeleteMascot::getUID() {
	return m_uid;
}

void CmdDeleteMascot::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdDeleteMascot::getID() {
	return m_id;
}

void CmdDeleteMascot::setID(int32_t _id) {
	m_id = _id;
}
