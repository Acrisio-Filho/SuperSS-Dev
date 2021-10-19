// Arquivo cmd_update_trofel_especial_qntd.cpp
// Criado em 21/06/2019 as 18:59 por Acrisio
// Implementa��o da classe CmdUpdateTrofelEspecialQntd

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_trofel_especial_qntd.hpp"

using namespace stdA;

CmdUpdateTrofelEspecialQntd::CmdUpdateTrofelEspecialQntd(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(-1), m_qntd(0u), m_type(eTYPE::ESPECIAL) {
}

CmdUpdateTrofelEspecialQntd::CmdUpdateTrofelEspecialQntd(uint32_t _uid, int32_t _id, uint32_t _qntd, eTYPE _type, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id), m_qntd(_qntd), m_type(_type) {
}

CmdUpdateTrofelEspecialQntd::~CmdUpdateTrofelEspecialQntd() {
}

void CmdUpdateTrofelEspecialQntd::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateTrofelEspecialQntd::prepareConsulta(database& _db) {
	
	if (m_uid == 0u)
		throw exception("[CmdUpdateTrofelEspecialQntd::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_id <= 0)
		throw exception("[CmdUpdateTrofelEspecialQntd::prepareConsulta][Error] m_id[VALUE=" + std::to_string(m_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_type > eTYPE::GRAND_PRIX)
		throw exception("[CmdUpdateTrofelEspecialQntd::prepareConsulta][Error] m_type[VALUE=" + std::to_string((unsigned short)m_type) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = consulta(_db, m_szConsulta[m_type][0] + std::to_string(m_qntd) + m_szConsulta[m_type][1] + std::to_string(m_uid) + m_szConsulta[m_type][2] + std::to_string(m_id));

	checkResponse(r, "nao conseguiu Atualizar quantidade do Trofel Especial(" + std::string(m_type == eTYPE::GRAND_PRIX ? "Grand Prix" : "") 
			+ ")[ID=" + std::to_string(m_id) + ", QNTD=" + std::to_string(m_qntd) + "] do Player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateTrofelEspecialQntd::getUID() {
	return m_uid;
}

void CmdUpdateTrofelEspecialQntd::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdUpdateTrofelEspecialQntd::getId() {
	return m_id;
}

void CmdUpdateTrofelEspecialQntd::setId(int32_t _id) {
	m_id = _id;
}

uint32_t CmdUpdateTrofelEspecialQntd::getQntd() {
	return m_qntd;
}

void CmdUpdateTrofelEspecialQntd::setQntd(uint32_t _qntd) {
	m_qntd = _qntd;
}

CmdUpdateTrofelEspecialQntd::eTYPE CmdUpdateTrofelEspecialQntd::getType() {
	return m_type;
}

void CmdUpdateTrofelEspecialQntd::setType(eTYPE _type) {
	m_type = _type;
}
