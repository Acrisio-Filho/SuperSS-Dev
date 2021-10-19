// Arquivo cmd_delete_caddie.cpp
// Criado em 16/06/2018 as 13:04 por Acrisio
// Implementa��o da classe CmdDeleteCaddie

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_delete_caddie.hpp"

using namespace stdA;

CmdDeleteCaddie::CmdDeleteCaddie(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(0l) {
}

CmdDeleteCaddie::CmdDeleteCaddie(uint32_t _uid, int32_t _id, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id) {
}

CmdDeleteCaddie::~CmdDeleteCaddie() {
}

void CmdDeleteCaddie::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdDeleteCaddie::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdDeleteCaddie::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_id <= 0)
		throw exception("[CmdDeleteCaddie::prepareConsulta][Error] m_id[value=" + std::to_string(m_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_uid) + m_szConsulta[1] + std::to_string(m_id));

	checkResponse(r, "nao conseguiu deletar caddie[ID=" + std::to_string(m_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdDeleteCaddie::getUID() {
	return m_uid;
}

void CmdDeleteCaddie::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdDeleteCaddie::getID() {
	return m_id;
}

void CmdDeleteCaddie::setID(int32_t _id) {
	m_id = _id;
}
