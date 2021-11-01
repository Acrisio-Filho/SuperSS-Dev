// Arquivo cmd_delete_counter_item.cpp
// Criado em 14/04/2018 as 14:19 por Acrisio
// Implementa��o da classe CmdDeleteCounterItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_delete_counter_item.hpp"

#include "../../Projeto IOCP/UTIL/string_util.hpp"

using namespace stdA;

CmdDeleteCounterItem::CmdDeleteCounterItem(bool _waiter) : pangya_db(_waiter), m_uid(0), v_id() {
}

CmdDeleteCounterItem::CmdDeleteCounterItem(uint32_t _uid, int32_t _id, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), v_id(_id) {
}

CmdDeleteCounterItem::CmdDeleteCounterItem(uint32_t _uid, std::vector< int32_t >& _v_id, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), v_id(_v_id.begin(), _v_id.end()) {
}

CmdDeleteCounterItem::CmdDeleteCounterItem(uint32_t _uid, std::map< int32_t, CounterItemInfo >& _mp_id, bool _waiter) 
	: pangya_db(_waiter), m_uid(_uid), v_id() {

	for (auto& el : _mp_id)
		v_id.push_back(el.first);
}

CmdDeleteCounterItem::~CmdDeleteCounterItem() {
}

void CmdDeleteCounterItem::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um DELETE
	return;
}

response* CmdDeleteCounterItem::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdDeleteCounterItem::prepareConsulta][Error] m_uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (v_id.empty())
		throw exception("[CmdDeleteCounterItem::prepareConsulta][Error] v_counter_item_id is empty.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	std::string str_ids = stdA::to_string< std::vector< int32_t >, std::string >(v_id);

	auto r = _delete(_db, m_szConsulta[0] + std::to_string(m_uid) + m_szConsulta[1] + str_ids + m_szConsulta[2]);

	checkResponse(r, "nao conseguiu deletar Counter Item[ID={" + str_ids + "}] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdDeleteCounterItem::getUID() {
	return m_uid;
}

void CmdDeleteCounterItem::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdDeleteCounterItem::getId() {

	if (!v_id.empty())
		return *v_id.begin();

	return -1;
}

std::vector< int32_t >& CmdDeleteCounterItem::getIds() {
	return v_id;
}

void CmdDeleteCounterItem::setId(int32_t _id) {

	if (!v_id.empty())
		v_id.clear();

	v_id.push_back(_id);
}

void CmdDeleteCounterItem::setId(std::vector< int32_t >& _v_id) {

	v_id = _v_id;
}

void CmdDeleteCounterItem::setId(std::map< int32_t, CounterItemInfo >& _mp_id) {

	if (!v_id.empty())
		v_id.clear();

	for (auto& el : _mp_id)
		v_id.push_back(el.first);
}
