// Arquivo cmd_add_warehouse_item.cpp
// Criado em 25/03/2018 as 19:30 por Acrisio
// Implementa��o da classe CmdAddWarehouseItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_warehouse_item.hpp"
#include "../../Projeto IOCP/TYPE/data_iff.h"

using namespace stdA;

CmdAddWarehouseItem::CmdAddWarehouseItem(bool _waiter) : CmdAddItemBase(_waiter), m_wi{0} {
}

CmdAddWarehouseItem::CmdAddWarehouseItem(uint32_t _uid, WarehouseItemEx& _wi, unsigned char _purchase, unsigned char _gift_flag, unsigned short _flag_iff, bool _waiter)
		: CmdAddItemBase(_uid, _purchase, _gift_flag, _waiter), m_wi(_wi), m_flag_iff(_flag_iff) {
}

CmdAddWarehouseItem::~CmdAddWarehouseItem() {
}

void CmdAddWarehouseItem::lineResult(result_set::ctx_res* _result, uint32_t _index_result) {

	if ((m_flag_iff == IFF::Part::UCC_BLANK || m_flag_iff == IFF::Part::UCC_COPY) && _result->cols == 3) {	// UCC(Self Design)

		m_wi.id = IFNULL(atoi, _result->data[0]);
		if (is_valid_c_string(_result->data[1]))
			STRCPY_TO_MEMORY_FIXED_SIZE(m_wi.ucc.idx, sizeof(m_wi.ucc.idx), _result->data[1]);
		m_wi.ucc.seq = (short)IFNULL(atoi, _result->data[2]);

	}else if (!(m_flag_iff == IFF::Part::UCC_BLANK || m_flag_iff == IFF::Part::UCC_COPY) && (_index_result == 1 || _index_result == 0)) {		// Normal Item
		
		checkColumnNumber(1, (uint32_t)_result->cols);

		m_wi.id = IFNULL(atoi, _result->data[0]);
	}
}

response* CmdAddWarehouseItem::prepareConsulta(database& _db) {

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string((unsigned short)m_gift_flag) + ", " + std::to_string((unsigned short)m_purchase)
				+ ", " + std::to_string(m_wi.id) + ", " + std::to_string(m_wi._typeid) + ", " + std::to_string((unsigned short)m_wi.flag)
				+ ", " + std::to_string(m_flag_iff) + ", " + std::to_string(m_wi.STDA_C_ITEM_TIME) + ", " + std::to_string(m_wi.c[0])
				+ ", " + std::to_string(m_wi.c[1]) + ", " + std::to_string(m_wi.c[2]) + ", " + std::to_string(m_wi.c[3])
				+ ", " + std::to_string(m_wi.c[4]) + ", " + std::to_string(0.f) + ", " + std::to_string(0.f) 
				+ ", " + std::to_string(0.f) + ", " + std::to_string(0.f)

	);

	checkResponse(r, "nao conseguiu adicionar o warehouse item[TYPEID=" + std::to_string(m_wi._typeid) + "] do player: " + std::to_string(m_uid));

	return r;
}

WarehouseItemEx& CmdAddWarehouseItem::getInfo() {
	return m_wi;
}

void CmdAddWarehouseItem::setInfo(WarehouseItemEx& _wi) {
	m_wi = _wi;
}

unsigned short CmdAddWarehouseItem::getFlagIff() {
	return m_flag_iff;
}

void CmdAddWarehouseItem::setFlagIff(unsigned short _flag_iff) {
	m_flag_iff = _flag_iff;
}
