// Arquivo cmd_golden_time_round.cpp
// Criado em 24/10/2020 as 02:37 por Acrisio
// Implementa��o da classe CmdGoldenTimeRound

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_golden_time_round.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

#include <memory.h>

using namespace stdA;

CmdGoldenTimeRound::CmdGoldenTimeRound(uint32_t _id, bool _waiter) : pangya_db(_waiter), m_id(_id), m_round() {
}

CmdGoldenTimeRound::CmdGoldenTimeRound(bool _waiter) : pangya_db(_waiter), m_id(0u), m_round() {
}

CmdGoldenTimeRound::~CmdGoldenTimeRound() {

	if (!m_round.empty()) {
		m_round.clear();
		m_round.shrink_to_fit();
	}
}

void CmdGoldenTimeRound::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	stRound round{ 0u };

	if (_result->data[0] != nullptr)
		_translateTime(_result->data[0], &round.time);

	m_round.push_back(round);
}

response* CmdGoldenTimeRound::prepareConsulta(database& _db) {
	
	if (m_id == 0u)
		throw exception("[CmdGoldenTimeRound::prepareConsulta][Error] m_id is invalid(" + std::to_string(m_id) + ").", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (!m_round.empty())
		m_round.clear();

	auto r = consulta(_db, m_szConsulta + std::to_string(m_id));

	checkResponse(r, "nao conseguiu pegar o round do Golden Time[ID=" + std::to_string(m_id) + "]");

	return r;
}

uint32_t CmdGoldenTimeRound::getId() {
	return m_id;
}

void CmdGoldenTimeRound::setId(uint32_t _id) {
	m_id = _id;
}

std::vector< stRound >& CmdGoldenTimeRound::getInfo() {
	return m_round;
}
