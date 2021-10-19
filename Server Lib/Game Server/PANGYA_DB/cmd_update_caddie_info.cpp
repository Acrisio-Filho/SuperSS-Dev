// Arquivo cmd_update_caddie_info.cpp
// Criado em 23/09/2018 as 20:00 por Acrisio
// Implementa��o da classe CmdUpdateCaddieInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_caddie_info.hpp"

#include "../../Projeto IOCP/UTIL/util_time.h"

#include <string>

using namespace stdA;

CmdUpdateCaddieInfo::CmdUpdateCaddieInfo(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_ci{0} {
}

CmdUpdateCaddieInfo::CmdUpdateCaddieInfo(uint32_t _uid, CaddieInfoEx& _ci, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_ci(_ci) {
}

CmdUpdateCaddieInfo::~CmdUpdateCaddieInfo() {
}

void CmdUpdateCaddieInfo::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateCaddieInfo::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdUpdateCaddieInfo::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_ci.id < 0 || m_ci._typeid == 0u)
		throw exception("[CmdUpdateCaddieInfo::prepareConsulta][Error] CaddieInfo m_ci[TYPEID=" + std::to_string(m_ci._typeid) + ", ID=" 
				+ std::to_string(m_ci.id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 1));

	std::string end_dt = "null", parts_end_dt = "null";
	
	if (!isEmpty(m_ci.end_date))
		end_dt = _db.makeText(_formatDate(m_ci.end_date));

	if (!isEmpty(m_ci.end_parts_date))
		parts_end_dt = _db.makeText(_formatDate(m_ci.end_parts_date));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ci.id) + ", " + std::to_string(m_ci._typeid)
			+ ", " + std::to_string(m_ci.parts_typeid) + ", " + std::to_string((unsigned short)m_ci.level) + ", " + std::to_string(m_ci.exp)
			+ ", " + std::to_string((unsigned short)m_ci.rent_flag) + ", " + std::to_string((unsigned short)m_ci.purchase)
			+ ", " + std::to_string(m_ci.check_end) + ", " + end_dt + ", " + parts_end_dt
	);

	checkResponse(r, "player[UID=" + std::to_string(m_uid) + "] nao conseguiu Atualizar o Caddie Info[TYPEID=" + std::to_string(m_ci._typeid) + ", ID=" 
			+ std::to_string(m_ci.id) + ", PARTS_TYPEID=" + std::to_string(m_ci.parts_typeid) + ", LEVEL=" + std::to_string((unsigned short)m_ci.level) + ", EXP=" 
			+ std::to_string(m_ci.exp) + ", RENT_FLAG=" + std::to_string((unsigned short)m_ci.rent_flag) + ", PURCHASE=" + std::to_string((unsigned short)m_ci.purchase) + ", CHECK_END=" 
			+ std::to_string(m_ci.check_end) + ", END_DT=" + end_dt + ", PARTS_END_DT=" + parts_end_dt + "]");

	return r;
}

uint32_t CmdUpdateCaddieInfo::getUID() {
	return m_uid;
}

void CmdUpdateCaddieInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CaddieInfoEx& CmdUpdateCaddieInfo::getInfo() {
	return m_ci;
}

void CmdUpdateCaddieInfo::setInfo(CaddieInfoEx& _ci) {
	m_ci = _ci;
}
