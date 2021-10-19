// Arquivo cmd_delete_furniture.cpp
// Criado em 16/06/2018 as 13:27 por Acrisio
// Implementa��o da classe CmdDeleteFurniture

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_delete_furniture.hpp"

using namespace stdA;

CmdDeleteFurniture::CmdDeleteFurniture(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(0l) {
}

CmdDeleteFurniture::CmdDeleteFurniture(uint32_t _uid, int32_t _id, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id) {
}

CmdDeleteFurniture::~CmdDeleteFurniture() {
}

void CmdDeleteFurniture::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdDeleteFurniture::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdDeleteFurniture::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_id <= 0)
		throw exception("[CmdDeleteFurniture::prepareConsulta][Error] m_id[value=" + std::to_string(m_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsula[0] + std::to_string(m_uid) + m_szConsula[1] + std::to_string(m_id));

	checkResponse(r, "nao conseguiu deletat Furniture[ID=" + std::to_string(m_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdDeleteFurniture::getUID() {
	return m_uid;
}

void CmdDeleteFurniture::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdDeleteFurniture::getID() {
	return m_id;
}

void CmdDeleteFurniture::setID(int32_t _id) {
	m_id = _id;
}
