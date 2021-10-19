// Arquivo cmd_add_dolfini_locker_item.cpp
// Criado em 02/06/2018 as 23:28 por Acrisio
// Implementa��o da classe CmdAddDolfiniLockerItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_dolfini_locker_item.hpp"
#include "../../Projeto IOCP/UTIL/iff.h"

using namespace stdA;

CmdAddDolfiniLockerItem::CmdAddDolfiniLockerItem(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_dli{0} {
}

CmdAddDolfiniLockerItem::CmdAddDolfiniLockerItem(uint32_t _uid, DolfiniLockerItem& _dli, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_dli(_dli) {
}

CmdAddDolfiniLockerItem::~CmdAddDolfiniLockerItem() {
}

void CmdAddDolfiniLockerItem::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_dli.index = IFNULL(atoll, _result->data[0]);
}

response* CmdAddDolfiniLockerItem::prepareConsulta(database& _db) {

	if (m_dli.item.id <= 0 || m_dli.item._typeid == 0 || sIff::getInstance().getItemGroupIdentify(m_dli.item._typeid) != iff::PART)
		throw exception("[CmdAddDolfiniLockerItem][Error] player[UID=" + std::to_string(m_uid) + "] -> Item[TYPEID=" 
				+ std::to_string(m_dli.item._typeid) + ", ID=" + std::to_string(m_dli.item.id) + "] invalid for put in Dolfini Locker", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_dli.index = ~0ull;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_dli.item.id));

	checkResponse(r, "nao conseguiu colocar o item[TYPEID=" + std::to_string(m_dli.item._typeid) + ", ID=" + std::to_string(m_dli.item.id) + "] no Dolfini Locker do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdAddDolfiniLockerItem::getUID() {
	return m_uid;
}

void CmdAddDolfiniLockerItem::setUID(uint32_t _uid) {
	m_uid = _uid;
}

DolfiniLockerItem& CmdAddDolfiniLockerItem::getInfo() {
	return m_dli;
}

void CmdAddDolfiniLockerItem::setInfo(DolfiniLockerItem& _dli) {
	m_dli = _dli;
}
