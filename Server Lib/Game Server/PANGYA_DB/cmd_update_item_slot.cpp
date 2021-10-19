// Arquivo cmd_update_item_slot.cpp
// Criado em 25/03/2018 as 10:05 por Acrisio
// Implementa��o da classe CmdUpdateItemSlot

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_item_slot.hpp"

#include <memory.h>

using namespace stdA;

CmdUpdateItemSlot::CmdUpdateItemSlot(bool _waiter) : pangya_db(_waiter), m_uid(0), m_slot{0} {
}

CmdUpdateItemSlot::CmdUpdateItemSlot(uint32_t _uid, uint32_t *_slot, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_slot{0} {
	
	if (_slot == nullptr)
		throw exception("[CmdUpdateItemSlot::CmdUpdateItemSlot][Error] _slot is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 6, 0));
	
#if defined(_WIN32)
	memcpy_s(m_slot, sizeof(m_slot), _slot, sizeof(m_slot));
#elif defined(__linux__)
	memcpy(m_slot, _slot, sizeof(m_slot));
#endif
}

CmdUpdateItemSlot::~CmdUpdateItemSlot() {
}

void CmdUpdateItemSlot::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um update
	return;
}

response* CmdUpdateItemSlot::prepareConsulta(database& _db) {

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_slot[0]) + m_szConsulta[1] + std::to_string(m_slot[1]) + m_szConsulta[2] + std::to_string(m_slot[2])
		+ m_szConsulta[3] + std::to_string(m_slot[3]) + m_szConsulta[4] + std::to_string(m_slot[4]) + m_szConsulta[5] + std::to_string(m_slot[5])
		+ m_szConsulta[6] + std::to_string(m_slot[6]) + m_szConsulta[7] + std::to_string(m_slot[7]) + m_szConsulta[8] + std::to_string(m_slot[8])
		+ m_szConsulta[9] + std::to_string(m_slot[9]) + m_szConsulta[10] + std::to_string(m_uid));

	checkResponse(r, "nao conseguiud atualizar o item slot do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdateItemSlot::getUID() {
	return m_uid;
}

void CmdUpdateItemSlot::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t* CmdUpdateItemSlot::getSlot() {
	return m_slot;
}

void CmdUpdateItemSlot::setSlot(uint32_t *_slot) {
	
	if (_slot == nullptr)
		throw exception("[CmdUpdateItemSlot::setSlot][Error] _slot is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 6, 0));

#if defined(_WIN32)
	memcpy_s(m_slot, sizeof(m_slot), _slot, sizeof(m_slot));
#elif defined(__linux__)
	memcpy(m_slot, _slot, sizeof(m_slot));
#endif
}
