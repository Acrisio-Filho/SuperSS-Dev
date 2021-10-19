// Arquivo cmd_create_achievement.cpp
// Criado em 31/03/2018 as 21:56 por Acrisio
// Implementa��o da classe CmdCreateAchievement

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_create_achievement.hpp"

using namespace stdA;

CmdCreateAchievement::CmdCreateAchievement(bool _waiter) : pangya_db(_waiter), m_uid(0), m_typeid(0), m_name(), m_status(0) {
}

CmdCreateAchievement::CmdCreateAchievement(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_typeid(0), m_name(), m_status(0) {
}

CmdCreateAchievement::CmdCreateAchievement(uint32_t _uid, uint32_t _typeid, std::string& _name, uint32_t _status, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_typeid(_typeid), m_name(_name), m_status(_status) {
}

CmdCreateAchievement::~CmdCreateAchievement() {
}

void CmdCreateAchievement::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_id = IFNULL(atoi, _result->data[0]);
}

response* CmdCreateAchievement::prepareConsulta(database& _db) {
	
	if (m_typeid == 0)
		throw exception("[CmdCreateAchievement::prepareConsulta][Error] achievement invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_id = -1;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + _db.makeText(m_name) + ", " + std::to_string(m_typeid) + ", 1, " + std::to_string(m_status));

	checkResponse(r, "nao conseguiu adicionar Achievement para o player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdCreateAchievement::getUID() {
	return m_uid;
}

void CmdCreateAchievement::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdCreateAchievement::getTypeid() {
	return m_typeid;
}

void CmdCreateAchievement::setTypeid(uint32_t _typeid) {
	m_typeid = _typeid;
}

std::string& CmdCreateAchievement::getName() {
	return m_name;
}

void CmdCreateAchievement::setName(std::string& _name) {
	m_name = _name;
}

uint32_t CmdCreateAchievement::getStatus() {
	return m_status;
}

void CmdCreateAchievement::setStatus(uint32_t _status) {
	m_status = _status;
}

void CmdCreateAchievement::setAchievement(uint32_t _typeid, std::string& _name, uint32_t _status) {
	setTypeid(_typeid);
	setName(_name);
	setStatus(_status);
}

int32_t CmdCreateAchievement::getID() {
	return m_id;
}
