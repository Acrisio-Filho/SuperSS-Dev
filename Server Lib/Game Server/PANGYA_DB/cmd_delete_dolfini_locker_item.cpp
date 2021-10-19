// Arquivo cmd_delete_dolfini_locker_item.cpp
// Criado em 03/06/2018 as 00:49 por Acrisio
// Implementa��o da classe CmdDeleteDolfiniLockerItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_delete_dolfini_locker_item.hpp"

using namespace stdA;

CmdDeleteDolfiniLockerItem::CmdDeleteDolfiniLockerItem(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_index(0ull) {
}

CmdDeleteDolfiniLockerItem::CmdDeleteDolfiniLockerItem(uint32_t _uid, uint64_t _index, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_index(_index) {
}

CmdDeleteDolfiniLockerItem::~CmdDeleteDolfiniLockerItem() {
}

void CmdDeleteDolfiniLockerItem::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdDeleteDolfiniLockerItem::prepareConsulta(database& _db) {

	if (m_index <= 0ull)
		throw exception("[CmdDeleteDolfiniLockerItem][Error] Dolfini Locker Item[index=" + std::to_string(m_index) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_index));

	checkResponse(r, "nao conseguiu deletar Dolfini Locker item[index=" + std::to_string(m_index) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdDeleteDolfiniLockerItem::getUID() {
	return m_uid;
}

void CmdDeleteDolfiniLockerItem::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint64_t CmdDeleteDolfiniLockerItem::getIndex() {
	return m_index;
}

void CmdDeleteDolfiniLockerItem::setIndex(uint64_t _index) {
	m_index = _index;
}
