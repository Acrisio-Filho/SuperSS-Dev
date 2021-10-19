// Arquivo cmd_add_trofel_espcial.cpp
// Criado em 21/06/2019 as 18:28 por Acrisio
// Implementa��o da classe CmdAddTrofelEspecial

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_trofel_especial.hpp"

using namespace stdA;

CmdAddTrofelEspecial::CmdAddTrofelEspecial(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_tsi{0}, m_type(eTYPE::ESPECIAL) {
}

CmdAddTrofelEspecial::CmdAddTrofelEspecial(uint32_t _uid, TrofelEspecialInfo& _tsi, eTYPE _type, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_tsi(_tsi), m_type(_type) {
}

CmdAddTrofelEspecial::~CmdAddTrofelEspecial() {
}

void CmdAddTrofelEspecial::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_tsi.id = IFNULL(atoi, _result->data[0]);
}

response* CmdAddTrofelEspecial::prepareConsulta(database& _db) {
	
	if (m_uid == 0u)
		throw exception("[CmdAddTrofelEspecial::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_tsi._typeid == 0u)
		throw exception("[CmdAddTrofelEspecial::prepareConsulta][Error] TrofelEspecialInfo _typeid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_type > eTYPE::GRAND_PRIX)
		throw exception("[CmdAddTrofelEspecial::prepareConsulta][Error] TYPE[VALUE=" + std::to_string((unsigned short)m_type) 
				+ "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta[m_type], std::to_string(m_uid) 
						+ ", " + std::to_string(m_tsi._typeid) 
						+ ", " + std::to_string(m_tsi.qntd));

	checkResponse(r, "nao conseguiu Adicionar Trofel Especial(" + std::string(m_type == eTYPE::GRAND_PRIX ? " Grand Prix" : "") 
			+ ")[TYPEID=" + std::to_string(m_tsi._typeid) + ", QNTD=" + std::to_string(m_tsi.qntd) + "] para o Player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdAddTrofelEspecial::getUID() {
	return m_uid;
}

void CmdAddTrofelEspecial::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CmdAddTrofelEspecial::eTYPE CmdAddTrofelEspecial::getType() {
	return m_type;
}

void CmdAddTrofelEspecial::setType(eTYPE _type) {
	m_type = _type;
}

TrofelEspecialInfo& CmdAddTrofelEspecial::getInfo() {
	return m_tsi;
}

void CmdAddTrofelEspecial::setInfo(TrofelEspecialInfo& _tsi) {
	m_tsi = _tsi;
}
