// Arquivo cmd_daily_quest_info.cpp
// Criado em 07/04/2018 as 20:15 por Acrisio
// Implementa��o da classe CmdDailyQuestInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_daily_quest_info.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdDailyQuestInfo::CmdDailyQuestInfo(bool _waiter) : pangya_db(_waiter), m_dqi{0} {
}

CmdDailyQuestInfo::~CmdDailyQuestInfo() {
}

void CmdDailyQuestInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(4, (uint32_t)_result->cols);

	for (auto i = 0u; i < 3; ++i)
		m_dqi._typeid[i] = IFNULL(atoi, _result->data[0 + i]);		// 0 + 3 = 3
	
	/*if (is_valid_c_string(_result->data[3]))
		STRCPY_TO_MEMORY_FIXED_SIZE(m_dqi.date, sizeof(m_dqi.date), _result->data[3]);*/

	if (_result->data[3] != nullptr)
		_translateDate(_result->data[3], &m_dqi.date);
}

response* CmdDailyQuestInfo::prepareConsulta(database& _db) {

	m_dqi.clear();

	auto r = consulta(_db, m_szConsulta);

	checkResponse(r, "nao conseguiu pegar o Daily Quest Info");

	return r;
}

DailyQuestInfo& CmdDailyQuestInfo::getInfo() {
	return m_dqi;
}

void CmdDailyQuestInfo::setInfo(DailyQuestInfo& _dqi) {
	m_dqi = _dqi;
}
