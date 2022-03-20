// Arquivo cmd_find_mascot.cpp
// Criado em 22/05/2018 as 22:06 por Acrisio
// Implementa��o da classe CmdFindMascot

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_find_mascot.hpp"
#include "../../Projeto IOCP/UTIL/iff.h"

using namespace stdA;

CmdFindMascot::CmdFindMascot(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_typeid(0u), m_mi() {
}

CmdFindMascot::CmdFindMascot(uint32_t _uid, uint32_t _typeid, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_typeid(_typeid), m_mi() {
}

CmdFindMascot::~CmdFindMascot() {
}

void CmdFindMascot::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(10, (uint32_t)_result->cols);

	m_mi.id = IFNULL(atoi, _result->data[0]);

	if (m_mi.id > 0) { // found
		m_mi._typeid = IFNULL(atoi, _result->data[2]);
		m_mi.level = (unsigned char)IFNULL(atoi, _result->data[3]);
		m_mi.exp = IFNULL(atoi, _result->data[4]);
		m_mi.flag = (unsigned char)IFNULL(atoi, _result->data[5]);
		if (is_valid_c_string(_result->data[6]))
			STRCPY_TO_MEMORY_FIXED_SIZE(m_mi.message, sizeof(m_mi.message), _result->data[6]);
		m_mi.tipo = (short)IFNULL(atoi, _result->data[7]);
		m_mi.is_cash = (unsigned char)IFNULL(atoi, _result->data[8]);
		_translateDate(_result->data[9], &m_mi.data);
	}
}

response* CmdFindMascot::prepareConsulta(database& _db) {

	if (m_typeid == 0 || sIff::getInstance().getItemGroupIdentify(m_typeid) != iff::MASCOT)
		throw exception("[CmdFindMascot::prepareConsulta][Error] _typeid mascot is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_mi.clear();
	m_mi.id = -1;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_typeid));

	checkResponse(r, "nao conseguiu encontrar o mascot[TYPEID=" + std::to_string(m_typeid) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdFindMascot::getUID() {
	return m_uid;
}

void CmdFindMascot::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdFindMascot::getTypeid() {
	return m_typeid;
}

void CmdFindMascot::setTypeid(uint32_t _typeid) {
	m_typeid = _typeid;
}

bool CmdFindMascot::hasFound() {
	return m_mi.id > 0;
}

MascotInfoEx& CmdFindMascot::getInfo() {
	return m_mi;
}
