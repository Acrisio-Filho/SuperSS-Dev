// Arquivo cmd_add_furniture.cpp
// Criado em 12/06/2018 as 20:50 por Acrisio
// Implementa��o da classe CmdAddFurniture

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_furniture.hpp"

using namespace stdA;

CmdAddFurniture::CmdAddFurniture(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_mri{0} {
}

CmdAddFurniture::CmdAddFurniture(uint32_t _uid, MyRoomItem& _mri, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_mri(_mri) {
}

CmdAddFurniture::~CmdAddFurniture() {
}

void CmdAddFurniture::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_mri.id = IFNULL(atoi, _result->data[0]);
}

response* CmdAddFurniture::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdAddFurniture::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_mri._typeid == 0)
		throw exception("[CmdAddFurniture::prepareConsulta][Error] m_mri._typeid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_mri.id = -1;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_mri._typeid) + ", " + std::to_string(m_mri.location.x)
			+ ", " + std::to_string(m_mri.location.y) + ", " + std::to_string(m_mri.location.z) + ", " + std::to_string(m_mri.location.r)
	);

	checkResponse(r, "nao conseguiu adicionar o Furniture[TYPEID=" + std::to_string(m_mri._typeid) + "] para o player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdAddFurniture::getUID() {
	return m_uid;
}

void CmdAddFurniture::setUID(uint32_t _uid) {
	m_uid = _uid;
}

MyRoomItem& CmdAddFurniture::getInfo() {
	return m_mri;
}

void CmdAddFurniture::setInfo(MyRoomItem& _mri) {
	m_mri = _mri;
}
