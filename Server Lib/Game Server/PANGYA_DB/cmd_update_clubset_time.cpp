// Arquivo cmd_update_clubset_time.cpp
// Criado em 01/11/2020 as 03:01 por Acrisio
// Implementa��o da classe CmdUpdateClubSetItem

#if defined(_WIN32)
#pragma pack(1)
#endif
#include "cmd_update_clubset_time.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdUpdateClubSetTime::CmdUpdateClubSetTime(uint32_t _uid, WarehouseItemEx& _wi, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_wi(_wi) {
}

CmdUpdateClubSetTime::CmdUpdateClubSetTime(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_wi{ 0u } {
}

CmdUpdateClubSetTime::~CmdUpdateClubSetTime() {
}

void CmdUpdateClubSetTime::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateClubSetTime::prepareConsulta(database& _db) {
	
	if (m_uid == 0u)
		throw exception("[CmdUpdateClubSetTime::prepareConsulta][Error] m_uid is invalid(" 
				+ std::to_string(m_uid) + ")", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_wi.id <= 0)
		throw exception("[CmdUpdateClubSetTime::prepareConsulta][Error] m_wi.id is invalid("
				+ std::to_string(m_wi.id) + ")", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_wi._typeid == 0u)
		throw exception("[CmdUpdateClubSetTime::prepareConsulta][Error] m_wi._typeid is invalid("
				+ std::to_string(m_wi._typeid) + ")", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_wi.id) + ", " + std::to_string(m_wi._typeid) + ", " + _db.makeText(formatDateLocal(m_wi.end_date_unix_local)));

	checkResponse(r, "nao conseguiu atualizar o tempo do ClubSet[ID=" + std::to_string(m_wi.id) 
			+ ", TYPEID=" + std::to_string(m_wi._typeid) + ", ENDDATE=" + formatDateLocal(m_wi.end_date_unix_local) 
			+ "] do Player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateClubSetTime::getUID() {
	return m_uid;
}

void CmdUpdateClubSetTime::setUID(uint32_t _uid) {
	m_uid = _uid;
}

WarehouseItemEx& CmdUpdateClubSetTime::getClubSet() {
	return m_wi;
}

void CmdUpdateClubSetTime::setClubSet(WarehouseItemEx& _wi) {
	m_wi = _wi;
}
