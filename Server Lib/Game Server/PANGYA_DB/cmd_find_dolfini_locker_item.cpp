// Arquivo cmd_find_dolfini_locker_item.cpp
// Criado em 24/05/2019 as 00:20 por Acrisio
// Implementação da classe CmdFindDolfiniLockerItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_find_dolfini_locker_item.hpp"

using namespace stdA;

CmdFindDolfiniLockerItem::CmdFindDolfiniLockerItem(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_typeid(0u), m_dli{0} {
}

CmdFindDolfiniLockerItem::CmdFindDolfiniLockerItem(uint32_t _uid, uint32_t _typeid, bool _waiter) 
	: pangya_db(_waiter), m_uid(_uid), m_typeid(_typeid), m_dli{0} {
}

CmdFindDolfiniLockerItem::~CmdFindDolfiniLockerItem() {
}

void CmdFindDolfiniLockerItem::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(10, (uint32_t)_result->cols);

	m_dli.item.id = IFNULL(atoi, _result->data[0]);

	if (m_dli.item.id <= -1) {
		m_dli.index = ~0ull; // not found
	}else {

		long uid_req = 0l;

		uid_req = IFNULL(atoi, _result->data[1]);
		m_dli.item._typeid = IFNULL(atoi, _result->data[2]);
		if (is_valid_c_string(_result->data[3]))
			STRCPY_TO_MEMORY_FIXED_SIZE(m_dli.item.sd_name, sizeof(m_dli.item.sd_name), _result->data[3]);
		//strcpy_s(dli.item.sd_name, _result->data[3]);
		if (is_valid_c_string(_result->data[4]))
			STRCPY_TO_MEMORY_FIXED_SIZE(m_dli.item.sd_idx, sizeof(m_dli.item.sd_idx), _result->data[4]);
		//strcpy_s(dli.item.sd_idx, _result->data[4]);
		m_dli.item.sd_seq = (short)IFNULL(atoi, _result->data[5]);
		if (is_valid_c_string(_result->data[6]))
			STRCPY_TO_MEMORY_FIXED_SIZE(m_dli.item.sd_copier_nick, sizeof(m_dli.item.sd_copier_nick), _result->data[6]);
		//strcpy_s(dli.item.sd_copier_nick, _result->data[6]);
		m_dli.item.sd_status = (unsigned char)IFNULL(atoi, _result->data[7]);
		m_dli.index = IFNULL(atoll, _result->data[8]);
		m_dli.item.qntd = IFNULL(atoi, _result->data[9]);	// DOLFINI_LOCKER_FLAG, mas é quantidade

		if (uid_req != m_uid)
			throw exception("[CmdFindDolfiniLockerItem::lineResult][Error] O dolfini info requerido retornou um uid diferente. UID_req: "
					+ std::to_string(m_uid) + " != " + std::to_string(uid_req), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
	}
}

response* CmdFindDolfiniLockerItem::prepareConsulta(database& _db) {
	
	if (m_uid == 0u || m_typeid == 0u)
		throw exception("[CmdFindDolfiniLockerItem::prepareConsulta][Error] m_uid(" + std::to_string(m_uid) + ") ou o m_typeid(" 
				+ std::to_string(m_typeid) + ") is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_dli.clear();
	m_dli.index = ~0ull;
	m_dli.item.id = -1;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_typeid));

	checkResponse(r, "nao conseguiu encontrar o DolfiniLockerItem[TYPEID=" + std::to_string(m_typeid) + "] do Player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdFindDolfiniLockerItem::getUID() {
	return m_uid;
}

void CmdFindDolfiniLockerItem::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdFindDolfiniLockerItem::getTypeid() {
	return m_typeid;
}

void CmdFindDolfiniLockerItem::setTypeid(uint32_t _typeid) {
	m_typeid = _typeid;
}

DolfiniLockerItem& CmdFindDolfiniLockerItem::getInfo() {
	return m_dli;
}

bool CmdFindDolfiniLockerItem::hasFound() {
	return m_dli.index != ~0ull && m_dli.item.id > 0;
}
