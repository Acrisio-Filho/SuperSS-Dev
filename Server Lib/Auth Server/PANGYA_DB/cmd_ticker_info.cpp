// Arquivo cmd_ticker_info.cpp
// Criado em 03/12/2018 as 15:03 por Acrisio
// Implementa��o da classe CmdTickerInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_ticker_info.hpp"
#include "../../Projeto IOCP/UTIL/message_pool.h"

using namespace stdA;

CmdTickerInfo::CmdTickerInfo(bool _waiter) : pangya_db(_waiter), m_id(0u), m_ti{0} {
}

CmdTickerInfo::CmdTickerInfo(uint32_t _id, bool _waiter)
	: pangya_db(_waiter), m_id(_id), m_ti{0} {
}

CmdTickerInfo::~CmdTickerInfo() {
}

void CmdTickerInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(2, (uint32_t)_result->cols);

	char tmp[1024];
	int32_t len = 0;

	if (_result->data[0] != nullptr) {
		
		if ((len = (int)strlen(_result->data[0])) > sizeof(tmp))
			_smp::message_pool::getInstance().push(new message("[CmdTickerInfo::lineResult][WARNING] size of string great of variable, truncate variable data[0].", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
		memcpy_s(tmp, sizeof(tmp), _result->data[0], len);
#elif defined(__linux__)
		memcpy(tmp, _result->data[0], len);
#endif

		tmp[(len >= sizeof(tmp) ? sizeof(tmp) - 1 : len)] = '\0';

		m_ti.msg = tmp;
	}

	if (_result->data[1] != nullptr) {

		if ((len = (int)strlen(_result->data[1])) > sizeof(tmp))
			_smp::message_pool::getInstance().push(new message("[CmdTickerInfo::lineResult][WARNING] size of string great of variable, truncate variable data[1].", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
		memcpy_s(tmp, sizeof(tmp), _result->data[1], len);
#elif defined(__linux__)
		memcpy(tmp, _result->data[1], len);
#endif

		tmp[(len >= sizeof(tmp) ? sizeof(tmp) - 1 : len)] = '\0';

		m_ti.nick = tmp;
	}
}

response* CmdTickerInfo::prepareConsulta(database& _db) {

	if (m_id == 0u)
		throw exception("[CmdTickerInfo::prepareConsulta][Error] m_id is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_ti.clear();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_id));

	checkResponse(r, "nao conseguiu pegar Ticker Info do Server[COMMAND_ID=" + std::to_string(m_id) + "]");

	return r;
}

uint32_t CmdTickerInfo::getId() {
	return m_id;
}

void CmdTickerInfo::setId(uint32_t _id) {
	m_id = _id;
}

TickerInfo& CmdTickerInfo::getInfo() {
	return m_ti;
}
