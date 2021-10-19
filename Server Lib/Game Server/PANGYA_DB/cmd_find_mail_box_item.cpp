// Arquivo cmd_find_mail_box_item.cpp
// Criado em 29/10/2019 as 21:19 por Acrisio
// Implemneta��o da classe CmdFindMailBoxItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_find_mail_box_item.hpp"
#include <algorithm>

using namespace stdA;

CmdFindMailBoxItem::CmdFindMailBoxItem(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_typeid(0u), m_has_found(false) {
}

CmdFindMailBoxItem::CmdFindMailBoxItem(uint32_t _uid, uint32_t _typeid, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_typeid(_typeid), m_has_found(false) {
}

CmdFindMailBoxItem::~CmdFindMailBoxItem() {
}

void CmdFindMailBoxItem::lineResult(result_set::ctx_res *_result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	int32_t _typeid = IFNULL(atoi, _result->data[0]);

	if (_typeid != -1 && (uint32_t)_typeid != m_typeid)
		throw exception("[CmdFindMailBoxItem::lineResult][Error] typeid que retornou eh diferento do requisitado. [REQUEST=" + std::to_string(m_uid) 
				+ ", RETURN=" + std::to_string(_typeid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
	else
		m_has_found = _typeid > 0;
}

response* CmdFindMailBoxItem::prepareConsulta(database& _db) {
	
	if (m_uid == 0u)
		throw exception("[CmdFindMailBoxItem::prepareConsulta][Error] m_uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_typeid == 0u)
		throw exception("[CmdFindMailBoxItem::prepareConsulta][Error] m_typeid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 0, 4));

	m_has_found = false;

	auto r = procedure(_db, m_consulta, std::to_string(m_uid) + ", " + std::to_string(m_typeid));

	checkResponse(r, "nao conseguiu encontrar o item[TYPEID=" + std::to_string(m_typeid) + "] no Mail Box do Player[UID=" + std::to_string(m_uid) + "].");

	return r;
}

uint32_t CmdFindMailBoxItem::getUID() {
	return m_uid;
}

void CmdFindMailBoxItem::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdFindMailBoxItem::getTypeid() {
	return m_typeid;
}

void CmdFindMailBoxItem::setTypeid(uint32_t _typeid) {
	m_typeid = _typeid;
}

bool CmdFindMailBoxItem::hasFound() {
	return m_has_found;
}
