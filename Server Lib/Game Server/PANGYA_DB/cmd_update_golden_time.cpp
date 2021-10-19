// Arquivo cmd_update_golden_time.cpp
// Criado em 24/10/2020 as 03:32 por Acrisio
// Implementa��o da classe CmdUpdateGoldenTime

#if defined(_WIN32)
#pragma pack(1)
#endif
#include "cmd_update_golden_time.hpp"

using namespace stdA;

CmdUpdateGoldenTime::CmdUpdateGoldenTime(bool _waiter) : pangya_db(_waiter), m_id(0u), m_is_end(false) {
}

CmdUpdateGoldenTime::CmdUpdateGoldenTime(uint32_t _id, bool _is_end, bool _waiter)
	: pangya_db(_waiter), m_id(_id), m_is_end(_is_end) {
}

CmdUpdateGoldenTime::~CmdUpdateGoldenTime() {
}

void CmdUpdateGoldenTime::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateGoldenTime::prepareConsulta(database& _db) {
	
	if (m_id == 0u)
		throw exception("[CmdUpdateGoldenTime::prepareConsulta][Error] m_id is invalid(" + std::to_string(m_id) + ")", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = consulta(_db, m_szConsulta[0] + std::to_string(m_is_end ? 1 : 0) + m_szConsulta[1] + std::to_string(m_id));

	checkResponse(r, "nao conseguiu atualizar o Golden Time[ID=" + std::to_string(m_id) + ", IS_END=" + std::string(m_is_end ? "TRUE" : "FALSE") + "]");

	return r;
}

uint32_t CmdUpdateGoldenTime::getId() {
	return m_id;
}

void CmdUpdateGoldenTime::setId(uint32_t _id) {
	m_id = _id;
}

bool CmdUpdateGoldenTime::getIsEnd() {
	return m_is_end;
}

void CmdUpdateGoldenTime::setIsEnd(bool _is_end) {
	m_is_end = _is_end;
}
