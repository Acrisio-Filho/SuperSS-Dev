// Arquivo cmd_card_info.cpp
// Criado em 21/03/2018 as 22:15 por Acrisio
// Implementa��o da classe CmdCardInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_card_info.hpp"

using namespace stdA;

CmdCardInfo::CmdCardInfo(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_type(ALL), m_card_id(-1), v_ci() {
}

CmdCardInfo::CmdCardInfo(uint32_t _uid, TYPE _type, int32_t _card_id, bool _waiter) 
	: pangya_db(_waiter), m_uid(_uid), m_type(_type), m_card_id(_card_id), v_ci() {
}

CmdCardInfo::~CmdCardInfo() {
}

void CmdCardInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(11, (uint32_t)_result->cols);

	CardInfo ci{ 0 };

	ci.id = IFNULL(atoi, _result->data[0]);
	ci._typeid = IFNULL(atoi, _result->data[2]);
	ci.slot = IFNULL(atoi, _result->data[3]);
	ci.efeito = IFNULL(atoi, _result->data[4]);
	ci.efeito_qntd = IFNULL(atoi, _result->data[5]);
	ci.qntd = IFNULL(atoi, _result->data[6]);
	if (_result->data[7] != nullptr)
		_translateDate(_result->data[7], &ci.use_date);
	if (_result->data[8] != nullptr)
		_translateDate(_result->data[8], &ci.end_date);
	ci.type = (unsigned char)IFNULL(atoi, _result->data[9]);
	ci.use_yn = (unsigned char)IFNULL(atoi, _result->data[10]);

	v_ci.push_back(ci);
}

response* CmdCardInfo::prepareConsulta(database& _db) {
	
	v_ci.clear();
	v_ci.shrink_to_fit();

	auto r = procedure(_db, (m_type == ALL) ? m_szConsulta[0] : m_szConsulta[1], std::to_string(m_uid) + (m_type == ONE ? ", " + std::to_string(m_card_id) : std::string()));

	checkResponse(r, "nao conseguiu pegar card info do player: " + std::to_string(m_uid));

	return r;
}

std::vector< CardInfo >& CmdCardInfo::getInfo() {
	return v_ci;
}

uint32_t CmdCardInfo::getUID() {
	return m_uid;
}

void CmdCardInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CmdCardInfo::TYPE CmdCardInfo::getType() {
	return m_type;
}

void CmdCardInfo::setType(TYPE _type) {
	m_type = _type;
}

int32_t CmdCardInfo::getCardID() {
	return m_card_id;
}

void CmdCardInfo::setCardID(int32_t _card_id) {
	m_card_id = _card_id;
}
