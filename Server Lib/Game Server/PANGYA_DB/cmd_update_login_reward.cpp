// Arquivo cmd_update_login_reward.cpp
// Criado em 27/10/2020 as 20:05 por Acrisio
// Implementa��o da classe CmdUpdateLoginReward

#if defined(_WIN32)
#pragma pack(1)
#endif
#include "cmd_update_login_reward.hpp"

using namespace stdA;

CmdUpdateLoginReward::CmdUpdateLoginReward(uint64_t _id, bool _is_end, bool _waiter)
	: pangya_db(_waiter), m_id(_id), m_is_end(_is_end) {
}

CmdUpdateLoginReward::CmdUpdateLoginReward(bool _waiter) : pangya_db(_waiter), m_id(0ull), m_is_end(false) {
}

CmdUpdateLoginReward::~CmdUpdateLoginReward() {
}

void CmdUpdateLoginReward::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateLoginReward::prepareConsulta(database& _db) {
	
	if (m_id == 0ull)
		throw exception("[CmdUpdateLoginReward::prepareConsulta][Error] m_id is invalid(" + std::to_string(m_id) 
				+ ")", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = consulta(_db, m_szConsulta[0] + std::string(m_is_end ? "1" : "0") + m_szConsulta[1] + std::to_string(m_id));

	checkResponse(r, "nao conseguiu atualizar o Login Reward[ID=" + std::to_string(m_id) + ", IS_END=" + std::string(m_is_end ? "TRUE" : "FALSE") + "]");

	return r;
}

uint64_t CmdUpdateLoginReward::getId() {
	return m_id;
}

void CmdUpdateLoginReward::setId(uint64_t _id) {
	m_id = _id;
}

bool CmdUpdateLoginReward::getIsEnd() {
	return m_is_end;
}

void CmdUpdateLoginReward::setIsEnd(bool _is_end) {
	m_is_end = _is_end;
}
