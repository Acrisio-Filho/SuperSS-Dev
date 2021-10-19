// Arquivo cmd_use_item_buff.cpp
// Criado em 07/07/2018 as 21:42 por Acrisio
// Implementa��o da classe CmdUseItemBuff

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_use_item_buff.hpp"

using namespace stdA;

CmdUseItemBuff::CmdUseItemBuff(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_time(0u), m_ib{0} {
}

CmdUseItemBuff::CmdUseItemBuff(uint32_t _uid, ItemBuffEx& _ib, uint32_t _time, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_time(_time), m_ib(_ib) {
}

CmdUseItemBuff::~CmdUseItemBuff() {
}

void CmdUseItemBuff::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_ib.index = IFNULL(atoll, _result->data[0]);

	if (m_ib.index < 0)
		throw exception("[CmdUseItemBuff::lineResult][Error] m_ib[index=" + std::to_string(m_ib.index) + "] is invalid, nao conseguiu usar o Item Buff[TYPEID=" 
				+ std::to_string(m_ib._typeid) + "] para o player[UID=" + std::to_string(m_uid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 1));

	return;
}

response* CmdUseItemBuff::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdUseItemBuff::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_ib._typeid == 0 || m_time == 0)
		throw exception("[CmdUseItemBuff::prepareConsulta][Error] m_ib[TYPEID=" + std::to_string(m_ib._typeid) + ", TEMPO=" + std::to_string(m_time) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ib._typeid) + ", " 
				+ std::to_string(m_ib.tipo) + ", " + std::to_string(m_ib.percent) + ", " + std::to_string(m_time));

	m_ib.index = -1;

	checkResponse(r, "nao conseguiu usar Item[TYPEID=" + std::to_string(m_ib._typeid) + ", TIPO=" + std::to_string(m_ib.tipo) + ", PERCENT=" 
			+ std::to_string(m_ib.percent) + ", TEMPO=" + std::to_string(m_time) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUseItemBuff::getUID() {
	return m_uid;
}

void CmdUseItemBuff::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdUseItemBuff::getTime() {
	return m_time;
}

void CmdUseItemBuff::setTime(uint32_t _time) {
	m_time = _time;
}

ItemBuffEx& CmdUseItemBuff::getInfo() {
	return m_ib;
}

void CmdUseItemBuff::setInfo(ItemBuffEx& _ib) {
	m_ib = _ib;
}
