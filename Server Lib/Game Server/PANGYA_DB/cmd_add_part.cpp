// Arquivo cmd_add_part.cpp
// Criado em 31/05/2018 as 11:15 por Acrisio
// Implementa��o da classe CmdAddPart

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_part.hpp"

using namespace stdA;

CmdAddPart::CmdAddPart(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_purchase(0), m_gift_flag(0), m_type_iff(0), m_wi{0} {
}

CmdAddPart::CmdAddPart(uint32_t _uid, WarehouseItemEx& _wi, unsigned char _purchase, unsigned char _gift_flag, unsigned char _type_iff, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_purchase(_purchase), m_gift_flag(_gift_flag), m_type_iff(_type_iff), m_wi(_wi) {
}

CmdAddPart::~CmdAddPart() {
}

void CmdAddPart::lineResult(result_set::ctx_res* _result, uint32_t _index_result) {

	if (_index_result == 0 && (m_type_iff == 8 || m_type_iff == 9) && _result->cols == 3) {	// Part UCC [Self Design]

		m_wi.id = IFNULL(atoi, _result->data[0]);
		
		if (is_valid_c_string(_result->data[1]))
			STRCPY_TO_MEMORY_FIXED_SIZE(m_wi.ucc.idx, sizeof(m_wi.ucc.idx), _result->data[1]);
		
		m_wi.ucc.seq = (unsigned short)IFNULL(atoi, _result->data[2]);
	}else if (_index_result == 0 && !(m_type_iff == 8 || m_type_iff == 9) && _result->cols == 3) { // Parts Normal
		
		m_wi.id = IFNULL(atoi, _result->data[0]);
	}else
		checkColumnNumber(1, (uint32_t)_result->cols);	// S� para lan�a a exception, por que eu verificou em cima o n�mero das colunas retornadas
}

response* CmdAddPart::prepareConsulta(database& _db) {

	if (m_wi._typeid == 0)
		throw exception("[CmdAddPart::prepareConsulta][Error] Part is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string((unsigned short)m_gift_flag) + ", " + std::to_string((unsigned short)m_purchase)
			+ ", " + std::to_string(m_wi.id) + ", " + std::to_string(m_wi._typeid) + ", " + std::to_string((unsigned short)m_wi.flag)
			+ ", " + std::to_string((unsigned short)m_type_iff) + ", " + std::to_string(m_wi.STDA_C_ITEM_TIME) + ", " + std::to_string(m_wi.c[0])
			+ ", " + std::to_string(m_wi.c[1]) + ", " + std::to_string(m_wi.c[2]) + ", " + std::to_string(m_wi.c[3]) + ", " + std::to_string(m_wi.c[4])
	);

	checkResponse(r, "nao conseguiu adicionar Part[TYPEID=" + std::to_string(m_wi._typeid) + "] para o player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdAddPart::getUID() {
	return m_uid;
}

void CmdAddPart::setUID(uint32_t _uid) {
	m_uid = _uid;
}

unsigned char CmdAddPart::getPurchase() {
	return m_purchase;
}

void CmdAddPart::setPurchase(unsigned char _purchase) {
	m_purchase = _purchase;
}

unsigned char CmdAddPart::getGiftFlag() {
	return m_gift_flag;
}

void CmdAddPart::setGiftFlag(unsigned char _gift_flag) {
	m_gift_flag = _gift_flag;
}

unsigned char CmdAddPart::getTypeIFF() {
	return m_type_iff;
}

void CmdAddPart::setTypeIFF(unsigned char _type_iff) {
	m_type_iff = _type_iff;
}

WarehouseItemEx& CmdAddPart::getInfo() {
	return m_wi;
}

void CmdAddPart::setInfo(WarehouseItemEx& _wi) {
	m_wi = _wi;
}
