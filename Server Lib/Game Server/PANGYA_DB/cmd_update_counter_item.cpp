// Arquivo cmd_update_counter_item.cpp
// Criado em 15/04/2018 as 20:02 por Acrisio
// Implementa��o da classe CmdUpdateCounterItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_counter_item.hpp"

using namespace stdA;

CmdUpdateCounterItem::CmdUpdateCounterItem(bool _waiter) : pangya_db(_waiter), m_uid(0), m_cii{0} {
}

CmdUpdateCounterItem::CmdUpdateCounterItem(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_cii{0} {
}

CmdUpdateCounterItem::CmdUpdateCounterItem(uint32_t _uid, CounterItemInfo& _cii, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_cii(_cii) {
}

CmdUpdateCounterItem::~CmdUpdateCounterItem() {
}

void CmdUpdateCounterItem::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o aqui por que � um UPDATE
	return;
}

response* CmdUpdateCounterItem::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdUpdateCounterItem::prepareConsulta][Error] m_uid is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_cii.id <= 0 || m_cii._typeid == 0)
		throw exception("[CmdUpdateCounterItem::prepareConsulta][Error] CounterItemInfo m_cii is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_cii.value) + m_szConsulta[1] + std::to_string(m_uid) + m_szConsulta[2] + std::to_string(m_cii.id));

	checkResponse(r, "nao conseguiu atualizar o Counter Item[ID=" + std::to_string(m_cii.id) + "] do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdateCounterItem::getUID() {
	return m_uid;
}

void CmdUpdateCounterItem::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CounterItemInfo& CmdUpdateCounterItem::getInfo() {
	return m_cii;
}

void CmdUpdateCounterItem::setInfo(CounterItemInfo& _cii) {
	m_cii = _cii;
}
