// Arquivo cmd_update_mascot_info.cpp
// Criado em 23/09/2018 as 20:46 por Acrisio
// Implementa��o da classe CmdUpdateMascotInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_mascot_info.hpp"

#include "../../Projeto IOCP/UTIL/util_time.h"

#include <string>

using namespace stdA;

CmdUpdateMascotInfo::CmdUpdateMascotInfo(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_mi{0} {
}

CmdUpdateMascotInfo::CmdUpdateMascotInfo(uint32_t _uid, MascotInfoEx& _mi, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_mi(_mi) {
}

CmdUpdateMascotInfo::~CmdUpdateMascotInfo() {
}

void CmdUpdateMascotInfo::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateMascotInfo::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdUpdateMascotInfo::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_mi.id < 0 || m_mi._typeid == 0u)
		throw exception("[CmdUpdateMascotInfo::prepareConsulta][Error] MascotInfoEx m_mi[TYPEID=" + std::to_string(m_mi._typeid) + ", ID=" 
				+ std::to_string(m_mi.id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 1));

	auto end_dt = _formatDate(m_mi.data);

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_mi.id) + ", " + std::to_string(m_mi._typeid)
			+ ", " + std::to_string((unsigned short)m_mi.level) + ", " + std::to_string(m_mi.exp) + ", " + std::to_string((unsigned short)m_mi.flag)
			+ ", " + std::to_string(m_mi.tipo) + ", " + std::to_string((unsigned short)m_mi.is_cash) + ", " + std::to_string(m_mi.price)
			+ ", " + _db.makeText(m_mi.message) + ", " + _db.makeText(end_dt)
	);

	checkResponse(r, "player[UID=" + std::to_string(m_uid) + "] nao conseguiu Atualizar Mascot Info[TYPEID=" + std::to_string(m_mi._typeid) + ", ID=" 
			+ std::to_string(m_mi.id) + ", LEVEL=" + std::to_string((unsigned short)m_mi.level) + ", EXP=" + std::to_string(m_mi.exp) + ", FLAG=" 
			+ std::to_string((unsigned short)m_mi.flag) + ", TIPO=" + std::to_string(m_mi.tipo) + ", IS_CASH=" + std::to_string((unsigned short)m_mi.is_cash) + ", PRICE=" 
			+ std::to_string(m_mi.price) + ", MESSAGE=" + std::string(m_mi.message) + ", END_DT=" + end_dt + "]");

	return r;
}

uint32_t CmdUpdateMascotInfo::getUID() {
	return m_uid;
}

void CmdUpdateMascotInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}

MascotInfoEx& CmdUpdateMascotInfo::getInfo() {
	return m_mi;
}

void CmdUpdateMascotInfo::setInfo(MascotInfoEx& _mi) {
	m_mi = _mi;
}
