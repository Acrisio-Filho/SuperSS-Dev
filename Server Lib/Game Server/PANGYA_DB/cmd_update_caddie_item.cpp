// Arquivo cmd_update_caddie_item.cpp
// Criado em 14/05/2018 as 00:09 por Acrisio
// Implementa��o da classe CmdUpdateCaddieItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_caddie_item.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"

using namespace stdA;

CmdUpdateCaddieItem::CmdUpdateCaddieItem(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_time(), m_ci{0} {
}

CmdUpdateCaddieItem::CmdUpdateCaddieItem(uint32_t _uid, std::string& _time, CaddieInfoEx& _ci, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_time(_time), m_ci(_ci) {
}

CmdUpdateCaddieItem::~CmdUpdateCaddieItem() {
}

void CmdUpdateCaddieItem::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateCaddieItem::prepareConsulta(database& _db) {

	if (m_ci.id <= 0 || m_ci._typeid == 0 || m_ci.parts_typeid == 0 || m_time.empty())
		throw exception("[CmdUpdateCaddieItem::prepareConsulta][Error] invalid Caddie[TYPEID=" + std::to_string(m_ci._typeid) 
					+ ", ID=" + std::to_string(m_ci.id) + "] or Caddie Item[TYPEID=" + std::to_string(m_ci.parts_typeid) + ", TIME=" + m_time + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ci.id) + ", " + std::to_string(m_ci.parts_typeid) + ", " + _db.makeText(m_time));

	checkResponse(r, "nao conseguiu atualizar o caddie item[TYPEID=" + std::to_string(m_ci.parts_typeid) + "] do caddie[ID=" + std::to_string(m_ci.id) + "] do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdateCaddieItem::getUID() {
	return m_uid;
}

void CmdUpdateCaddieItem::setUID(uint32_t _uid) {
	m_uid = _uid;
}

std::string& CmdUpdateCaddieItem::getTime() {
	return m_time;
}

void CmdUpdateCaddieItem::setTime(std::string& _time) {
	m_time = _time;
}

CaddieInfoEx& CmdUpdateCaddieItem::getInfo() {
	return m_ci;
}

void CmdUpdateCaddieItem::setInfo(CaddieInfoEx& _ci) {
	m_ci = _ci;
}
