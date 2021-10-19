// Arquivo cmd_find_card.cpp
// Criado em 22/05/2018 as 23:14 por Acrisio
// Implementa��o da classe CmdFindCard

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_find_card.hpp"
#include "../../Projeto IOCP/UTIL/iff.h"

using namespace stdA;

CmdFindCard::CmdFindCard(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_typeid(0u), m_ci{0} {
}

CmdFindCard::CmdFindCard(uint32_t _uid, uint32_t _typeid, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_typeid(_typeid), m_ci{0} {
}

CmdFindCard::~CmdFindCard() {
}

void CmdFindCard::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(11, (uint32_t)_result->cols);

	m_ci.id = IFNULL(atoi, _result->data[0]);

	if (m_ci.id > 0) { // found
		m_ci._typeid = IFNULL(atoi, _result->data[2]);
		m_ci.slot = IFNULL(atoi, _result->data[3]);
		m_ci.efeito = IFNULL(atoi, _result->data[4]);
		m_ci.efeito_qntd = IFNULL(atoi, _result->data[5]);
		m_ci.qntd = IFNULL(atoi, _result->data[6]);
		if (_result->data[7] != nullptr)
			_translateDate(_result->data[7], &m_ci.use_date);
		if (_result->data[8] != nullptr)
			_translateDate(_result->data[8], &m_ci.end_date);
		m_ci.type = (unsigned char)IFNULL(atoi, _result->data[9]);
		m_ci.use_yn = (unsigned char)IFNULL(atoi, _result->data[10]);
	}
}

response* CmdFindCard::prepareConsulta(database& _db) {

	if (m_typeid == 0 || sIff::getInstance().getItemGroupIdentify(m_typeid) != iff::CARD)
		throw exception("[CmdFindCard::prepareConsulta][Error] _typeid card is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_ci.clear();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_typeid));

	checkResponse(r, "nao conseguiu encontrar card[TYPEID=" + std::to_string(m_typeid) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdFindCard::getUID() {
	return m_uid;
}

void CmdFindCard::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdFindCard::getTypeid() {
	return m_typeid;
}

void CmdFindCard::setTypeid(uint32_t _typeid) {
	m_typeid = _typeid;
}

bool CmdFindCard::hasFound() {
	return m_ci.id > 0;
}

CardInfo& CmdFindCard::getInfo() {
	return m_ci;
}
