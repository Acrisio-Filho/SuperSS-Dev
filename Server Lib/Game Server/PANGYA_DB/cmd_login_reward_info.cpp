// Arquivo cmd_login_reward_info.cpp
// Criado em 27/10/2020 as 18:54 por Acrisio
// Implementa��o da classe CmdLoginRewardInfo

#if defined(_WIN32)
#pragma pack(1)
#endif
#include "cmd_login_reward_info.hpp"

using namespace stdA;

CmdLoginRewardInfo::CmdLoginRewardInfo(bool _waiter) : pangya_db(_waiter), m_lr() {
}

CmdLoginRewardInfo::~CmdLoginRewardInfo() {

	if (!m_lr.empty()) {
		m_lr.clear();
		m_lr.shrink_to_fit();
	}
}

void CmdLoginRewardInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(10, (uint32_t)_result->cols);

	stLoginReward::stItemReward item(
		(uint32_t)IFNULL(atoi, _result->data[5]), 
		(uint32_t)IFNULL(atoi, _result->data[6]),
		(uint32_t)IFNULL(atoi, _result->data[7])
	);

	SYSTEMTIME end_date{ 0u };

	if (_result->data[9] != nullptr)
		_translateDate(_result->data[9], &end_date);

	m_lr.push_back(stLoginReward(
		(uint64_t)IFNULL(atoll, _result->data[0]),
		stLoginReward::eTYPE((unsigned char)IFNULL(atoi, _result->data[2])),
		_result->data[1],
		(uint32_t)IFNULL(atoi, _result->data[3]),
		(uint32_t)IFNULL(atoi, _result->data[4]),
		item,
		end_date,
		(IFNULL(atoi, _result->data[8]) ? true : false)
	));
}

response* CmdLoginRewardInfo::prepareConsulta(database& _db) {
	
	if (!m_lr.empty())
		m_lr.clear();

	auto r = consulta(_db, m_szConsulta);

	checkResponse(r, "nao conseguiu pegar o Login Reward Info");

	return r;
}

std::vector< stLoginReward >& CmdLoginRewardInfo::getInfo() {
	return m_lr;
}
