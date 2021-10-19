// Arquivo cmd_add_counter_item.cpp
// Criado em 14/04/2018 as 15:30 por Acrisio
// Implementa��o da classe CmdAddCounterItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_counter_item.hpp"

using namespace stdA;

CmdAddCounterItem::CmdAddCounterItem(bool _waiter) : pangya_db(_waiter), m_uid(0), m_typeid(0), m_value(0), m_id(-1) {
}

CmdAddCounterItem::CmdAddCounterItem(uint32_t _uid, uint32_t _typeid, uint32_t _value, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_typeid(_typeid), m_value(_value), m_id(-1) {
}

CmdAddCounterItem::~CmdAddCounterItem() {
}

void CmdAddCounterItem::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_id = IFNULL(atoi, _result->data[0]);
}

response* CmdAddCounterItem::prepareConsulta(database& _db) {

	m_id = -1;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_typeid) + ", 1, " + std::to_string(m_value));
	
	checkResponse(r, "nao conseguiu adicionar o counter item[Typeid=" + std::to_string(m_typeid) + "] para o player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdAddCounterItem::getUID() {
	return m_uid;
}

void CmdAddCounterItem::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdAddCounterItem::getTypeid() {
	return m_typeid;
}

void CmdAddCounterItem::setTypeid(uint32_t _typeid) {
	m_typeid = _typeid;
}

uint32_t CmdAddCounterItem::getValue() {
	return m_value;
}

void CmdAddCounterItem::setValue(uint32_t _value) {
	m_value = _value;
}

int32_t CmdAddCounterItem::getId() {
	return m_id;
}
