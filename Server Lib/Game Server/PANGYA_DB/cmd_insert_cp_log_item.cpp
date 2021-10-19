// Arquivo cmd_insert_cp_log_item.cpp
// Criado em 24/05/2019 as 04:30 por Acrisio
// Implementação da classe CmdInsertCPLogItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_insert_cp_log_item.hpp"

using namespace stdA;

CmdInsertCPLogItem::CmdInsertCPLogItem(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_log_id(-1ll), m_item_id(-1ll), m_item{0} {
}

CmdInsertCPLogItem::CmdInsertCPLogItem(uint32_t _uid, int64_t _log_id, CPLog::stItem& _item, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_log_id(_log_id), m_item(_item), m_item_id(-1ll) {
}

CmdInsertCPLogItem::~CmdInsertCPLogItem() {
}

void CmdInsertCPLogItem::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_item_id = IFNULL(atoll, _result->data[0]);
}

response* CmdInsertCPLogItem::prepareConsulta(database& _db) {
	
	if (m_log_id <= 0ll)
		throw exception("[CmdInsertCPLogItem::prepareConsulta][Error] m_log_id[VALUE=" + std::to_string(m_log_id) 
				+ "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_item_id = -1ll;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_log_id) + ", " + std::to_string(m_item._typeid)
			+ ", " + std::to_string(m_item.qntd) + ", " + std::to_string(m_item.price)
	);

	checkResponse(r, "nao conseguiu inserir CPLogItem[LOD_ID=" + std::to_string(m_log_id) + ", ITEM_TYPEID=" + std::to_string(m_item._typeid) 
			+ ", ITEM_QNTD=" + std::to_string(m_item.qntd) + ", ITEM_PRICE=" + std::to_string(m_item.price) + "] do Player[UID="  + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdInsertCPLogItem::getUID() {
	return m_uid;
}

void CmdInsertCPLogItem::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int64_t CmdInsertCPLogItem::getLogId() {
	return m_log_id;
}

void CmdInsertCPLogItem::setLogId(int64_t _log_id) {
	m_log_id = _log_id;
}

CPLog::stItem& CmdInsertCPLogItem::getItem() {
	return m_item;
}

void CmdInsertCPLogItem::setItem(CPLog::stItem& _item) {
	m_item = _item;
}

int64_t CmdInsertCPLogItem::getItemId() {
	return m_item_id;
}
