// Arquivo cmd_approach_missions.cpp
// Criado em 14/06/2020 as 17:23 por Acrisio
// Implementa��o da classe CmdApproachMissions

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "cmd_approach_missions.hpp"

using namespace stdA;

CmdApproachMissions::CmdApproachMissions(bool _waiter) : pangya_db(_waiter) {
}

CmdApproachMissions::~CmdApproachMissions() {

	if (!m_missions.empty()) {
		m_missions.clear();
		m_missions.shrink_to_fit();
	}
}

void CmdApproachMissions::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(5, (uint32_t)_result->cols);

	mission_approach_dados mad{ 0 };

	mad.numero = IFNULL(atoi, _result->data[0]);
	mad.tipo = eMISSION_TYPE(IFNULL(atoi, _result->data[1]));
	mad.reward_tipo = IFNULL(atoi, _result->data[2]);
	mad.box = IFNULL(atoi, _result->data[3]);
	mad.flag.flag = IFNULL(atoi, _result->data[4]);

	m_missions.push_back(mad);
}

response* CmdApproachMissions::prepareConsulta(database& _db) {

	if (!m_missions.empty()) {
		m_missions.clear();
		m_missions.shrink_to_fit();
	}

	auto r = procedure(_db, m_szConsulta, "");

	checkResponse(r, "nao conseguiu pegar as missions do approach");

	return r;
}

std::vector< mission_approach_dados >& CmdApproachMissions::getInfo() {
	return m_missions;
}
