// Arquivo cmd_add_mascot.cpp
// Criado em 19/05/2018 as 16:33 por Acrisio
// Implementa��o da classe CmdAddMascot

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_mascot.hpp"

using namespace stdA;

CmdAddMascot::CmdAddMascot(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_purchase(0), m_gift_flag(0), m_time(0u), m_mi() {
}

CmdAddMascot::CmdAddMascot(uint32_t _uid, MascotInfoEx& _mi, uint32_t _time, unsigned char _purchase, unsigned char _gift_flag, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_purchase(_purchase), m_gift_flag(_gift_flag), m_time(_time), m_mi(_mi) {
}

CmdAddMascot::~CmdAddMascot() {
}

void CmdAddMascot::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_mi.id = IFNULL(atoi, _result->data[0]);
}

response* CmdAddMascot::prepareConsulta(database& _db) {

	if (m_mi._typeid == 0)
		throw exception("[CmdAddMascot::prepareConsulta][Error] Mascot info is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));
	
	// Ignora as flag gift e purchase para usar minha nova proc de add mascot
	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_mi._typeid)
			+ ", " + std::to_string(m_mi.tipo) + ", " + std::to_string((unsigned short)m_mi.is_cash)
			+ ", " + std::to_string(m_time) + ", " + _db.makeText(m_mi.message) + ", " + std::to_string(m_mi.price)
	);

	checkResponse(r, "nao conseguiu adicionar o Mascot[TYPEID=" + std::to_string(m_mi._typeid) + "] para o player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdAddMascot::getUID() {
	return m_uid;
}

void CmdAddMascot::setUID(uint32_t _uid) {
	m_uid = _uid;
}

MascotInfoEx& CmdAddMascot::getInfo() {
	return m_mi;
}

void CmdAddMascot::setInfo(MascotInfoEx& _mi) {
	m_mi = _mi;
}

uint32_t CmdAddMascot::getTime() {
	return m_time;
}

void CmdAddMascot::setTime(uint32_t _time) {
	m_time = _time;
}

unsigned char CmdAddMascot::getGiftFlag() {
	return m_gift_flag;
}

void CmdAddMascot::setGiftFlag(unsigned char _gift_flag) {
	m_gift_flag = _gift_flag;
}

unsigned char CmdAddMascot::getPurchase() {
	return m_purchase;
}

void CmdAddMascot::setPurchase(unsigned char _purchase) {
	m_purchase = _purchase;
}
