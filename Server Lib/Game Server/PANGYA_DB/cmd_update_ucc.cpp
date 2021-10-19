// Arquivo cmd_update_ucc.cpp
// Criado em 14/07/2018 as 20:16 por Acrisio
// Implementa��o da classe CmdUpdateUCC

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_ucc.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdUpdateUCC::CmdUpdateUCC(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_wi{ 0 }, m_dt_draw{ 0 }, m_type(TEMPORARY) {
}

CmdUpdateUCC::CmdUpdateUCC(uint32_t _uid, WarehouseItemEx& _wi, SYSTEMTIME& _si, T_UPDATE _type, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_wi(_wi), m_dt_draw(_si), m_type(_type) {
}

CmdUpdateUCC::~CmdUpdateUCC() {
}

void CmdUpdateUCC::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	if (m_type == COPY)
		m_wi.ucc.seq = (unsigned short)IFNULL(atoi, _result->data[0]);
	
	// Ignora o retorno dos outros tipos

	return;
}

response* CmdUpdateUCC::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdSaveUCC::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_wi._typeid == 0)
		throw exception("[CmdSaveUCC::prepareConsulta][Error] m_typeid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_wi.ucc.idx[0] == '\0' || m_wi.ucc.name[0] == '\0')
		throw exception("[CmdSaveUCC::prepareConsulta][Error] UCC[IDX=" + std::string(m_wi.ucc.idx) + ", NAME=" + std::string(m_wi.ucc.name) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_wi.id) + ", " + _db.makeText(m_wi.ucc.idx) + ", " + _db.makeText(m_wi.ucc.name)
			+ ", " + ((m_dt_draw.wYear == 0) ? std::string("NULL") : _db.makeText(_formatDate(m_dt_draw))) 
			+ ", " + ((m_wi.ucc.copier_nick[0] == '\0') ? std::string("NULL") : _db.makeText(m_wi.ucc.copier_nick))
			+ ", " + std::to_string(m_wi.ucc.copier)
			+ ", " + std::to_string((unsigned short)m_wi.ucc.status)
			+ ", " + _db.makeText(((m_type == T_UPDATE::TEMPORARY) ? "T" : "Y"))
			+ ", " + std::to_string(m_type)
	);

	checkResponse(r, "nao conseguiu salvar o UCC[TYPEID=" + std::to_string(m_wi._typeid) + ", ID=" + std::to_string(m_wi.id) + ", UCCIDX=" 
			+ std::string(m_wi.ucc.idx) + ", NAME=" + std::string(m_wi.ucc.name) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateUCC::getUID() {
	return m_uid;
}

void CmdUpdateUCC::setUID(uint32_t _uid) {
	m_uid = _uid;
}

SYSTEMTIME& CmdUpdateUCC::getDrawDate() {
	return m_dt_draw;
}

void CmdUpdateUCC::setDrawDate(SYSTEMTIME& _si) {
	m_dt_draw = _si;
}

CmdUpdateUCC::T_UPDATE CmdUpdateUCC::getType() {
	return m_type;
}

void CmdUpdateUCC::setType(T_UPDATE _type) {
	m_type = _type;
}

WarehouseItemEx& CmdUpdateUCC::getInfo() {
	return m_wi;
}

void CmdUpdateUCC::setInfo(WarehouseItemEx& _wi) {
	m_wi = _wi;
}
