// Arquivo cmd_bot_gm_event_info.cpp
// Criado em 03/11/2020 as 20:59 por Acrisio
// Implementa��o da classe CmdBotGMEventInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "cmd_bot_gm_event_info.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdBotGMEventInfo::CmdBotGMEventInfo(bool _waiter) : pangya_db(_waiter), m_reward(), m_time() {
}

CmdBotGMEventInfo::~CmdBotGMEventInfo() {

	if (!m_reward.empty()) {
		m_reward.clear();
		m_reward.shrink_to_fit();
	}

	if (!m_time.empty()) {
		m_time.clear();
		m_time.shrink_to_fit();
	}
}

void CmdBotGMEventInfo::lineResult(result_set::ctx_res* _result, uint32_t _index_result) {

	if (_index_result == 0u) {

		checkColumnNumber(3, (uint32_t)_result->cols);

		// Time
		stRangeTime rt{ 0u };

		if (_result->data[0] != nullptr)
			_translateTime(_result->data[0], &rt.m_start);

		if (_result->data[1] != nullptr)
			_translateTime(_result->data[1], &rt.m_end);

		rt.m_channel_id = (unsigned char)IFNULL(atoi, _result->data[2]);

		m_time.push_back(rt);

	}else if (_index_result == 1u) {

		checkColumnNumber(4, (uint32_t)_result->cols);

		// Reward
		m_reward.push_back(stReward{
			(uint32_t)IFNULL(atoi, _result->data[0]), // Typeid
			(uint32_t)IFNULL(atoi, _result->data[1]), // Qntd
			(uint32_t)IFNULL(atoi, _result->data[2]), // Qntd Time
			(uint32_t)IFNULL(atoi, _result->data[3])  // Rate
		});
	}

}

response* CmdBotGMEventInfo::prepareConsulta(database& _db) {
	
	auto r = consulta(_db, m_szConsulta);

	checkResponse(r, "nao conseguiu pegar o info do Bot GM Event");

	return r;
}

std::vector< stReward >& CmdBotGMEventInfo::getRewardInfo() {
	return m_reward;
}

std::vector< stRangeTime >& CmdBotGMEventInfo::getTimeInfo() {
	return m_time;
}
