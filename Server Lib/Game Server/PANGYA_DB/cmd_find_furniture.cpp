// Arquivo cmd_find_furniture.cpp
// Criado em 26/05/2018 as 14:06 por Acrisio
// Implementa��o da classe CmdFindFurniture

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_find_furniture.hpp"
#include "../../Projeto IOCP/UTIL/iff.h"

using namespace stdA;

CmdFindFurniture::CmdFindFurniture(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_typeid(0u), m_mri{0} {
}

CmdFindFurniture::CmdFindFurniture(uint32_t _uid, uint32_t _typeid, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_typeid(_typeid), m_mri{0} {
}

CmdFindFurniture::~CmdFindFurniture() {
}

void CmdFindFurniture::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(9, (uint32_t)_result->cols);

	m_mri.id = IFNULL(atoi, _result->data[0]);

	if (m_mri.id > 0) { // found
		//uid_req = IFNULL(atoi, _result->data[1]);	ignora o uid retornado
		m_mri._typeid = IFNULL(atoi, _result->data[2]);
		m_mri.number = (unsigned short)IFNULL(atoi, _result->data[3]);
		m_mri.location.x = (float)IFNULL(atof, _result->data[4]);
		m_mri.location.y = (float)IFNULL(atof, _result->data[5]);
		m_mri.location.z = (float)IFNULL(atof, _result->data[6]);
		m_mri.location.r = (float)IFNULL(atof, _result->data[7]);
		m_mri.equiped = (unsigned char)IFNULL(atoi, _result->data[8]);
	}
}

response* CmdFindFurniture::prepareConsulta(database& _db) {

	if (m_typeid == 0 || sIff::getInstance().getItemGroupIdentify(m_typeid) != iff::FURNITURE)
		throw exception("[CmdFindFurniture::prepareConsulta][Error] _typeid furniture is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_mri.clear();
	m_mri.id = -1;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_typeid));

	checkResponse(r, "nao conseguiu encontrar o furniture[TYPEID=" + std::to_string(m_typeid) + "] do player[TYPEID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdFindFurniture::getUID() {
	return m_uid;
}

void CmdFindFurniture::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdFindFurniture::getTypeid() {
	return m_typeid;
}

void CmdFindFurniture::setTypeid(uint32_t _typeid) {
	m_typeid = _typeid;
}

bool CmdFindFurniture::hasFound() {
	return m_mri.id > 0;
}

MyRoomItem& CmdFindFurniture::getInfo() {
	return m_mri;
}
