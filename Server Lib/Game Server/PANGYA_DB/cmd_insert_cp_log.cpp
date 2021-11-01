// Arquivo cmd_insert_cp_log.cpp
// Criado em 24/05/2019 as 04:20 por Acrisio
// Implementação da classe CmdInsertCPLog

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_insert_cp_log.hpp"

using namespace stdA;

CmdInsertCPLog::CmdInsertCPLog(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(-1ll), m_cp_log{0} {
}

CmdInsertCPLog::CmdInsertCPLog(uint32_t _uid, CPLog& _cp_log, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_cp_log(_cp_log), m_id(-1ll) {
}

CmdInsertCPLog::~CmdInsertCPLog() {
}

void CmdInsertCPLog::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_id = IFNULL(atoll, _result->data[0]);
}

response* CmdInsertCPLog::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdInsertCPLog::prepareConsulta][Error] m_uid is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_id = -1ll;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string((unsigned short)m_cp_log.getType())
			+ ", " + std::to_string(m_cp_log.getMailId()) + ", " + std::to_string(m_cp_log.getCookie()) + ", " + std::to_string(m_cp_log.getItemCount())
	);

	checkResponse(r, "nao conseguiu inserir o CPLog[" + m_cp_log.toString() + "] do Player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdInsertCPLog::getUID() {
	return m_uid;
}

void CmdInsertCPLog::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CPLog& CmdInsertCPLog::getLog() {
	return m_cp_log;
}

void CmdInsertCPLog::setLog(CPLog& _cp_log) {
	m_cp_log = _cp_log;
}

int64_t CmdInsertCPLog::getId() {
	return m_id;
}
