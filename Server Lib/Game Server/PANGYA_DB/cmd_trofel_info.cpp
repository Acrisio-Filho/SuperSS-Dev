// Arquivo cmd_trofel_info.cpp
// Criado em 18/03/2018 as 17:07 por Acrisio
// Implementação da classe CmdTrofelInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_trofel_info.hpp"

using namespace stdA;

CmdTrofelInfo::CmdTrofelInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_season(CURRENT), m_ti{0} {
}

CmdTrofelInfo::CmdTrofelInfo(uint32_t _uid, TYPE_SEASON _season, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_season(_season), m_ti{0} {
}

CmdTrofelInfo::~CmdTrofelInfo() {
}

void CmdTrofelInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {
	
	checkColumnNumber(39, (uint32_t)_result->cols);

	auto i = 0, j = 0;

	// AMA 6~1
	for (i = 0; i < 6; ++i)
		for (j = 0; j < 3; ++j)
			m_ti.ama_6_a_1[i][j] = (short)IFNULL(atoi, _result->data[(i * 3) + j]); // 0 a (3 * 6) = 18

	// PRO 1~7
	for (i = 0; i < 7; ++i)
		for (j = 0; j < 3; ++j)
			m_ti.pro_1_a_7[i][j] = (short)IFNULL(atoi, _result->data[18 + (i * 3) + j]);	// 18 a (3 * 7) = 39

	/*for (i = 0; i < 3; i++)
		m_ti.ama_6[i] = (short)IFNULL(atoi, _result->data[0 + i]);	// 0 + 3
	for (i = 0; i < 3; i++)
		m_ti.ama_5[i] = (short)IFNULL(atoi, _result->data[3 + i]);	// 3 + 3
	for (i = 0; i < 3; i++)
		m_ti.ama_4[i] = (short)IFNULL(atoi, _result->data[6 + i]);	// 6 + 3
	for (i = 0; i < 3; i++)
		m_ti.ama_3[i] = (short)IFNULL(atoi, _result->data[9 + i]);	// 9 + 3
	for (i = 0; i < 3; i++)
		m_ti.ama_2[i] = (short)IFNULL(atoi, _result->data[12 + i]);	// 12 + 3
	for (i = 0; i < 3; i++)
		m_ti.ama_1[i] = (short)IFNULL(atoi, _result->data[15 + i]);	// 15 + 3
	for (i = 0; i < 3; i++)
		m_ti.pro_1[i] = (short)IFNULL(atoi, _result->data[18 + i]);	// 18 + 3
	for (i = 0; i < 3; i++)
		m_ti.pro_2[i] = (short)IFNULL(atoi, _result->data[21 + i]);	// 21 + 3
	for (i = 0; i < 3; i++)
		m_ti.pro_3[i] = (short)IFNULL(atoi, _result->data[24 + i]);	// 24 + 3
	for (i = 0; i < 3; i++)
		m_ti.pro_4[i] = (short)IFNULL(atoi, _result->data[27 + i]);	// 27 + 3
	for (i = 0; i < 3; i++)
		m_ti.pro_5[i] = (short)IFNULL(atoi, _result->data[30 + i]);	// 30 + 3
	for (i = 0; i < 3; i++)
		m_ti.pro_6[i] = (short)IFNULL(atoi, _result->data[33 + i]);	// 33 + 3
	for (i = 0; i < 3; i++)
		m_ti.pro_7[i] = (short)IFNULL(atoi, _result->data[36 + i]);	// 36 + 3*/
}

response* CmdTrofelInfo::prepareConsulta(database& _db) {
	
	m_ti.clear();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_season));

	checkResponse(r, "nao conseguiu recuperar o trofel info do player: " + std::to_string(m_uid));

	return r;
}

TrofelInfo& CmdTrofelInfo::getInfo() {
	return m_ti;
}

void CmdTrofelInfo::setInfo(TrofelInfo& _ti) {
	m_ti = _ti;
}

uint32_t CmdTrofelInfo::getUID() {
	return m_uid;
}

void CmdTrofelInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CmdTrofelInfo::TYPE_SEASON CmdTrofelInfo::getSeason() {
	return m_season;
}

void CmdTrofelInfo::setSeason(TYPE_SEASON _season) {
	m_season = _season;
}
