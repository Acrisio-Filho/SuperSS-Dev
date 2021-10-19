// Arquivo cmd_add_item_base.cpp
// Criado em 25/03/2018 as 18:46 por Acrisio
// Implementa��o da classe CmdAddItemBase

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_item_base.hpp"

using namespace stdA;

CmdAddItemBase::CmdAddItemBase(bool _waiter) : pangya_db(_waiter), m_uid(0), m_purchase(0), m_gift_flag(0) {
}

CmdAddItemBase::CmdAddItemBase(uint32_t _uid, unsigned char _purchase, unsigned char _gift_flag, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_purchase(_purchase), m_gift_flag(_gift_flag) {
}

CmdAddItemBase::~CmdAddItemBase() {
}

uint32_t CmdAddItemBase::getUID() {
	return m_uid;
}

void CmdAddItemBase::setUID(uint32_t _uid) {
	m_uid = _uid;
}

unsigned char CmdAddItemBase::getPurchase() {
	return m_purchase;
}

void CmdAddItemBase::setPurchase(unsigned char _purchase) {
	m_purchase = _purchase;
}

unsigned char CmdAddItemBase::getGiftFlag() {
	return m_gift_flag;
}

void CmdAddItemBase::setGiftFlag(unsigned char _gift_flag) {
	m_gift_flag = _gift_flag;
}
