// Arquivo cmd_update_card_special_time.cpp
// Criado em 07/07/2018 as 22:35 por Acrisio
// Implementa��o da classe CmdUpdateCardSpecialTime

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_card_special_time.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdUpdateCardSpecialTime::CmdUpdateCardSpecialTime(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_cei{0} {
}

CmdUpdateCardSpecialTime::CmdUpdateCardSpecialTime(uint32_t _uid, CardEquipInfoEx& _cei, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_cei(_cei) {
}

CmdUpdateCardSpecialTime::~CmdUpdateCardSpecialTime() {
}

void CmdUpdateCardSpecialTime::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateCardSpecialTime::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdUpdateCardSpecialTime::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_cei.index <= 0 || m_cei._typeid == 0)
		throw exception("[CmdUpdateCardSpecialTime::prepareConsulta][Error] m_cei[index=" + std::to_string(m_cei.index) + ", TYPEID=" + std::to_string(m_cei._typeid) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_cei.index) + ", " + std::to_string(m_cei._typeid)
			+ ", " + std::to_string(m_cei.efeito) + ", " + std::to_string(m_cei.efeito_qntd) + ", " 
			+ std::to_string(m_cei.tipo) + ", " + _db.makeText(_formatDate(m_cei.end_date))
	);

	checkResponse(r, "nao conseguiu atualizar tempo do Card Special[index=" + std::to_string(m_cei.index) + ", TYPEID=" + std::to_string(m_cei._typeid) + ", EFEITO{TYPE: " 
			+ std::to_string(m_cei.efeito) + ", QNTD: " + std::to_string(m_cei.efeito_qntd) + "}, TIPO=" + std::to_string(m_cei.tipo) + ", DATE{REG_DT: " 
			+ _formatDate(m_cei.use_date) + ", END_DT: " + _formatDate(m_cei.end_date) + "}] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateCardSpecialTime::getUID() {
	return m_uid;
}

void CmdUpdateCardSpecialTime::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CardEquipInfoEx& CmdUpdateCardSpecialTime::getInfo() {
	return m_cei;
}

void CmdUpdateCardSpecialTime::setInfo(CardEquipInfoEx& _cei) {
	m_cei = _cei;
}
