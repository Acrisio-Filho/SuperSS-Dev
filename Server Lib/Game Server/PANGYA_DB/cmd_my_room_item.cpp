// Arquivo cmd_my_room_item.cpp
// Criado em 22/03/2018 as 20:44 por Acrisio
// Implementa��o da classe CmdMyRoomItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_my_room_item.hpp"

using namespace stdA;

CmdMyRoomItem::CmdMyRoomItem(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_item_id(-1), m_type(ALL), v_mri() {
}

CmdMyRoomItem::CmdMyRoomItem(uint32_t _uid, TYPE _type, int32_t _item_id, bool _waiter) 
	: pangya_db(_waiter), m_uid(_uid), m_type(_type), m_item_id(_item_id), v_mri() {
}

CmdMyRoomItem::~CmdMyRoomItem() {
}

void CmdMyRoomItem::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(9, (uint32_t)_result->cols);

	MyRoomItem mri{ 0 };
	uint32_t uid_req = 0u;

	mri.id = IFNULL(atoi, _result->data[0]);
	uid_req = IFNULL(atoi, _result->data[1]);
	mri._typeid = IFNULL(atoi, _result->data[2]);
	mri.number = (unsigned short)IFNULL(atoi, _result->data[3]);
	mri.location.x = (float)IFNULL(atof, _result->data[4]);
	mri.location.y = (float)IFNULL(atof, _result->data[5]);
	mri.location.z = (float)IFNULL(atof, _result->data[6]);
	mri.location.r = (float)IFNULL(atof, _result->data[7]);
	mri.equiped = (unsigned char)IFNULL(atoi, _result->data[8]);

	v_mri.push_back(mri);

	if (uid_req != m_uid)
		throw exception("[CmdMyRoomItem::lineResult][Error] o uid do my room item requisitado do player e diferente. UID_req: " + std::to_string(uid_req) + " != " + std::to_string(m_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
}

response* CmdMyRoomItem::prepareConsulta(database& _db) {

	v_mri.clear();
	v_mri.shrink_to_fit();

	auto r = procedure(_db, (m_type == ALL) ? m_szConsulta[0] : m_szConsulta[1], std::to_string(m_uid) + (m_type == ONE ? ", " + std::to_string(m_item_id) : std::string()));

	checkResponse(r, "nao conseguiu pegar o(s) item(ns) do my room do player: " + std::to_string(m_uid));

	return r;
}

std::vector< MyRoomItem >& CmdMyRoomItem::getMyRoomItem() {
	return v_mri;
}

uint32_t CmdMyRoomItem::getUID() {
	return m_uid;
}

void CmdMyRoomItem::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdMyRoomItem::getItemID() {
	return m_item_id;
}

void CmdMyRoomItem::setItemID(int32_t _item_id) {
	m_item_id = _item_id;
}

CmdMyRoomItem::TYPE CmdMyRoomItem::getType() {
	return m_type;
}

void CmdMyRoomItem::setType(TYPE _type) {
	m_type = _type;
}
