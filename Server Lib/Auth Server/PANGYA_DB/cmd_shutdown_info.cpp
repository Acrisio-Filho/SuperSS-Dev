// Arquivo cmd_shutdown_info.cpp
// Criado em 02/12/2018 as 23:19 por Acrisio
// Implementa��o da classe CmdShutdownInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_shutdown_info.hpp"

using namespace stdA;

CmdShutdownInfo::CmdShutdownInfo(bool _waiter) : pangya_db(_waiter), m_id(0u), m_time_sec(0u) {
}

CmdShutdownInfo::CmdShutdownInfo(uint32_t _id, bool _waiter) 
	: pangya_db(_waiter), m_id(_id), m_time_sec(0u) {
}

CmdShutdownInfo::~CmdShutdownInfo() {
}

void CmdShutdownInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_result_index*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_time_sec = IFNULL(atoi, _result->data[0]);

}

response* CmdShutdownInfo::prepareConsulta(database& _db) {

	if (m_id == 0u)
		throw exception("[CmdShutdownInfo::prepareConsulta][Error] m_id is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_id));

	checkResponse(r, "nao conseguiu pegar o shutdown server Info[COMMAND_ID=" + std::to_string(m_id) + "]");

	return r;
}

uint32_t CmdShutdownInfo::getId() {
	return m_id;
}

void CmdShutdownInfo::setId(uint32_t _id) {
	m_id = _id;
}

int32_t CmdShutdownInfo::getInfo() {
	return m_time_sec;
}
