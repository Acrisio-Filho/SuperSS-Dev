// Arquivo cmd_notice_info.cpp
// Criado em 03/12/2018 as 00:53 por Acrisio
// Implementa��o da classe CmdNoticeInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_notice_info.hpp"
#include <cstring>

using namespace stdA;

CmdNoticeInfo::CmdNoticeInfo(bool _waiter) : pangya_db(_waiter), m_id(0u), m_message("") {
}

CmdNoticeInfo::CmdNoticeInfo(uint32_t _id, bool _waiter)
	: pangya_db(_waiter), m_id(_id), m_message("") {
}

CmdNoticeInfo::~CmdNoticeInfo() {
}

void CmdNoticeInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	if (_result->data[0] != nullptr) {

		auto len = strlen(_result->data[0]);
		
		if (len > 0) {
			char *tmp = new char[len + 1];

#if defined(_WIN32)
			memcpy_s(tmp, len, _result->data[0], len);
#elif defined(__linux__)
			memcpy(tmp, _result->data[0], len);
#endif

			tmp[len] = '\0';

			m_message = tmp;

			delete[] tmp;
		}
	}
}

response* CmdNoticeInfo::prepareConsulta(database& _db) {

	if (m_id == 0u)
		throw exception("[CmdNoticeInfo::prepareConsulta][Error] m_id is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (!m_message.empty()) {
		m_message.clear();
		m_message.shrink_to_fit();
	}

	auto r = procedure(_db, m_szConsulta, std::to_string(m_id));

	checkResponse(r, "nao conseguiu pegar a Notice do server Info[COMMAND_ID=" + std::to_string(m_id) + "]");

	return r;
}

uint32_t CmdNoticeInfo::getId() {
	return m_id;
}

void CmdNoticeInfo::setId(uint32_t _id) {
	m_id = _id;
}

std::string CmdNoticeInfo::getInfo() {
	return m_message;
}
