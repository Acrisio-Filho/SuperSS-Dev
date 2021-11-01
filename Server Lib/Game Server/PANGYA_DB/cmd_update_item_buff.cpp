// Arquivo cmd_update_item_buff.cpp
// Criado em 07/07/2018 as 22:09 por Acrisio
// Implementa��o da classe CmdUpdateItemBuff

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_item_buff.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdUpdateItemBuff::CmdUpdateItemBuff(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_ib{0} {
}

CmdUpdateItemBuff::CmdUpdateItemBuff(uint32_t _uid, ItemBuffEx& _ib, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_ib(_ib) {
}

CmdUpdateItemBuff::~CmdUpdateItemBuff() {
}

void CmdUpdateItemBuff::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateItemBuff::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdUpdateItemBuff::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_ib.index <= 0 || m_ib._typeid == 0)
		throw exception("[CmdUpdateItemBuff::prepareConsulta][Error] m_ib[index=" + std::to_string(m_ib.index) + ", TYPEID=" + std::to_string(m_ib._typeid) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ib.index) + ", " + std::to_string(m_ib._typeid)
			+ ", " + std::to_string(m_ib.tipo) + ", " + _db.makeText(_formatDate(m_ib.end_date))
	);

	checkResponse(r, "nao conseguiu atualizar o tempo do item buff[INDEX=" + std::to_string(m_ib.index) + ", TYPEID=" + std::to_string(m_ib._typeid) + ", TIPO=" 
			+  std::to_string(m_ib.tipo) + ", DATE{REG_DT: " + _formatDate(m_ib.use_date) + ", END_DT: " + _formatDate(m_ib.end_date) + "}] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateItemBuff::getUID() {
	return m_uid;
}

void CmdUpdateItemBuff::setUID(uint32_t _uid) {
	m_uid = _uid;
}

ItemBuffEx& CmdUpdateItemBuff::getInfo() {
	return m_ib;
}

void CmdUpdateItemBuff::setInfo(ItemBuffEx& _ib) {
	m_ib = _ib;
}
