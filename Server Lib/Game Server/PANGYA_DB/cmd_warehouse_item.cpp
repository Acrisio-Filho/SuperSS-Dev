// Arquivo cmd_warehouse_item.cpp
// Criado em 18/03/2018 as 22:06 por Acrisio
// Implementa��o da classe CmdWarehouseItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_warehouse_item.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include <memory.h>

#include <algorithm>

using namespace stdA;

CmdWarehouseItem::CmdWarehouseItem(bool _waiter) : pangya_db(_waiter), m_uid(0), m_type(ALL), m_item_id(-1), v_wi() {
}

CmdWarehouseItem::CmdWarehouseItem(uint32_t _uid, TYPE _type, int32_t _item_id, bool _waiter) 
	: pangya_db(_waiter), m_uid(_uid), m_type(_type), m_item_id(_item_id), v_wi() {
}

CmdWarehouseItem::~CmdWarehouseItem() {
}

void CmdWarehouseItem::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(45, (uint32_t)_result->cols);

	WarehouseItemEx wi{ 0 };
	auto i = 0;

	wi.id = IFNULL(atoi, _result->data[0]);
	wi._typeid = IFNULL(atoi, _result->data[2]);
	wi.ano = IFNULL(atoi, _result->data[3]);
	for (i = 0; i < 5; i++)
		wi.c[i] = (short)IFNULL(atoi, _result->data[4 + i]);			// 4 + 5
	wi.purchase = (unsigned char)IFNULL(atoi, _result->data[9]);
	wi.flag = (unsigned char)IFNULL(atoi, _result->data[11]);
	//wi.apply_date = IFNULL(atoll, _result->data[12]);
	//wi.end_date = IFNULL(atoll, _result->data[13]);

	// Salve local unix date on WarehouseItemEx and System Unix Date on apply_date to send to client
	wi.apply_date_unix_local = IFNULL(atoi, _result->data[12]);
	wi.end_date_unix_local = IFNULL(atoi, _result->data[13]);

	// Date 
	if (wi.apply_date_unix_local > 0)
		wi.apply_date = TzLocalUnixToUnixUTC(wi.apply_date_unix_local);

	if (wi.end_date_unix_local > 0)
		wi.end_date = TzLocalUnixToUnixUTC(wi.end_date_unix_local);

	wi.type = (unsigned char)IFNULL(atoi, _result->data[14]);
	for (i = 0; i < 4; i++)
		wi.card.character[i] = IFNULL(atoi, _result->data[15 + i]);	// 15 + 4
	for (i = 0; i < 4; i++)
		wi.card.caddie[i] = IFNULL(atoi, _result->data[19 + i]);		// 19 + 4
	for (i = 0; i < 4; i++)
		wi.card.NPC[i] = IFNULL(atoi, _result->data[23 + i]);			// 23 + 4
	wi.clubset_workshop.flag = (short)IFNULL(atoi, _result->data[27]);
	for (i = 0; i < 5; i++)
		wi.clubset_workshop.c[i] = (short)IFNULL(atoi, _result->data[28 + i]);	// 28 + 5
	wi.clubset_workshop.mastery = IFNULL(atoi, _result->data[33]);
	wi.clubset_workshop.recovery_pts = IFNULL(atoi, _result->data[34]);
	wi.clubset_workshop.level = IFNULL(atoi, _result->data[35]);
	wi.clubset_workshop.rank = IFNULL(atoi, _result->data[36]);
	if (is_valid_c_string(_result->data[37]))
		STRCPY_TO_MEMORY_FIXED_SIZE(wi.ucc.name, sizeof(wi.ucc.name), _result->data[37]);
		//strcpy_s(wi.ucc.name, _result->data[37]);
	if (is_valid_c_string(_result->data[38]))
		STRCPY_TO_MEMORY_FIXED_SIZE(wi.ucc.idx, sizeof(wi.ucc.idx), _result->data[38]);
		//strcpy_s(wi.ucc.idx, _result->data[38]);
	wi.ucc.seq = (unsigned short)IFNULL(atoi, _result->data[39]);
	if (is_valid_c_string(_result->data[40]))
		STRCPY_TO_MEMORY_FIXED_SIZE(wi.ucc.copier_nick, sizeof(wi.ucc.copier_nick), _result->data[40]);
		//strcpy_s(wi.ucc.copier_nick, _result->data[40]);
	wi.ucc.copier = IFNULL(atoi, _result->data[41]);
	wi.ucc.trade = (unsigned char)IFNULL(atoi, _result->data[42]);
	//wi.sd_flag = (unsigned char)IFNULL(atoi, _result->data[43]);
	wi.ucc.status = (unsigned char)IFNULL(atoi, _result->data[44]);

	auto it = v_wi.find(wi.id);

	if (it == v_wi.end() || (v_wi.count(wi.id) == 1 && it->second._typeid != wi._typeid))
		v_wi.insert(std::make_pair(wi.id, wi));
	else if (v_wi.count(wi.id) > 1) {

		auto er = v_wi.equal_range(wi.id);

		it = std::find_if(er.first, er.second/*End*/, [&](auto& _el) {
			return _el.second._typeid == wi._typeid;
		});

		// N�o tem um igual add um novo
		if (it == er.second/*End*/) {

			v_wi.insert(std::make_pair(wi.id, wi));

			_smp::message_pool::getInstance().push(new message("[CmdWarehouseItemInfo::lineResult][WARNING] player[UID=" + std::to_string(m_uid) + "] adicionou WarehouseItem[TYPEID=" 
					+ std::to_string(wi._typeid) + ", ID=" + std::to_string(wi.id) + "], com mesmo id e typeid diferente de outro WarehouseItemEx que tem no multimap", CL_FILE_LOG_AND_CONSOLE));
		}else
			// Tem um WarehouseItemEx com o mesmo ID e TYPEID (DUPLICATA)
			_smp::message_pool::getInstance().push(new message("[CmdWarehouseItemInfo::lineResult][WARNING] player[UID=" + std::to_string(m_uid) + "] tentou adicionar no multimap um WarehouseItem[TYPEID=" 
					+ std::to_string(it->second._typeid) + ", ID=" + std::to_string(it->second.id) + "] com o mesmo ID e TYPEID, DUPLICATA", CL_FILE_LOG_AND_CONSOLE));

	}else // Tem um WarehouseItemEx com o mesmo ID e TYPEID (DUPLICATA)
		_smp::message_pool::getInstance().push(new message("[CmdWarehouseItemInfo::lineResult][WARNING] player[UID=" + std::to_string(m_uid) + "] tentou adicionar no multimap um WarehouseItem[TYPEID="
				+ std::to_string(it->second._typeid) + ", ID=" + std::to_string(it->second.id) + "] com o mesmo ID e TYPEID, DUPLICATA", CL_FILE_LOG_AND_CONSOLE));
}

response* CmdWarehouseItem::prepareConsulta(database& _db) {
	
	v_wi.clear();
	//v_wi.shrink_to_fit();

	auto r = procedure(_db, (m_type == ALL) ? m_szConsulta[0] : m_szConsulta[1], std::to_string(m_uid) + (m_type == ONE ? ", " + std::to_string(m_item_id) : std::string()));

	checkResponse(r, "nao conseguiu pegar o(s) WarehouseItem(ns) do player: " + std::to_string(m_uid));

	return r;
}

std::multimap< int32_t/*ID*/, WarehouseItemEx >& CmdWarehouseItem::getInfo() {
	return v_wi;
}

uint32_t CmdWarehouseItem::getUID() {
	return m_uid;
}

void CmdWarehouseItem::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CmdWarehouseItem::TYPE CmdWarehouseItem::getType() {
	return m_type;
}

void CmdWarehouseItem::setType(TYPE _type) {
	m_type = _type;
}

int32_t CmdWarehouseItem::getItemID() {
	return m_item_id;
}

void CmdWarehouseItem::setItemID(int32_t _item_id) {
	m_item_id = _item_id;
}
