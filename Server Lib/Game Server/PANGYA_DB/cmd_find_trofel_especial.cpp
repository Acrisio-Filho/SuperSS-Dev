// Arquivo cmd_find_trofel_especial.cpp
// Criado em 21/06/2019 as 19:43 por Acrisio
// Implementa��o da classe CmdFindTrofelEspecial

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_find_trofel_especial.hpp"

using namespace stdA;

CmdFindTrofelEspecial::CmdFindTrofelEspecial(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_typeid(0u), m_type(eTYPE::ESPECIAL), m_tsi{0} {
}

CmdFindTrofelEspecial::CmdFindTrofelEspecial(uint32_t _uid, uint32_t _typeid, eTYPE _type, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_typeid(_typeid), m_type(_type), m_tsi{0} {
}

CmdFindTrofelEspecial::~CmdFindTrofelEspecial() {
}

void CmdFindTrofelEspecial::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(3, (uint32_t)_result->cols);

	m_tsi.id = IFNULL(atoi, _result->data[0]);

	if (m_tsi.id > 0) { // found
		m_tsi._typeid = (uint32_t)IFNULL(atoi, _result->data[1]);
		m_tsi.qntd = (uint32_t)IFNULL(atoi, _result->data[2]);
	}
		
}

response* CmdFindTrofelEspecial::prepareConsulta(database& _db) {
	
	if (m_uid == 0u)
		throw exception("[CmdFindTrofelEspecial::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_typeid == 0u || sIff::getInstance().getItemGroupIdentify(m_typeid) != iff::MATCH)
		throw exception("[CmdFindTrofelEspecial::prepareConsulta][Error] TrofelEspecialInfo[TYPEID=" 
				+ std::to_string(m_typeid) + "] m_typeid is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_type > eTYPE::GRAND_PRIX)
		throw exception("[CmdFindTrofelEspecial::prepareConsulta][Error] m_type[VALUE=" + std::to_string(m_type) + "] is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_tsi.clear();
	m_tsi.id = -1;

	auto r = procedure(_db, m_szConsulta[m_type], std::to_string(m_uid) + ", " + std::to_string(m_typeid));

	checkResponse(r, "nao conseguiu encontrar o TrofelEspecial(" + std::string(m_type == eTYPE::GRAND_PRIX ? "Grand Prix" : "") 
			+ ")[TYPEID=" + std::to_string(m_typeid) + "] do Player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdFindTrofelEspecial::getUID() {
	return m_uid;
}

void CmdFindTrofelEspecial::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdFindTrofelEspecial::getTypeid() {
	return m_typeid;
}

void CmdFindTrofelEspecial::setTypeid(uint32_t _typeid) {
	m_typeid = _typeid;
}

CmdFindTrofelEspecial::eTYPE CmdFindTrofelEspecial::getType() {
	return m_type;
}

void CmdFindTrofelEspecial::setType(eTYPE _type) {
	m_type = _type;
}

bool CmdFindTrofelEspecial::hasFound() {
	return m_tsi.id > 0;
}

TrofelEspecialInfo& CmdFindTrofelEspecial::getInfo() {
	return m_tsi;
}
