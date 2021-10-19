// Arquivo cmd_find_caddie.cpp
// Criado em 22/05/2018 as 21:12 por Acrisio
// Implementa��o da classe CmdFindCaddie

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_find_caddie.hpp"
#include "../../Projeto IOCP/UTIL/iff.h"

using namespace stdA;

CmdFindCaddie::CmdFindCaddie(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_typeid(0u), m_ci{0} {
}

CmdFindCaddie::CmdFindCaddie(uint32_t _uid, uint32_t _typeid, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_typeid(_typeid), m_ci{0} {
}

CmdFindCaddie::~CmdFindCaddie() {
}

void CmdFindCaddie::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(11, (uint32_t)_result->cols);

	m_ci.id = IFNULL(atoi, _result->data[0]);

	if (m_ci.id > 0) { // found
		
		// Begin of Initialization Caddie Info Structure
		m_ci._typeid = IFNULL(atoi, _result->data[2]);
		m_ci.parts_typeid = IFNULL(atoi, _result->data[3]);
		m_ci.level = (unsigned char)IFNULL(atoi, _result->data[4]);
		m_ci.exp = IFNULL(atoi, _result->data[5]);
		m_ci.rent_flag = (unsigned char)IFNULL(atoi, _result->data[6]);

		//m_ci.end_date_unix = (short)IFNULL(atoi, _result->data[7]);
		if (_result->data[7] != nullptr)
			_translateDate(_result->data[7], &m_ci.end_date);

		m_ci.purchase = (unsigned char)IFNULL(atoi, _result->data[8]);

		//m_ci.parts_end_date_unix = (short)IFNULL(atoi, _result->data[9]);
		if (_result->data[9] != nullptr)
			_translateDate(_result->data[9], &m_ci.end_parts_date);

		m_ci.check_end = (short)IFNULL(atoi, _result->data[10]);
		// End of Initialization Caddie Info Structure

	}
}

response* CmdFindCaddie::prepareConsulta(database& _db) {

	if (m_typeid == 0 || sIff::getInstance().getItemGroupIdentify(m_typeid) != iff::CADDIE)
		throw exception("[CmdFindCaddie::prepareConsulta][Error] _typeid caddie is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_ci.clear();
	m_ci.id = -1;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_typeid));

	checkResponse(r, "nao conseguiu encontrar o caddie[TYPEID=" + std::to_string(m_typeid) + "] para do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdFindCaddie::getUID() {
	return m_uid;
}

void CmdFindCaddie::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdFindCaddie::getTypeid() {
	return m_typeid;
}

void CmdFindCaddie::setTypeid(uint32_t _typeid) {
	m_typeid = _typeid;
}

bool CmdFindCaddie::hasFound() {
	return m_ci.id > 0;
}

CaddieInfoEx& CmdFindCaddie::getInfo() {
	return m_ci;
}
