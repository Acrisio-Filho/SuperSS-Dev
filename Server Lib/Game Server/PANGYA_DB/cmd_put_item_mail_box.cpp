// Arquivo cmd_put_item_mail_box.cpp
// Criado em 26/05/2018 as 15:39 por Acrisio
// Implementa��o da classe CmdPutItemMailBox

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_put_item_mail_box.hpp"

using namespace stdA;

CmdPutItemMailBox::CmdPutItemMailBox(bool _waiter) : pangya_db(_waiter), m_uid_from(0u), m_uid_to(0u), m_mail_id(-1), m_item{0} {
}

CmdPutItemMailBox::CmdPutItemMailBox(uint32_t _uid_from, uint32_t _uid_to, int32_t _mail_id, stItem& _item, bool _waiter)
	: pangya_db(_waiter), m_uid_from(_uid_from), m_uid_to(_uid_to), m_mail_id(_mail_id),  m_item(_item) {
}

CmdPutItemMailBox::CmdPutItemMailBox(uint32_t _uid_from, uint32_t _uid_to, int32_t _mail_id, EmailInfo::item& _item, bool _waiter)
	: pangya_db(_waiter), m_uid_from(_uid_from), m_uid_to(_uid_to), m_mail_id(_mail_id), m_item{0} {

	m_item.id = _item.id;
	m_item._typeid = _item._typeid;
	m_item.flag_time = _item.flag_time;
	m_item.STDA_C_ITEM_QNTD = (unsigned short)(m_item.qntd = (uint32_t)_item.qntd);
	m_item.STDA_C_ITEM_TIME = (unsigned short)_item.tempo_qntd;
}

CmdPutItemMailBox::~CmdPutItemMailBox() {
}

void CmdPutItemMailBox::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um INSERT
	return;
}

response* CmdPutItemMailBox::prepareConsulta(database& _db) {

	if (m_mail_id <= 0 || m_uid_to == 0)
		throw exception("[CmdPutItemMailBox::prepareConsulta][Error] mail_id[value=" + std::to_string(m_mail_id) + "] is invalid or uid to send is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_item._typeid == 0)
		throw exception("[CmdPutItemMailBox::prepareConsulta][Error] item is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid_from) + ", " + std::to_string(m_uid_to)
					+ ", " + std::to_string(m_mail_id) + ", " + std::to_string(m_item.id)
					+ ", " + std::to_string(m_item._typeid) + ", "  + std::to_string((unsigned short)m_item.flag_time)
					+ ", " + std::to_string((m_item.qntd > 0xFFu) ? m_item.qntd : m_item.STDA_C_ITEM_QNTD) + ", " + std::to_string(m_item.STDA_C_ITEM_TIME)
	);
	
	checkResponse(r, "player[UID=" + std::to_string(m_uid_from) + "] nao conseguiu adicionar item[TYPEID=" 
			+ std::to_string(m_item._typeid) + ", ID=" + std::to_string(m_item.id) + "] no mail[ID=" + std::to_string(m_mail_id) 
			+ "] do player[UID=" + std::to_string(m_uid_to) + "]");

	return r;
}

uint32_t CmdPutItemMailBox::getUIDFrom() {
	return m_uid_from;
}

void CmdPutItemMailBox::setUIDFrom(uint32_t _uid_from) {
	m_uid_from = _uid_from;
}

uint32_t CmdPutItemMailBox::getUIDTo() {
	return m_uid_to;
}

void CmdPutItemMailBox::setUIDTo(uint32_t _uid_to) {
	m_uid_to = _uid_to;
}

int32_t CmdPutItemMailBox::getMailID() {
	return m_mail_id;
}

void CmdPutItemMailBox::setLong(int32_t _mail_id) {
	m_mail_id = _mail_id;
}

stItem& CmdPutItemMailBox::getItem() {
	return m_item;
}

void CmdPutItemMailBox::setItem(stItem& _item) {
	m_item = _item;
}
